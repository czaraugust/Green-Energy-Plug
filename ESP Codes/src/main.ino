// https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define ESTADO_INICIAL 0
#define ESTADO_CAPTIVE_PORTAL 1
#define ESTADO_CONECTE_WIFI 2
#define ESTADO_CONECTE_MQTT 3
#define ESTADO_RUN 4

const int button = 16;
int temp = 0;

char ControleMaquinaEstado;

bool rele = false;
bool connectedAWS = false;
const char *softAP_ssid = "TOMADA VERDE";
const char *softAP_password = "12345678";
WiFiClient espClient;
PubSubClient client(espClient);
/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";
const char* mqtt_server = "192.168.0.168";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
boolean has_ssid;

/** Last time I tried to connect to WLAN */
long lastConnectTry = 0;

/** Current WLAN status */
int status = WL_IDLE_STATUS;

String id = "\0";

void connectWifi() {
    delay(1000);
      WiFi.mode(WIFI_STA);
      Serial.println("Connecting as wifi client...");
      WiFi.disconnect();
      WiFi.begin ( ssid, password );
      int connRes = WiFi.waitForConnectResult();
      Serial.print ( "connRes: " );
      Serial.println ( connRes );

  }



void captiveSetup(){
        delay(1000);
        Serial.println("CAPTIVE_SETUP");
        Serial.println();
        Serial.print("Configuring access point...");
        //clearEEPROM();
        /* You can remove the password parameter if you want the AP to be open. */
        WiFi.softAPConfig(apIP, apIP, netMsk);
        WiFi.softAP(softAP_ssid, softAP_password);
        delay(500); // Without delay I've seen the IP address blank
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());

        /* Setup the DNS server redirecting all the domains to the apIP */
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", apIP);

        /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
        server.on("/", handleRoot);
        server.on("/wifi", handleWifi);
        server.on("/wifisave", handleWifiSave);
        server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
        server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
        server.onNotFound ( handleNotFound );
        server.begin(); // Web server start
        Serial.println("HTTP server started");

}

void  captiveLoop(){
  Serial.println("CAPTIVE_LOOP");
/*  if (has_ssid) {
  Serial.println ( "Connect requested" );
  has_ssid = false;
  connectWifi();
  lastConnectTry = millis();
}*/
    int s = WiFi.status();
    if (s == 0 && millis() > (lastConnectTry + 60000) ) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      has_ssid = true;
    }
    if (status != s) { // WLAN status change
      Serial.print ( "Status: " );
      Serial.println ( s );
      status = s;
      if (s == WL_CONNECTED) {
        /* Just connected to WLAN */
        Serial.println ( "" );
        Serial.print ( "Connected to " );
        Serial.println ( ssid );
        Serial.print ( "IP address: " );
        Serial.println ( WiFi.localIP() );

        // Setup MDNS responder
        if (!MDNS.begin(myHostname)) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
      } else if (s == WL_NO_SSID_AVAIL) {
        WiFi.disconnect();
      }
    }

  // Do work:
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();

}

void callback(char* topic, byte* payload, unsigned int length) {
  /*Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.println(i);
    Serial.println((char)payload[i]);

  }
  Serial.println(length);*/

  // Switch on the LED if an 1 was received as first character

  //Serial.println(topic);
  if ((char)payload[0] == '1') {
        rele = true;
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else  if ((char)payload[0] == '0'){
      rele = false;


  }
  if ((char) payload[0] == 'a' && (char) payload[1] == 'c' && (char)payload[2]=='c'){
      connectedAWS = true;
      Serial.println("acc");
  }

  if (rele){
     Serial.print('@');
  }
  else if (!rele){
     Serial.print ('&');

  }
}

void reconnect() {
  // Loop until we're reconnected
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
    connectedAWS = false;
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "TOMADA VERDE IC-";
    byte mac[6];
    WiFi.macAddress(mac);
    char mess[35];
    clientId += String(mac[0], HEX);
    clientId += String(mac[1], HEX);
    clientId += String(mac[2], HEX);
    clientId += String(mac[3], HEX);
    clientId += String(mac[4], HEX);
    clientId += String(mac[5], HEX);

     id = "\0";
    id += String(mac[0], HEX);
    id += String(mac[1], HEX);
    id += String(mac[2], HEX);
    id += String(mac[3], HEX);
    id += String(mac[4], HEX);
    id += String(mac[5], HEX);



    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    String aws = "connection/";
    aws += id;
    aws += "/status";
    client.subscribe(aws.c_str());
    Serial.println(aws.c_str());
  }
}

