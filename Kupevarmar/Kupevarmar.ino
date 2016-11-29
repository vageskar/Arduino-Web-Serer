#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 1, 178 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 1, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port
bool heaterStatus = false;
bool timer = false;
String readString;
unsigned long timerTime;

void setup() {
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
        }

        //if HTTP request has ended
        if (c == '\n') {
          printWebPage(client);
          handleInput();
        }
      }
    }
  }
  handleHeater();
}
void printWebPage(EthernetClient client) {
  String heaterStatusTxt;
  if (heaterStatus) {
    if (timer) {
      long timeLeft = timerTime - millis();
      int h = (timeLeft / 3600000) % 24;
      int m = (timeLeft / 60000) % 60;
      String temp;
      if(h < 1){
        temp = "<H3>Kupevarmar på. Tid igjen: ";
        temp += m;
        temp += " min</h3";
      }
      else{
        temp = "<H3>Kupevarmar på. Tid igjen: ";
        temp += h;
        temp += " time og ";
        temp += m;
        temp += " min</h3";
      }
      heaterStatusTxt = "<H3>Kupevarmar på</H1>";
    }
    else {
      heaterStatusTxt = "<H3>Kupevarmar på</H3>";
    }
  }
  else {
    heaterStatusTxt = "<H3>Kupevarmar av</H3>";
  }
  client.println("HTTP/1.1 200 OK"); //send new page
  client.println("Content-Type: text/html");
  client.println();
  client.println("<HTML>");
  client.println("<HEAD>");
  client.println("<TITLE>Kupevarmar</TITLE>");
  client.println("</HEAD>");
  client.println("<BODY>");
  client.println("<H1>Skru på og av kupevarmar</H1><br />");
  client.println(heaterStatusTxt);
  client.println("<br />");
  client.println("<a href=\"/?heatOn\"\">Skru på kupevarmar</a><br />");
  client.println("<a href=\"/?heatOff\"\">Skru av kupevarmar</a><br />");
  client.println("<a href=\"/?heatOn1t\"\">Kupevarmar på 1 time</a><br />");
  client.println("<a href=\"/?heatOn2t\"\">Kupevarmar på 2 timar</a><br />");
  client.println("<a href=\"/?heatOn30m\"\">Kupevarmar på 30 min</a><br />");
  client.println("</BODY>");
  client.println("</HTML>");

  delay(1);
  //stopping client
  client.stop();
}

void handleInput() {
  if (readString.indexOf("?heatOn") > 0) {
    heaterStatus = true;
  }
  if (readString.indexOf("?heatOff") > 0) {
    heaterStatus = false;
  }
  if (readString.indexOf("?heatOn1t") > 0) {
    heaterStatus = true;
    timer = true;
    timerTime = millis() + 3600000;
  }
  if (readString.indexOf("?heatOn2t") > 0) {
    heaterStatus = true;
    timer = true;
    timerTime = millis() + 7200000;
  }
  if (readString.indexOf("?heatOn30m") > 0) {
    heaterStatus = true;
    timer = true;
    timerTime = millis() + 1800000;
  }
}

void handleHeater() {
  if (heaterStatus) {
    //Heater on
  }
  else {
    //Heater of
  }
  if (timer && millis() > timerTime) {
    heaterStatus = false;
    timer = false;
  }
}

