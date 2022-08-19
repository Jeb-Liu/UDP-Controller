#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "MPU9250.h" //IMU data & fuction

#ifndef STASSID
#define STASSID "YJJKDS"
#define STAPSK  "18689479617"
#define PC_IP "192.168.1.100" //MY PC IP
#define PC_PORT 23333 //communicate with PORT 23333
#endif

unsigned int localPort = 23333;
WiFiUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged\r\n";// a string to send back

void setup() {
  //Serial
  Serial.begin(115200);
  //Wire
  Wire.begin();
  Wire.setClock(400000);
  
  //UDP ON
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);

  //IMU 初期化
  setupMPU( MPU_9250_ADDRESS );

   //Fin
  delay(100);
  Serial.println("setup fin!");
}

int oldMillis,newMillis,dt;
double ddt=0;

void loop() {
  //Serial.println("in loop");
  oldMillis = millis();        
  ddt = double(dt)/1000;
  
  recordAccelRegister( MPU_9250_ADDRESS );    //加速度センサデータ読み取り
  recordGryoRegister( MPU_9250_ADDRESS );     //角速度センサデータ読み取り
  recordMagRegister( AK8975_ADDRESS );        //地磁気センサデータ読み取り
  IMUoffset();
  processAccelData();       //加速度計算
  processGryoData();        //角速度計算
  processAccAngle();
  processGryoAngle(ddt);
  processEulerAngle(ddt);
  
  printEuler();
  print9axis();          //Serialへ計測データをプリント
  udpPrint();
  udpRead();
  
  newMillis = millis();
  dt = newMillis-oldMillis;
  Serial.println(dt);
}


//==== Fuction ===========================================================

void udpRead()
{
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                  packetSize,
                  Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                  Udp.destinationIP().toString().c_str(), Udp.localPort(),
                  ESP.getFreeHeap());

    // read packet
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply
    Udp.beginPacket(PC_IP, PC_PORT);
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}
void IMUoffset()
{
  G_FORCE_X = G_FORCE_X - 0.1460297;
  G_FORCE_Y = G_FORCE_Y - 0.1398246;
  G_FORCE_Z = G_FORCE_Z + 0.5811982;
  ROT_X = ROT_X - 1.1944745;
  ROT_Y = ROT_Y + 2.1362600;
  ROT_Z = ROT_Z;//0.0746470
}
void udpPrint()
{
  Udp.beginPacket(PC_IP, PC_PORT);
  Udp.print(newMillis - oldMillis,6);Udp.print(",");
  Udp.print(G_FORCE_X,6);Udp.print(",");
  Udp.print(G_FORCE_Y,6);Udp.print(",");
  Udp.print(G_FORCE_Z,6);Udp.print(",");
  Udp.print(ROT_X,6);Udp.print(",");
  Udp.print(ROT_Y,6);Udp.print(",");
  Udp.print(ROT_Z,6);Udp.print(",");
  Udp.print(MAG_X,6);Udp.print(",");
  Udp.print(MAG_Y,6);Udp.print(",");
  Udp.print(MAG_Z,6);Udp.print(",");
  Udp.print(roll,6);Udp.print(",");
  Udp.print(pitch,6);Udp.print(",");
  Udp.print(yaw,6);
  Udp.endPacket();
}
void print9axis()
{
  Serial.print(G_FORCE_X);Serial.print("\t");
  Serial.print(G_FORCE_Y);Serial.print("\t");
  Serial.print(G_FORCE_Z);Serial.print("\t");
  Serial.print(ROT_X);Serial.print("\t");
  Serial.print(ROT_Y);Serial.print("\t");
  Serial.print(ROT_Z);Serial.print("\t");
  Serial.print(MAG_X);Serial.print("\t");
  Serial.print(MAG_Y);Serial.print("\t");
  Serial.print(MAG_Z);Serial.print("\t");
}
void printEuler()
{
  Serial.print(roll);Serial.print("\t");
  Serial.print(pitch);Serial.print("\t");
  Serial.print(yaw);Serial.print("\t");

}
