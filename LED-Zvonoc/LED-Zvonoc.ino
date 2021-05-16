#include <ESP8266WiFi.h>//обычная библтотека для есп
#include <ESP8266WebServer.h> //библиотека для вебсервера 
#include <ESP8266SSDP.h>//библиботека для опознования устройства
#include <DNSServer.h>//библиотека для DNS-server
#include <FS.h> //библиотека для работы с файловой системы
#include <ArduinoJson.h>//библиотека для работы с json файлами
#include <time.h>
#include <ArduinoOTA.h> // Библиотека для OTA-прошивки
#include <ESP8266mDNS.h> //тоже для OTA
#include <WiFiUdp.h>//UDP-библиотека
#include <PxMatrix.h>
#include <Ticker.h>
#include <ESP8266mDNS.h>
File fsUploadFile;//подключение файловой системы
DNSServer dnsServer; //объявление тела dnsServer
WiFiUDP Udp;//объявление тела Udp
Ticker display_ticker;
IPAddress apIP(192,168,4,1);//ip точки доступа
IPAddress ip_take(192, 168, 4, 255);//ip при получении данных
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2
#define matrix_width 64
#define matrix_height 32
uint8_t display_draw_time=50; //30-70 is usually fine
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);
uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myWHITE,myYELLOW,myCYAN,myMAGENTA,myBLACK};
const byte DNS_PORT = 53; //порт для DNS-server
const byte HTTP_PORT = 80; //порт для HTTP-server
ESP8266WebServer HTTP(HTTP_PORT);
String _ssid="RT-GPON-E25F";
String _password="pvkp25BT";/*
String _ssid="WIFI-Zvonoc";
String _password="";*/
String _ssidAP="LED-Zvonoc";//логин для точки досьупа  
String _passwordAP="";//пароль для точки доступа
String SSDP_Name="LED-Zvonoc"; //имя устройство
String _domen="led-zvonoc";
String ip="Не знаю";
String HTTP_ssid="123"; //логин для Web-server
String HTTP_password="123";//пароль для Web-server
String jsonConfig="{}";
String text="Привет";
String byfer;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet
int color=0;
byte val=0;
byte timezone=3;
bool cycle50mils = false;/*
bool cycle3s = false;
bool cycle5s = false;
bool cycle10s = false;
bool cycle30s = false;
bool cycle1m = false;
bool cycle3m = false;
bool cycle24h = false;*/
unsigned long timer50mils;/*
 unsigned long timer3s;
  unsigned long timer5s;
  unsigned long timer10s;
  unsigned long timer30s;
  unsigned long timer1m;
  unsigned long timer3m;
  unsigned long timer24h;*/
  int xpos=matrix_width;
void setup() {
   Serial.begin(115200); Serial.println("");
   Serial.println("Start 1-FS");
   FS_init();//запуск файловой системы
   Serial.println("Start 2-Fileconfig");
   loadConfig();//считывает данные из файла
   Serial.println("Start 3-WIFI");
   WIFI_init();//запускаем подключение к вайфаю
   Serial.println("Start 4-Time");
   Time_init();//получаем время
   Serial.println("Start 5-SSDP");
   SSDP_init();//Настройка и запуск SSDP интерфейс
   Serial.println("Start 6-Webserver");
   HTTP_init();//запуск и настройка HTTP интерфейс
   Serial.println("Start 7-Clock");
   timersInit();
   Serial.println("Start 8-WIFI Update");
   Udp.begin(8888);//udp порт
   ArduinoOTA.setPort(8266);//порт подключения
   ArduinoOTA.setHostname("LED-Zvonoc"); // Задаем имя сетевого порта
   ArduinoOTA.setPassword((const char *)"1975"); // Задаем пароль доступа для удаленной прошивки
   ArduinoOTA.begin(); // Инициализируем OTA
   Serial.println("Start 9-Led");
   display.begin(16);
  display.clearDisplay();
  display.setPanelsWidth(1);
  display_update_enable(true);//обновление экрана
  display.setTextWrap(false);  // перенос того что не поместилось
    display.setTextSize(2);//развмер шрифта
    display.setRotation(0);  //поворот экрана
    //display.setTextColor(display.color565(colorR,colorG,colorB));
    if (WiFi.status()== WL_CONNECTED){  MDNS.begin(_domen);
        MDNS.addService("http", "tcp", HTTP_PORT);}
}
void loop() {
     timersWorks();
     HTTP.handleClient();
      if (WiFi.status()!= WL_CONNECTED) dnsServer.processNextRequest();
      else { 
           MDNS.update();
           ArduinoOTA.handle(); // Всегда готовы к прошивке
      }
     if (Serial.available() > 0){
        val = Serial.read();
        if (val=='1'){ 
           if(WiFi.status() == WL_CONNECTED)Serial.println(ip);
           else Serial.println("192.168.4.1");
        }
        else if (val=='2'){ 
          Serial.print(_ssid);Serial.print(" "); Serial.print(_password);Serial.print(" "); Serial.print(SSDP_Name);Serial.print(" "); Serial.print(_ssidAP);Serial.print(" "); Serial.print(_passwordAP);
        }
     }
  if (Udp.parsePacket()>0) {
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("ok");
    Udp.endPacket();
    byfer=packetBuffer;
    Serial.println(byfer);
    if(byfer=="!9!"){text="";}
    else if(byfer=="!0!") {color=0;saveConfig();}
    else if(byfer=="!1!") {color=1;saveConfig();}
    else if(byfer=="!2!") {color=2;saveConfig();}
    else if(byfer=="!3!") {color=3;saveConfig();}
    else if(byfer=="!4!") {color=4;saveConfig();}
    else if(byfer=="!5!") {color=5;saveConfig();}
    else if(byfer=="!6!") {color=6;saveConfig();}
    else if(byfer=="!7!") {color=7;saveConfig();}
    else {text+=packetBuffer;saveConfig();}
  } 
    if (cycle50mils) scroll_text(8,text,myCOLORS[color]);
    eraseCyclos();
}
