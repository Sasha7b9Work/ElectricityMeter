#include <SoftwareSerial.h>
SoftwareSerial SIM800(7, 6);        // для новых плат начиная с версии RX,TX
#include <DallasTemperature.h>      // подключаем библиотеку чтения датчиков температуры
OneWire oneWire(4);                 // и настраиваем  пин 4 как шину подключения датчиков DS18B20
DallasTemperature sensors(&oneWire);
#define BAT_Pin      A0             // на батарею, через делитель напряжения 39кОм / 11 кОм
/*  ----------------------------------------- ИНДИВИДУАЛЬНЫЕ НАСТРОЙКИ !!!---------------------------------------------------------   */
String APN = "internet.mts.by";    // тчка доступа выхода в интернет вашего сотового оператора
String USER = "mts";               // имя выхода в интернет вашего сотового оператора
String PASS = "mts";               // пароль доступа выхода в интернет вашего сотового оператора
bool  broker = false;               // статус подклюлючения к брокеру
/*  ----------------------------------------- НАСТРОЙКИ MQTT брокера---------------------------------------------------------   */
const char MQTT_user[10] = "drive2ru";      // api.cloudmqtt.com > Details > User  
const char MQTT_pass[15] = "martinhol221";  // api.cloudmqtt.com > Details > Password
const char MQTT_type[15] = "MQIsdp";        // тип протокола
const char MQTT_CID[15] = "CITROEN";        // уникальное имя устройства в сети MQTT
String MQTT_SERVER = "m23.cloudmqtt.com";   // api.cloudmqtt.com > Details > Server  сервер MQTT брокера
String PORT = "10077";                      // api.cloudmqtt.com > Details > Port    порт MQTT брокера
/*  ----------------------------------------- ДАЛЕЕ НЕ ТРОГАЕМ --------------------------------------------------------------------   */
String at = "";                    // строка хранения ответов модема в ASCII
float TempDS[11];                  // массив хранения температуры c рахных датчиков 
float Vbat;                        // переменная хранящая напряжение бортовой сети
float m = 69.91;                   // делитель для перевода АЦП в вольты для резистров 39/11kOm
unsigned long Time1 = 0; 
int k = 0;
int interval = 2;                  // интервал отправки данных на народмон сразу после загрузки 
int inDS;
int Timer = 29;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);              //скорость порта
  SIM800.begin(9600);              //скорость связи с модемом
  Serial.println("MQTT |04/01/2018"); 
  delay (1000);
  SIM800.println("AT+CFUN=1,1");   // перезагрузка модема
             }


void loop() {
/*  --------------------------------------------------- АНАЛИЗИРУЕМ БУФЕР ВИРТУАЛЬНОГО ПОРТА МОДЕМА---------------------------------- */ 
  if (SIM800.available()) {                                               // если что-то пришло от SIM800 в SoftwareSerial Ардуино
    while (SIM800.available()) k = SIM800.read(),at += char(k),delay(1);  // набиваем в переменную at
   
  Serial.println(at);               // Возвращаем ответ можема в монитор порта в ACSII
   
   /*  -------------------------------------- проверяем соеденеиние с ИНТЕРНЕТ, конектимся к серверу------------------------------------------------------- */
          if (at.indexOf("AT+SAPBR=3,1, \"Contype\",\"GPRS\"\r\r\nOK") > -1 ) {SIM800.println("AT+SAPBR=3,1, \"APN\",\""+APN+"\""), delay (500); 
   } else if (at.indexOf("AT+SAPBR=3,1, \"APN\",\""+APN+"\"\r\r\nOK") > -1 )  {SIM800.println("AT+SAPBR=1,1"), delay (1000); // устанавливаем соеденение   
   } else if (at.indexOf("AT+SAPBR=1,1\r\r\nOK") > -1 )                       {SIM800.println("AT+SAPBR=2,1"), delay (1000); // проверяем статус соединения  
   } else if (at.indexOf("+SAPBR: 1,1") > -1 )    {delay (200),  SIM800.println("AT+CIPSTART=\"TCP\",\""+MQTT_SERVER+"\",\""+PORT+"\""), delay (1000);
   } else if (at.indexOf("+CME ERROR:") > -1 )    {broker = false, delay (50), SIM800.println("AT+CFUN=1,1"), delay (1000), interval = 6 ;

  
   } else if (at.indexOf("C5/comandREL",4) > -1 )        {int stat = at.substring(at.indexOf("")+16, at.indexOf("")+17).toInt();
                                                         int  rel = at.substring(at.indexOf("")+17, at.indexOf("")+19).toInt();
                                                         digitalWrite(rel, stat);
   Serial.print ("Установка пина"), Serial.print (rel), Serial.print (" в состояние "), Serial.println (stat); 

   
   } else if (at.indexOf("C5/comandRefresh",4) > -1 )    {// Serial.println ("Команда обнвления");
                                                          SIM800.println("AT+CIPSEND"), delay (200);  
                                                          MQTT_FloatPub ("C5/ds0", TempDS[0],2);
                                                          MQTT_FloatPub ("C5/ds1", TempDS[1],2);
                                                          MQTT_FloatPub ("C5/vbat", Vbat,2);
                                                          MQTT_FloatPub ("C5/uptime", millis()/1000,0); 
                                                          SIM800.write(0x1A); 
                                                          interval = 6; // швырнуть данные на сервер и ждать 60 сек
            
   } else if (at.indexOf("CONNECT OK\r\n") > -1 ) MQTT_CONNECT (); // после соединения с сервером отправляем пакет авторизации, публикации и пдписки у брокера
   
     at = "";   }                                               // очищаем переменные
   
if (millis()> Time1 + 10000) Time1 = millis(), detection();               // выполняем функцию detection () каждые 10 сек 

             }

