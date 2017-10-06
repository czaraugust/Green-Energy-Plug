// /*
//  Basic ESP8266 MQTT example
//
//  This sketch demonstrates the capabilities of the pubsub library in combination
//  with the ESP8266 board/library.
//
//  It connects to an MQTT server then:
//   - publishes "hello world" to the topic "outTopic" every two seconds
//   - subscribes to the topic "inTopic", printing out any messages
//     it receives. NB - it assumes the received payloads are strings not binary
//   - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
//     else switch it off
//
//  It will reconnect to the server if the connection is lost using a blocking
//  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
//  achieve the same result without blocking the main loop.
//
//  To install the ESP8266 board, (using Arduino 1.6.4+):
//   - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
//        http://arduino.esp8266.com/stable/package_esp8266com_index.json
//   - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
//   - Select your ESP8266 in "Tools -> Board"
//
// */
//
// #include <ESP8266WiFi.h>
// #include <PubSubClient.h>
//
// // Update these with values suitable for your network.
//
// const char* ssid = "EASY_NETWORK";
// const char* password = "easygroup";
// const char* mqtt_server = "192.168.0.168";
//
//
//
// WiFiClient espClient;
// PubSubClient client(espClient);
// long lastMsg = 0;
// char msg[50];
// int value = 0;
//
// void setup_wifi() {
//
//   delay(10);
//   // We start by connecting to a WiFi network
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//
//   WiFi.begin(ssid, password);
//
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//
//   randomSeed(micros());
//
//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }
//
// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.print("] ");
//   for (int i = 0; i < length; i++) {
//     Serial.print((char)payload[i]);
//   }
//   Serial.println();
//
//   // Switch on the LED if an 1 was received as first character
//   if ((char)payload[0] == '1') {
//     digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//     // but actually the LED is on; this is because
//     // it is acive low on the ESP-01)
//   } else {
//     digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//   }
//
// }
//
// void reconnect() {
//   // Loop until we're reconnected
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     // Create a random client ID
//     String clientId = "ESP8266Client-";
//     clientId += String(random(0xffff), HEX);
//     // Attempt to connect
//     if (client.connect(clientId.c_str())) {
//       Serial.println("connected");
//       // Once connected, publish an announcement...
//       client.publish("devicepub", "");
//       // ... and resubscribe
// 			//client.publish("connection/new", "HELLO|O2O" );
//       //client.subscribe("devicesub");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       // Wait 5 seconds before retrying
//       delay(5000);
//     }
//   }
// }
//
// void setup() {
//
//   pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
//   Serial.begin(115200);
//   setup_wifi();
//   client.setServer(mqtt_server, 1883);
//   client.setCallback(callback);
//
// 	while (!client.connected()) {
//     reconnect();
//   }
// 	byte mac[6];
// 	WiFi.macAddress(mac);
//
// 		//char message[12] = "new_device|";
// //snprintf (msg, 75, correnteAC);
//
// 	char mess[35];
// 	// {'n','e','w','_','d','e','v','i','c','e','|', mac[0] ,mac[1],mac[2],mac[3],mac[4], mac[5]};
// 	snprintf (mess, 35, "new_device|%02X%02X%02X%02X%02X%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
// 	client.publish("connection/new", mess );
//
// 		//client.publish("connection/new", "HELLO|001" );
// 	Serial.println(mess);
// }
//
// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();
// 	String c;
// 	String tensao, correnteAC;
// 	 while (Serial.available()) {
// 		char c = Serial.read();
// 		if (c== 'A'){
// 			 correnteAC += Serial.readString();
// 			 Serial.print(correnteAC);
// 			//Serial.print("CorrenteAC");
// 			 //snprintf (msg, 75, correnteAC);
// 			 correnteAC.toCharArray(msg,75);
// 			 Serial.print(msg);
// 			 //client.publish("devicepub", msg);
// 		}
// 		else if (c== 'V'){
// 			// tensao += Serial.readString();
// 			// Serial.print(tensao);
// 			 //snprintf (msg, 75, tensao);
// 				//client.publish("devicepub", msg);
// 			//Serial.print("TENSAO");
// 		 }
// 		else{
// 		//Serial.print(c);
// 		//Serial.print("\n");
// 	 }
// 	delay(100);
// 	}
// 		delay(1000);
//   // long now = millis();
//   // if (now - lastMsg > 2000) {
//   //   lastMsg = now;
//   //   ++value;
//   //   snprintf (msg, 75, "T100helloworldUFAL");
//   //   Serial.print("Publish message: ");
//   //   Serial.println(msg);
//   //   client.publish("devicepub", msg);
//   // }
// }


// void setup() {
// 	Serial.begin(115200);
// }
//
// void loop(){
// 	 	String c;
// 		String tensao, correnteAC;
// 		 while (Serial.available()) {
// 			char c = Serial.read();
//
// 			if (c== 'A'){
// 				 correnteAC += Serial.readString();
// 				 Serial.print(correnteAC);
// 				//Serial.print("CorrenteAC");
// 			}else if (c== 'V'){
// 				 tensao += Serial.readString();
// 				 Serial.print(tensao);
// 				//Serial.print("TENSAO");
// 			 }
// 		 	else{
// 		 	//Serial.print(c);
// 			//Serial.print("\n");
// 		 }
//
//
//
// 		delay(100);
// 		}
// 	 		delay(1000);
// }
