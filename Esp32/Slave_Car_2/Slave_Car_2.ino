#include <WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "ESP32_AP";
const char *password = "12345678";

// 设置静态IP地址
IPAddress local_IP(192, 168, 4, 3); // 从机1的静态IP地址
IPAddress gateway(192, 168, 4, 1);  // AP的IP地址
IPAddress subnet(255, 255, 255, 0); // 子网掩码

WiFiUDP udp;
const unsigned int udpPort = 12345; // UDP端口

void setup() {
  Serial.begin(115200);

  // 配置静态IP地址
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  // 连接到AP
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // 开始UDP
  udp.begin(udpPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char packetBuffer[255];
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.printf("Received packet: %s\n", packetBuffer);
  }
}