void detection(){                                                        // условия проверяемые каждые 10 сек  
    Vbat = analogRead(BAT_Pin);                                          // замеряем напряжение на батарее
    Vbat = Vbat / m ;                                                    // переводим попугаи в вольты
    Serial.print("Vbat="),Serial.print(Vbat);  
    Serial.print("| int: "), Serial.print(interval);
    inDS = 0;
    sensors.requestTemperatures();          // читаем температуру с трех датчиков
    while (inDS < 10){
          TempDS[inDS] = sensors.getTempCByIndex(inDS);            // читаем температуру
      if (TempDS[inDS] == -127.00){ 
                                  TempDS[inDS]= 80;
                                  break; }                               // пока не доберемся до неподключенного датчика
              inDS++;} 
    for (int i=0; i < inDS; i++) Serial.print(" | Temp"), Serial.print(i), Serial.print("= "), Serial.print(TempDS[i]); 
    Serial.println (" ");

    if (Timer >0) Timer--;
    interval--;
    if (interval <1) { interval = 6; 
        if (broker == true) { SIM800.println("AT+CIPSEND"), delay (200);  
                              MQTT_FloatPub ("C5/ds0", TempDS[0],2);
                              MQTT_FloatPub ("C5/ds1", TempDS[1],2);
                              MQTT_FloatPub ("C5/vbat", Vbat,2);
                              MQTT_FloatPub ("C5/uptime", millis()/1000,0); 
                              SIM800.write(0x1A); 
                              
    } else  SIM800.println ("AT+SAPBR=3,1, \"Contype\",\"GPRS\""), delay (200);    // подключаемся к GPRS 
                     }
}             



void  MQTT_FloatPub (const char topic[15], float val, int x) { // топик, переменная в float, количество знаков после запятой
           char st[10];
           dtostrf(val,0, x, st), MQTT_PUB (topic, st);      }


void MQTT_CONNECT () {
  SIM800.println("AT+CIPSEND"), delay (100);
     
  SIM800.write(0x10);                                // заголовок пакета на установку соединения
  SIM800.write(strlen(MQTT_type)+strlen(MQTT_CID)+strlen(MQTT_user)+strlen(MQTT_pass)+12);
  SIM800.write((byte)0);                             // 0x00
  SIM800.write(strlen(MQTT_type));                   // длинна типа протокола
  SIM800.write(MQTT_type);                           // тип протокола
  SIM800.write(0x03);                                // LVL
  SIM800.write(0xC2);                                // FL
  SIM800.write((byte)0);                             // 0x00
  SIM800.write(0x3C);                                // KA
  SIM800.write((byte)0);                             // 0x00
  SIM800.write(strlen(MQTT_CID));                    // длинна идентификатора устройства
  SIM800.write(MQTT_CID);                            // MQTT  идентификатор устройства
  SIM800.write((byte)0);                             // 0x00  
  SIM800.write(strlen(MQTT_user));                   // длинна логина 
  SIM800.write(MQTT_user);                           // логин
  SIM800.write((byte)0);                             // 0x00
  SIM800.write(strlen(MQTT_pass));                   // длинна пароля
  SIM800.write(MQTT_pass);                           // пароль

  MQTT_PUB ("C5/status", "Подключено");              // пакет публикации
  
  MQTT_SUB ("C5/comand");                            // пакет подписки
  MQTT_SUB ("C5/timer");                             // пакет подписки
  
  SIM800.write(0x1A),  broker = true;          // символ завершения и маркер
  }

void  MQTT_PUB (const char MQTT_topic[15], const char MQTT_messege[15]) {
  
  SIM800.write(0x30);                                // заголовок пакета на публикацию
  SIM800.write(strlen(MQTT_topic)+strlen(MQTT_messege)+2);
  SIM800.write((byte)0);                             // 0x00
  SIM800.write(strlen(MQTT_topic));                  // длинна топика
  SIM800.write(MQTT_topic);                          // топик
  SIM800.write(MQTT_messege);                        // сообщение
                     }


void  MQTT_SUB (const char MQTT_topic[15]) {
  
  SIM800.write(0x82);                               // заголовок пакета подписки на топик
  SIM800.write(strlen(MQTT_topic)+5);               // сумма пакета 
  SIM800.write((byte)0);                            // 0x00
  SIM800.write(0x01);                               // PKTID
  SIM800.write((byte)0);                            // 0x00
  SIM800.write(strlen(MQTT_topic));                 // длинна топика
  SIM800.write(MQTT_topic);                         // топик
  SIM800.write((byte)0);                            // 0x00
                     }