void MQTTsetup(){
  Serial.println("MQTT_SETUP");
  connectedAWS = false;
       id = "\0";
      byte mac[6];
      WiFi.macAddress(mac);
      //char mess[35];
      id += String(mac[0], HEX);
      id += String(mac[1], HEX);
      id += String(mac[2], HEX);
      id += String(mac[3], HEX);
      id += String(mac[4], HEX);
      id += String(mac[5], HEX);
      //snprintf (mess, 35, "new_device|%x%x%x%x%x%x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
      //snprintf (mess, 35, "new_device|"+id ,"");
      String mess = "new_device|"+id;
      //batt.c_str()

      client.publish("connection/new", mess.c_str() );
      //client.publish("connection/new", "HELLO|001" );
      Serial.println(mess);

      String topic =  "plug/";
      topic += id;
      topic += "/power";
      client.subscribe(topic.c_str());
      Serial.println(topic);




      delay(1000);
}



void setup() {
  delay(1000);
  Serial.begin(115200);
 //clearEEPROM();
  captiveSetup();
  ControleMaquinaEstado = ESTADO_INICIAL;
/*  captiveSetup();
    if(WiFi.status()== WL_CONNECTED){
            MQTTsetup();
    }*/
  }

void loop() {
  delay(500);
temp = digitalRead(button);
  if (temp == HIGH) {
    clearEEPROM();
    Serial.println("CLEAR");

  }
  switch (ControleMaquinaEstado) {

    case ESTADO_INICIAL :{
    loadCredentials();
    has_ssid = (strlen(ssid) > 0);
    if (has_ssid){
      ControleMaquinaEstado = ESTADO_CONECTE_WIFI;
      Serial.println("INICIO -> CONECTE WIFI");
      connectWifi();


    }
    else{
      ControleMaquinaEstado = ESTADO_CAPTIVE_PORTAL;
      Serial.println("INICIO -> CAPTIVE");

    }
      break;

    }
    case ESTADO_CAPTIVE_PORTAL:{
      loadCredentials();
      has_ssid = (strlen(ssid) > 0);
       if (has_ssid) {
         ControleMaquinaEstado = ESTADO_CONECTE_WIFI;
         Serial.println("CAPTIVE -> CONECTE WIFI");
         connectWifi();
       }
       else{
         captiveLoop();
         ControleMaquinaEstado = ESTADO_CAPTIVE_PORTAL;
         Serial.println("CAPTIVE -> CAPTIVE");

       }
       break;
     }

       case ESTADO_CONECTE_WIFI: {
         if (WiFi.status()== WL_CONNECTED){

           ControleMaquinaEstado = ESTADO_CONECTE_MQTT;
           Serial.println("CONECTEWIFI - > VAI PARA MQTT");
         }
         else {
           ControleMaquinaEstado = ESTADO_CONECTE_WIFI;
           connectWifi();
           Serial.println("CONECTEWIFI-> CONECTE WIFI");

         }

          break;
       }
       case ESTADO_CONECTE_MQTT: {
         if (!client.connected() && WiFi.status()== WL_CONNECTED) { //CLIENTE NÃO CONECTADO MAS WIFI CONECTADO
           ControleMaquinaEstado = ESTADO_CONECTE_MQTT;

           reconnect();
           Serial.println("CONECTEMQTT-> CONECTEMQTT");
         }
         else if(client.connected() && WiFi.status()== WL_CONNECTED){ //CLIENTE E WIFI CONECTADOS
           ControleMaquinaEstado = ESTADO_RUN;
           MQTTsetup();
           Serial.println("CONECTEMQTT-> RUN");


         }



         else if(!client.connected() && WiFi.status()!= WL_CONNECTED){ //CLIENTE E WIFI DESCONECTADOS
           ControleMaquinaEstado = ESTADO_CONECTE_WIFI;
           Serial.println("CONECTEMQTT-> CONECTEWIFI");

         }
         else if(client.connected() && WiFi.status()!= WL_CONNECTED){ //CLIENTE CONECTADO MAS WIFI DESCONECTADO (SITUAÇÃO ESTRANHA!)
           ControleMaquinaEstado = ESTADO_CONECTE_WIFI;
           Serial.println("CONECTEMQTT-> CONECTEWIFI (strange)");

         }


         break;
       }
       case ESTADO_RUN :{
         if (client.connected()){
          ControleMaquinaEstado = ESTADO_RUN;
         client.loop();


         String c;
         String batt, correnteAC;
          while (Serial.available()) {
           char c = Serial.read();

           if (c== 'A'){
              correnteAC += Serial.readString();
              //Serial.print(correnteAC);
              String topicCurrent =  "plug/";
              topicCurrent += id;
              topicCurrent  += "/sensor/consumption";

              //Serial.println(topicCurrent);
              client.publish(topicCurrent.c_str(), correnteAC.c_str());

           }
           else  if ( c == 'B' ) {
              batt += Serial.readString();
              //Serial.println(batt);

              String topicBattery =  "plug/";
              topicBattery += id;
              topicBattery += "/sensor/battery";


              /*String t =batt;
              t += "|"  ;
              t += batt ;*/
              client.publish(topicBattery.c_str(), batt.c_str());
            }

              delay(100);
          }
           delay(1000);
         }
         else if (!client.connected()){
           ControleMaquinaEstado = ESTADO_CONECTE_MQTT;
           Serial.println("RUN - >CONECTEMQTT");



         }


       }
    }
  }
