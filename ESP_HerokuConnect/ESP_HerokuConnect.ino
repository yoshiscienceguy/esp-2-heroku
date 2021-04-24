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


char ssid[] = "Dank_Memes";     // your network SSID (name)
char pwd[] = "fernando123";  // your network password

char server[] = "https://esp-2-heroku.herokuapp.com";
// Initialize the Wifi client library
WiFiEspClient client;

float temp = 25;
int ledStatus = 0;
unsigned long timeUntilNextUpdate = 20000;
unsigned long timeCheckStatus = 2000;
unsigned long currentTime = 0;

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(12, INPUT_PULLUP);

  Serial.begin(115200);

  Serial_3.begin(115200);
  Serial_3.println("AT+UART_DEF=9600,8,1,0,0\r\n");
  Serial_3.flush();
  delay(2);
  Serial_3.end();

  Serial_3.begin(9600);
  WiFi.init(&Serial_3);

  WiFi.begin(ssid, pwd);
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  assertEquals("Connect to server", client.connectSSL(server, 443), 1);
  assertEquals("Connected", client.connected(), true);
  //--------------------------------------------------------------
  // HTTP request without 'Connection: close' command
}

void updateLight() {
  if (ledStatus == 1) {
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, LOW);
  }
}

void updateStatus() {
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
      client.find((char*) "\r\n\r\n");
      found = true;
    }
    else {
      String info = client.readString();
      int startIndex = info.indexOf(":") + 2;
      int endIndex = info.indexOf(",") - 1;

      info = info.substring(startIndex, endIndex);
      String Stat = info.substring(0, info.indexOf(";"));
      String Temp = info.substring(info.indexOf(";") + 1);

      ledStatus = Stat.toInt();
      temp = Stat.toFloat();

    }


  }
}

void ConnectServer(){
  client.connectSSL(server, 443);
}

void toggleStatus() {
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

void postTemp() {
  String togo = String(analogRead(A0));
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
  Serial.println();
  Serial.println("Disconnecting from server...");

  client.stop();

  ConnectServer();
}
void loop()
{
  if(digitalRead(2) == HIGH){
    while(digitalRead(2) == HIGH){}
    toggleStatus();
    updateStatus();
    updateLight();  
  }

  if((millis() - currentTime) > timeUntilNextUpdate){
    currentTime = millis();
    postTemp();  
    
  }
  if((millis() - currentTime) > timeCheckStatus){
    currentTime = millis();  
    updateStatus();
    updateLight();
  }

  if(client.connected() == false){
    disconnectServer();
  }
}


////////////////////////////////////////////////////////////////////////////////////
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
