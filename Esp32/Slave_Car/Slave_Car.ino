#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <HardwareSerial.h>

const char *ssid = "Base_Station";
const char *password = "12345678";

// 设置静态IP地址
IPAddress local_IP(192, 168, 4, 2); // 从机1的静态IP地址
IPAddress gateway(192, 168, 4, 1);  // AP的IP地址
IPAddress subnet(255, 255, 255, 0); // 子网掩码

WiFiUDP udp;
const unsigned int udpPort = 12345; // UDP端口
HardwareSerial MySerial(0);

//开启http服务
WebServer server(80);

void handleRoot() {
  String message = "Data received";
  if (server.hasArg("message")) {
    message = server.arg("message");
  }

  String Success_Back = "Salve_1_OK";
  server.send(200, "text/plain", Success_Back);
  MySerial.print(message);
}

void setup() {

  Serial.begin(115200);

  MySerial.begin(115200, SERIAL_8N1, 20, 21);

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
  // udp.begin(udpPort);

  // 配置HTTP请求处理
  server.on("/", HTTP_POST, handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // int packetSize = udp.parsePacket();
  // if (packetSize) {
  //   char packetBuffer[255];
  //   int len = udp.read(packetBuffer, 20);
  //   if (len > 0) {
  //     packetBuffer[len] = 0;
  //   }

  //   //Serial.printf("Received packet: %s\n", packetBuffer);
  //   MySerial.print(packetBuffer);

  // }
  server.handleClient();
}
