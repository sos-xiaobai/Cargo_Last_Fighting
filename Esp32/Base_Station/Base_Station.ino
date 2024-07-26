#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
#include <string.h>

//wifi名字
const char *ssid = "Base_Station";
//wifi密码
const char *password = "12345678";
// 定义UART管理对象
HardwareSerial* UART_Manage_Object = new HardwareSerial(0);

// 从机的静态IP地址
const char *slave1IP = "http://192.168.4.2/";
const char *slave2IP = "http://192.168.4.3/";
const char *slave3IP = "http://192.168.4.4/";

//udp对象
WiFiUDP udp;
// 广播地址
const IPAddress broadcastIP(192, 168, 4, 255); 
// UDP端口
const unsigned int udpPort = 12345;            

/********************UDP函数****************************/

//udp发送添加/删除货物
void Udp_Send_Message(const char* Send_Buffer)
{
  String message = Send_Buffer;
  udp.beginPacket(broadcastIP, udpPort);
  udp.print(message);
  udp.endPacket();

  Serial.println("Broadcast message: " + message);  
}

void Http_sendData(const char *url, const String &data) {
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String Pack_Data = "message="+data;

  int httpCode = http.POST(Pack_Data);

  if (httpCode > 0) 
  {
    Serial.printf("HTTP POST to %s... code: %d\n", url, httpCode);
    if (httpCode == HTTP_CODE_OK) 
    {
      String payload = http.getString();
      Serial.println("收到："+payload);
    }
  } 
  else 
  {
    Serial.printf("HTTP POST to %s... failed, error: %s\n", url, http.errorToString(httpCode).c_str());
  }

  http.end();
}

/********************货物链表****************************/

/*
* 货物类型 大/小 Ascii码值
*/
enum Enum_Cargo_Type :uint8_t
{
    Cargo_Type_Samll = 48,
    Cargo_Type_Big,
};


/*
* 货物位置 层数 Ascii码值
*/
enum Enum_Cargo_Floor :uint8_t
{
    Cargo_First_Floor = 49,
    Cargo_Second_Floor,
};


// 定义货物的结构体
struct Struct_Cargo {
    enum Enum_Cargo_Type Cargo_Type;
    enum Enum_Cargo_Floor Cargo_Floor;
    uint8_t Position_X;
    uint8_t Position_Y;
    uint8_t Phone_Number[11];
    uint8_t Code[4];
    Struct_Cargo* Next_Cargo;
};

// 定义货物列表类
class Class_Cargo_List {
public:
    Struct_Cargo* First_Cargo;

    Class_Cargo_List() : First_Cargo(nullptr) {}

    void Add_Cargo(enum Enum_Cargo_Type Cargo_Type, enum Enum_Cargo_Floor Cargo_Floor, uint8_t Position_X, uint8_t Position_Y, uint8_t* Phone_Number, uint8_t* Code);
    void Delete_Cargo(uint8_t Code[4]);
    Struct_Cargo* Exist_Cargo(uint8_t Code[4]);
      void Print_All_Cargo();
};

/********************货物变量****************************/
// 定义货物变量
Struct_Cargo Now_Cargo;
// 定义货物列表对象
Class_Cargo_List Cargo_List;


void handle_add_cargo(struct Struct_Cargo cargo) {
    Cargo_List.Add_Cargo(cargo.Cargo_Type, cargo.Cargo_Floor, cargo.Position_X, cargo.Position_Y, cargo.Phone_Number, cargo.Code);
}

void handle_delete_cargo(struct Struct_Cargo cargo) {
    Cargo_List.Delete_Cargo(cargo.Code);
}

Struct_Cargo* Class_Cargo_List::Exist_Cargo(uint8_t Code[4])
{
    Struct_Cargo* p = First_Cargo;
    while (p != NULL)
    {
        if (p->Code[0] == Code[0] && p->Code[1] == Code[1] && p->Code[2] == Code[2] && p->Code[3] == Code[3])
        {
            return p;
        }
        p = p->Next_Cargo;
    }
    return NULL;
}

void Class_Cargo_List::Add_Cargo(enum Enum_Cargo_Type Cargo_Type, enum Enum_Cargo_Floor Cargo_Floor, uint8_t Position_X, uint8_t Position_Y, uint8_t* Phone_Number, uint8_t* Code) 
{
     //如果已经存在该货物 则不添加
    if(Exist_Cargo(Code) != NULL)
    {
        return;
    }   
    // 头插法插入新节点
    Struct_Cargo* New_Cargo = new Struct_Cargo;
    New_Cargo->Cargo_Type = Cargo_Type;
    New_Cargo->Cargo_Floor = Cargo_Floor;
    New_Cargo->Position_X = Position_X;
    New_Cargo->Position_Y = Position_Y;
    memcpy(New_Cargo->Phone_Number, Phone_Number, 11);
    memcpy(New_Cargo->Code, Code, 4 * sizeof(uint8_t));

    New_Cargo->Next_Cargo = First_Cargo;
    First_Cargo = New_Cargo;

    // 添加调试信息
    // Serial.println("Cargo added:");
    // Serial.print("Type: ");
    // Serial.println(New_Cargo->Cargo_Type);
    // Serial.print("Floor: ");
    // Serial.println(New_Cargo->Cargo_Floor);    
    // Serial.print("Position X: ");
    // Serial.println(New_Cargo->Position_X);
    // Serial.print("Position Y: ");
    // Serial.println(New_Cargo->Position_Y);
    // Serial.print("Phone Number: ");
    // for (int i = 0; i < 11; i++) {
    //     Serial.print(New_Cargo->Phone_Number[i]);
    // }
    // Serial.println();
    // Serial.print("Code: ");
    // for (int i = 0; i < 4; i++) {
    //     Serial.print(New_Cargo->Code[i]);
    // }
    // Serial.println();
}

