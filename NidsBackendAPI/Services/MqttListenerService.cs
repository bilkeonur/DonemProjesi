using System.Text.Json;
using MQTTnet;

namespace NidsBackendAPI.Services
{
    public class MqttListenerService : BackgroundService
    {
        private IMqttClient _mqttClient;
        private MqttClientOptions _mqttOptions;

        private readonly JsonSerializerOptions _jsonOptions = new JsonSerializerOptions
        {
            PropertyNameCaseInsensitive = true
        };

        public MqttListenerService()
        {
            var factory = new MqttClientFactory();
            _mqttClient = factory.CreateMqttClient();

            _mqttOptions = new MqttClientOptionsBuilder()
                .WithTcpServer("192.168.1.50", 1883)
                .WithClientId("NIDS_Backend_Listener")
                .WithCleanSession()
                .Build();
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            _mqttClient.ApplicationMessageReceivedAsync += e =>
            {
                string topic = e.ApplicationMessage.Topic;
                string payload = e.ApplicationMessage.ConvertPayloadToString();

                if (topic.StartsWith("zigbee2mqtt/0x"))
                {
                    string deviceMac = topic.Replace("zigbee2mqtt/", "");

                    try
                    {
                        var data = JsonSerializer.Deserialize<ZigbeeData>(payload, _jsonOptions);

                        if (data != null)
                        {
                            string lqiInfo = data.Linkquality.HasValue ? $" | LQI: {data.Linkquality}" : " | LQI: Yok";
                            
                            Console.WriteLine($"[{DateTime.Now:HH:mm:ss}] ZIGBEE VERİSİ -> Cihaz: {deviceMac} " +
                                              $"| Sıcaklık: {data.Temperature}°C{lqiInfo}");

                        }
                    }
                    catch (Exception jsonEx)
                    {
                        Console.WriteLine($"[HATA] JSON Çözümlenemedi: {jsonEx.Message} | Ham Veri: {payload}");
                    }
                }

                return Task.CompletedTask;
            };

            _mqttClient.DisconnectedAsync += async e =>
            {
                if (!stoppingToken.IsCancellationRequested)
                {
                    Console.WriteLine("MQTT Broker bağlantısı koptu. 5 saniye içinde yeniden deneniyor...");
                    await Task.Delay(TimeSpan.FromSeconds(5), stoppingToken);
                    try { await _mqttClient.ConnectAsync(_mqttOptions, stoppingToken); } catch { }
                }
            };

            try
            {
                await _mqttClient.ConnectAsync(_mqttOptions, stoppingToken);
                
                var subscribeOptions = new MqttClientSubscribeOptionsBuilder()
                    .WithTopicFilter(f => f.WithTopic("zigbee2mqtt/#"))
                    .Build();
                
                await _mqttClient.SubscribeAsync(subscribeOptions, stoppingToken);
                
                Console.WriteLine("MQTT Dinleyicisi Başlatıldı. Zigbee verileri NIDS merkezine akıyor...");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"MQTT Bağlantı Hatası: {ex.Message}");
            }

            while (!stoppingToken.IsCancellationRequested)
            {
                await Task.Delay(1000, stoppingToken);
            }
        }
    }
}