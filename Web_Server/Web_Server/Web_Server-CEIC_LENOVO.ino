#include <SPI.h>
#include <Ethernet.h>
#include <TinkerKit.h>

#define REFRESH_RATE 3;  
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress ip(192,168,2,14);
EthernetServer server(80);
TKThermistor therm(I4);  
TKLed led(O0);
float C, F;
float pct_of_light;		
int sensorReading;
EthernetClient client;

void setup() 
{
  Serial.begin(9600);
   // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() 
{
  led.off();
  // listen for incoming clients
  client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        led.on();
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline and the line is blank, the http request has ended, so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          send_response_header();
          // compose HTML
          client.println("<html>");
          
          client.print ("Bienvenue a LAKEVIEW HOME Centre de commande");client.println("<br />");client.println("<br />");
          
          // GRAND POTENTIOMETRE
          sensorReading = analogRead(0);
          client.print("Grand Potentiometre= ");client.print(sensorReading);client.println("<br />"); 
          
          // PETIT POTENTIOMETRE
          sensorReading = analogRead(2);
          client.print("Petit Potentiometre= ");client.print(sensorReading);client.println("<br />");
          
          //DETECTEUR de LUMIERE
          sensorReading = analogRead(1);
          client.print("IL FAIT ");
          if (sensorReading<50) {  client.print("NOIR");}
          if (sensorReading>50 && sensorReading<120) {  client.print("UN PEU DE LUMIERE");}
          if (sensorReading>120) {  client.print("JOUR");}
          client.print(" (Niveau de lumiere= ");
          pct_of_light= ((float)sensorReading/1024)*100;
          client.print(pct_of_light); client.print(" %)");
          client.println("<br />");
          
           //CAPTEUR de TOUCHER
          sensorReading = analogRead(3);
          if (sensorReading>50) {  client.print("APPELLE A TOUS LES EMPLOYEES DE LAKEVIEW");} client.println("<br />");client.println("<p/>");
          
         //DETECTEUR de LUMIERE
          C = therm.readCelsius();     	
          F = therm.readFahrenheit(); 
          sensorReading=therm.read();
          client.print("TEMPERATURE A LAKEVIEW HOME= ");
          client.print(C); client.print(" celcius / "); client.print(F); client.print(" fahrenheit");
          client.println("<br />");
          
          sensorReading = analogRead(5);
          if (sensorReading>50) { client.println("<br/>"); client.print("*** ALARME ***"); client.println("<br />");}

          client.println("<p/>");client.print ("MR le Directeur ENZOVT");
         
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    led.off();
  }
}

void send_response_header()
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Refresh: 3"); 
  client.println();
  client.println("<!DOCTYPE HTML>");
}