void Class_Cargo_List::Delete_Cargo(uint8_t Code[4]) {
    Struct_Cargo* p = First_Cargo;
    Struct_Cargo* q = First_Cargo;
    // 如果删除的就是第一个节点
    if (First_Cargo->Code[0] == Code[0] && First_Cargo->Code[1] == Code[1] && First_Cargo->Code[2] == Code[2] && First_Cargo->Code[3] == Code[3]) {
        First_Cargo = First_Cargo->Next_Cargo;
        delete p;
        return;
    }
    // 如果删除的不是第一个节点
    while (p != NULL) {
        if (p->Code[0] == Code[0] && p->Code[1] == Code[1] && p->Code[2] == Code[2] && p->Code[3] == Code[3]) {
            q->Next_Cargo = p->Next_Cargo;
            delete p;
            break;
        }
        q = p;
        p = p->Next_Cargo;
    }
}

void Class_Cargo_List::Print_All_Cargo() {
    Struct_Cargo* current = First_Cargo;
    while (current != NULL) {
        Serial.println("Cargo details:");
        Serial.print("Type: ");
        Serial.println(current->Cargo_Type);
        Serial.print("Floor: ");
        Serial.println(current->Cargo_Floor); 
        Serial.print("Position X: ");
        Serial.println(current->Position_X);
        Serial.print("Position Y: ");
        Serial.println(current->Position_Y);
        Serial.print("Phone Number: ");
        for (int i = 0; i < 11; i++) {
            Serial.print(current->Phone_Number[i]);
        }
        Serial.println();
        Serial.print("Code: ");
        for (int i = 0; i < 4; i++) {
            Serial.print(current->Code[i]);
        }
        Serial.println();
        current = current->Next_Cargo;
    }
}




void wired_equipment_detect(void)
{
      uint8_t data_len=UART_Manage_Object->available();
      if (data_len) 
      {
        //UART_Manage_Object->println("nihao");
        // 读取数据
        UART_Manage_Object->print(data_len);
        uint8_t received_data[22];
        int bytes_read = UART_Manage_Object->readBytes(received_data, 22);
        // for(int i=0;i<20;i++)
        // {
        //   Serial.println(received_data[i]);
        // }
        // 检查帧头和帧尾
         if (received_data[0] == 0x01)
          Cargo_List.Print_All_Cargo();
        if (received_data[0] == 0xA5 && received_data[21] == 0x11) 
        {
            // 解析数据
            uint8_t command = received_data[1];
            memcpy(&Now_Cargo, &received_data[2], 19);

            // 根据命令类型处理数据
            if (command == 0x01) 
            {
              // 添加货物
              handle_add_cargo(Now_Cargo);
              String Http_Data = String((const char*)received_data);
              if(Now_Cargo.Cargo_Type == Cargo_Type_Samll)
              {
                Http_sendData(slave1IP,Http_Data);
              }
              else if(Now_Cargo.Cargo_Type == Cargo_Type_Big)
              {
                Http_sendData(slave2IP,Http_Data);
              }
              // Udp_Send_Message((char*)received_data);
            } 
            else if (command == 0x02) 
            {
              // 删除货物
              handle_delete_cargo(Now_Cargo);
              String Http_Data = String((const char*)received_data);
              if(Now_Cargo.Cargo_Type == Cargo_Type_Samll)
              {
                Http_sendData(slave1IP,Http_Data);
              }
              else if(Now_Cargo.Cargo_Type == Cargo_Type_Big)
              {
                Http_sendData(slave2IP,Http_Data);
              }
              // Udp_Send_Message((char*)received_data);
            }
          Cargo_List.Print_All_Cargo();
        }
    }
}

void wireless_equipment_detect(void)
{
  // WiFiClient client = server.available();
  // if (client) 
  // {
  //   Serial.println("New client connected");
  //   while (client.connected()) {
  //     if (client.available()) {
  //       String request = client.readStringUntil('\r');
  //       Serial.print("Message from client: ");
  //       Serial.println(request);
  //       client.print("Message received: ");
  //       client.print(request);
  //     }
  //   }
  //   client.stop();
  //   Serial.println("Client disconnected");
  // }
}


void setup() {

  // 初始化串口0，波特率设置为115200
  UART_Manage_Object->begin(115200, SERIAL_8N1, 20, 21);
  
  // 初始化串口，波特率设置为115200
  Serial.begin(115200); 

  // 配置AP模式
  WiFi.softAP(ssid, password);

  // 打印AP IP地址
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // 开始UDP
  // udp.begin(udpPort);
}

void loop() {
  //串口buffer获取货物信息
  wired_equipment_detect();
  //Serial.println("Working...");

  // delay(1000); // 每隔10秒发送一次广播
}
