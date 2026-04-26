PROTOCOL_MAP = {
    1: "ICMP",       # Ping ve hata mesajları
    2: "IGMP",       # Multicast grup yönetimi
    6: "TCP",        # Web (HTTP/HTTPS), MQTT gibi güvenilir bağlantılar
    17: "UDP",       # DNS sorguları, NTP (Zaman senkronizasyonu) ve hızlı veri akışları
    47: "GRE",       # Tünelleme protokolü
    50: "ESP",       # IPsec şifreli veri (VPN)
    51: "AH",        # IPsec kimlik doğrulama
    58: "IPv6-ICMP", # IPv6 için Ping ve hata mesajları
    89: "OSPF",      # Yönlendirme protokolü
    132: "SCTP",     # Gelişmiş taşıma protokolü
    255: "Reserved"  # Ayrılmış
}

from scapy.all import sniff, IP

# IP Protokol Numaraları Sözlüğü
PROTOCOL_MAP = {
    1: "ICMP", 2: "IGMP", 6: "TCP", 17: "UDP", 47: "GRE", 
    50: "ESP", 51: "AH", 58: "IPv6-ICMP", 89: "OSPF", 132: "SCTP"
}

def process_packet(packet):
    if packet.haslayer(IP):
        src_ip = packet[IP].src
        dst_ip = packet[IP].dst
        protocol_num = packet[IP].proto
        
        # Numarayı isme çevir, sözlükte yoksa 'Bilinmeyen' yazıp numarasını ekle
        protocol_name = PROTOCOL_MAP.get(protocol_num, f"Bilinmeyen ({protocol_num})")
        
        print(f"Paket Yakalandı -> Kaynak: {src_ip} | Hedef: {dst_ip} | Protokol: {protocol_name}")

print("NIDS Dinleme Başladı... (wlan0 arayüzü izleniyor)")
sniff(iface="wlan0", prn=process_packet, store=0)