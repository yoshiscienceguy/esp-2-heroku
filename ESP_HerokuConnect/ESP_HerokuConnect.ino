/*
  WiFiEsp test: ClientTest

  Test client functions.
*/

#include "WiFiEsp.h"

// Emulate Serial_3 on pins 7/6 if not present
//#ifndef HAVE_HWSerial_3
#include "SoftwareSerial.h"
SoftwareSerial Serial_3(10, 11); // TX, RX
//#endif


char ssid[] = "network";     // your network SSID (name)
char pwd[] = "pass";  // your network password

char server[] = "https://esp-2-heroku.herokuapp.com";
// Initialize the Wifi client library
WiFiEspClient client;

float temp = 25;
int ledStatus = 0;
unsigned long timeUntilNextUpdate = 20000; // 20 seconds to send temp info
unsigned long timeCheckStatus = 2000; // 2 seconds pull info
unsigned long currentTime = 0;

void setup()
{
  pinMode(13, OUTPUT); //built in LED
  pinMode(2, INPUT_PULLUP); // built in switch

  Serial.begin(115200); //baudrate for serial monitor

  Serial_3.begin(115200); //defaultbaudrate for ESP
  Serial_3.println("AT+UART_DEF=9600,8,1,0,0\r\n"); //AT command to change Baudrate
  Serial_3.flush();
  delay(2);
  Serial_3.end(); //close comm port

  Serial_3.begin(9600); //start comm port at 9600
  WiFi.init(&Serial_3); //initialize ESP with Serial3

  WiFi.begin(ssid, pwd); //connect to supplied wifi name and password
  IPAddress ip = WiFi.localIP(); //get IP address
  Serial.print("IP Address: "); //print IP address
  Serial.println(ip);

  assertEquals("Connect to server", client.connectSSL(server, 443), 1); //connect to server
  assertEquals("Connected", client.connected(), true); // check if connnection is valid
  //--------------------------------------------------------------
  // HTTP request without 'Connection: close' command
}

void updateLight() { //function to update LED if 1 == On or 0 == Off
  if (ledStatus == 1) {
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, LOW);
  }
}

void updateStatus() { //Pull information from web
  client.println("GET /latest.html HTTP/1.1");
  client.println("Host: esp-2-heroku.herokuapp.com");
  client.println("Connection: keep-alive");
  client.println();
  delay(10);

  bool found = false;
  while (client.available()) {
    //char c = client.read();
    //Serial.write(c);
    if (found == false) {
      client.find((char*) "\r\n\r\n"); // text before the body of a request so we can ignore the header
      found = true;
    }
    else {
      String info = client.readString(); //text ex. {'info':'1;255','status':200}
      int startIndex = info.indexOf(":") + 2; 
      int endIndex = info.indexOf(",") - 1;
		
	  //Substring ex. 1;255
      info = info.substring(startIndex, endIndex);
      String Stat = info.substring(0, info.indexOf(";"));
      String Temp = info.substring(info.indexOf(";") + 1);

      ledStatus = Stat.toInt();
      temp = Stat.toFloat();

    }


  }
}

void ConnectServer(){ // connect to server
  client.connectSSL(server, 443);
}

void toggleStatus() { //toggle status from 1 to 0 or viceversa
  client.println("POST /toggle.html HTTP/1.1");
  client.println("Host: esp-2-heroku.herokuapp.com");
  client.println("Connection: keep-alive");
  client.println();
  delay(10);
  while (client.available()) {
    //char c = client.read();
    //Serial.write(c);
    Serial.println(client.readString());
  }
}

void postTemp() { //send current temp from A0 to server.
  String togo = String(analogRead(A0)); //object connected to A0
  client.println("POST /temp.html HTTP/1.1");
  client.println("Host: esp-2-heroku.herokuapp.com");
  client.println("Accept: */*");
  client.print("Content-Length: " );
  client.println(togo.length());
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println();
  client.println(togo);
  delay(10);
  while (client.available()) {
    //char c = client.read();
    //Serial.write(c);
    Serial.println(client.readString());


  }
}

void disconnectServer(){
  Serial.println(); // If server is disconnected, we disconnect and reconnect server
  Serial.println("Disconnecting from server...");

  client.stop();

  ConnectServer();
}
void loop()
{ 
  if(digitalRead(2) == HIGH){ // check if switch is pulled
    while(digitalRead(2) == HIGH){} //wait for debounce effect
    toggleStatus();
    updateStatus();
    updateLight();  
  }

  if((millis() - currentTime) > timeUntilNextUpdate){ // Post temp after timeUntilNextUpdate milliseconds value
    currentTime = millis();
    postTemp();  
    
  }
  if((millis() - currentTime) > timeCheckStatus){// Post temp after timeCheckStatus milliseconds value
    currentTime = millis();  
    updateStatus();
    updateLight();
  }

  if(client.connected() == false){ // If client disconnects, reconnect
    disconnectServer();
  }
}


////////////////////////////////////////////////////////////////////////////////////
//Testing
void assertEquals(const char* test, int actual, int expected)
{
  if (actual == expected)
    pass(test);
  else
    fail(test, actual, expected);
}

void assertEquals(const char* test, char* actual, char* expected)
{
  if (strcmp(actual, expected) == 0)
    pass(test);
  else
    fail(test, actual, expected);
}


void pass(const char* test)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.println(" > PASSED");
  Serial.println();
}

void fail(const char* test, char* actual, char* expected)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.print(" > FAILED");
  Serial.print(" (actual=\"");
  Serial.print(actual);
  Serial.print("\", expected=\"");
  Serial.print(expected);
  Serial.println("\")");
  Serial.println();
  delay(10000);
}

void fail(const char* test, int actual, int expected)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.print(" > FAILED");
  Serial.print(" (actual=");
  Serial.print(actual);
  Serial.print(", expected=");
  Serial.print(expected);
  Serial.println(")");
  Serial.println();
  delay(10000);
}
