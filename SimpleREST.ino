#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,69, 252);
EthernetServer server(80);
char arg1[20] = "";
char arg2[20]= "";
int arg3;
int arg4;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
          String HTTP_request = client.readStringUntil('\n');
          Serial.print("HTTP request: ");  // DEBUG
          Serial.println(HTTP_request);
        if ((HTTP_request.startsWith("GET ")) && (HTTP_request.endsWith("\r"))) {
          sscanf(HTTP_request.c_str(), "GET /%99[^&/ ]/%99[^/]/%99d/%99d", arg1, arg2, &arg3, &arg4);
          HTTP_switch(client, arg1, arg2, arg3, arg4);
        } 
      }
      client.stop();
      delay(1);
    }
  }
}

void HTTP_switch(EthernetClient client, char arg1[20], char oper[20], int pin, int value){
  int pinValue;
  if ((strcmp(arg1, "") == 0) || (strcmp(arg1, "index.htm") == 0) || (strcmp(arg1, "index.html") == 0)) {
    HTTP_reply_index(client);
  } else if (strcmp(arg1, "arduino") == 0) {
      if (strcmp(oper, "digitalWrite") == 0) {
        digitalWrite(pin, value);
        HTTP_reply(client, value);
      } else if (strcmp(oper, "digitalRead") == 0) {
        pinValue = digitalRead(pin);
        HTTP_reply(client, pinValue);
      } else if (strcmp(oper, "analogWrite") == 0) {
        analogWrite(pin, value);
        HTTP_reply(client, value);
      } else if (strcmp(oper, "analogRead") == 0) {
        pinValue = analogRead(pin);
        HTTP_reply(client, value);
      } else {
        HTTP_reply_invalid(client);
      }
  } else {
    HTTP_reply_invalid(client);
  }
}


void HTTP_reply(EthernetClient client, int value) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println(value);
}

void HTTP_reply_invalid(EthernetClient client){
  client.println("HTTP/1.1 406 Not Acceptable");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
}

void HTTP_reply_index(EthernetClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<html><body>");
  client.println("Request example: /arduino/digitalWrite/5/1");
  client.println("</body></html>");
}

