using NidsBackendAPI.Services;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddHostedService<MqttListenerService>();

var app = builder.Build();

if (app.Environment.IsDevelopment() || true)
{
    app.UseSwagger();
    app.UseSwaggerUI(options => 
    {
        options.RoutePrefix = "swagger";
    });
}

app.UseAuthorization();
app.MapControllers();


Console.WriteLine("NIDS Backend Sunucusu Baslatiliyor... Port: 5030");
app.Run("http://0.0.0.0:5030");