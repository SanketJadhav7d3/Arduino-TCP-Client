#include "SoftwareSerial.h"
#include "TinyGPSPlus.h"
#include "AltSoftSerial.h"
#include "String.h"

#define rxPin 2
#define txPin 3

SoftwareSerial sim900(rxPin,txPin);

//GPS Module RX pin to Arduino 9
//GPS Module TX pin to Arduino 8
AltSoftSerial neogps;

TinyGPSPlus gps;

unsigned long previousMillis = 0;
int geoFenceStatus = 0;
long interval = 60000;

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200); 

  //Begin serial communication with Arduino and sim900
  sim900.begin(9600);

  //Begin serial communication with Arduino and SIM800L
  neogps.begin(9600);

  Serial.println("Initializing...");
  //delay(10000);

  //Once the handshake test is successful, it will back to OK
  sendATcommand("AT", "OK", 2000);
  sendATcommand("AT+CMGF=1", "OK", 2000);
  //sim900.print("AT+CMGR=40\r");
}

void loop()
{
    while(sim900.available()){
        Serial.println(sim900.readString());
    }
    while(Serial.available())  {
        sim900.println(Serial.readString());
    }
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;
        sendGpsToServer();
    }
    // read the geofence status from the server
    readDataFromServer();
}

int sendGpsToServer()
{
    //Can take up to 60 seconds
    boolean newData = false;
    for (unsigned long start = millis(); millis() - start < 2000;){
      while (neogps.available()){
        if (gps.encode(neogps.read())){
          newData = true;
          break;
        }
      }
    }
    //If newData is true
    if(newData){
        newData = false;
        String latitude, longitude;
        float altitude;
        unsigned long date, time, speed, satellites;

        latitude = String(gps.location.lat(), 6); // Latitude in degrees (double)
        longitude = String(gps.location.lng(), 6); // Longitude in degrees (double)
        altitude = gps.altitude.meters(); // Altitude in meters (double)
        date = gps.date.value(); // Raw date in DDMMYY format (u32)
        time = gps.time.value(); // Raw time in HHMMSSCC format (u32)
        speed = gps.speed.kmph();
        Serial.print("Latitude= "); 
        Serial.print(latitude);
        Serial.print(" Longitude= "); 
        Serial.println(longitude);

        // send data to the server
        sendDataToServer(latitude, longitude, "34");
    }
    return 1;
}

void sendDataToServer(String latitude, String longitude, String rotY) {
    String url;
    url = "GET /update?api_key=UBKWDZUZJ01YHYQ5&field1=";
    url += latitude;
    url += "&field2=";
    url += longitude;
    url += "&field3";
    url += rotY;
    url += "\r\n\x1a";

    Serial.println("TCP Send :");
    Serial.print("AT\\r\\n");
    sim900.println("AT"); /* Check Communication */
    delay(5000);
    ShowSerialData();	/* Print response on the serial monitor */
    delay(5000);
    Serial.print("AT+CIPMODE=0\\r\\n");
    sim900.println("AT+CIPMODE=0");	/* Non-Transparent (normal) mode for TCP/IP application */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPMUX=0\\r\\n");
    sim900.println("AT+CIPMUX=0");	/* Single TCP/IP connection mode */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT=1\\r\\n");
    sim900.println("AT+CGATT=1");	/* Attach to GPRS Service */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CREG?\\r\\n");
    sim900.println("AT+CREG?");	/* Network registration status */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT?\\r\\n");
    sim900.println("AT+CGATT?");	/* Attached to or detached from GPRS service */ 
    delay(5000); 
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CSTT=\"airtelgprs.com\",\"\",\"\"\\r\\n");
    sim900.println("AT+CSTT=\"airtelgprs.com\",\"\",\"\"");	/* Start task and set APN */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIICR\\r\\n");
    sim900.println("AT+CIICR");	/* Bring up wireless connection with GPRS */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIFSR\\r\\n");
    sim900.println("AT+CIFSR");	/* Get local IP address */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\\r\\n");
    sim900.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");	/* Start up TCP connection */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSEND\\r\\n");
    sim900.println("AT+CIPSEND");	/* Send data through TCP connection */
    delay(2000);
    ShowSerialData();
    delay(2000);
    Serial.print("Sending coordinates to the server");
    sim900.print(url);	/* URL for data to be sent to */
    delay(10000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSHUT\\r\\n");
    sim900.println("AT+CIPSHUT");	/* Deactivate GPRS PDP content */
    delay(5000);
    ShowSerialData();
    delay(5000);
}

void readDataFromServer() {
    // to get geofenced status from the server
    Serial.println("TCP Receive :");
    Serial.print("AT\\r\\n");
    sim900.println("AT"); /* Check Communication */
    delay(5000);
    ShowSerialData();	/* Print response on the serial monitor */
    delay(5000);
    Serial.print("AT+CIPMODE=0\\r\\n");	
    sim900.println("AT+CIPMODE=0");	/* Non-Transparent (normal) mode for TCP/IP application */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPMUX=0\\r\\n");
    sim900.println("AT+CIPMUX=0");	/* Single TCP/IP connection mode */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT=1\\r\\n");
    sim900.println("AT+CGATT=1");	/* Attach to GPRS Service */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CREG?\\r\\n");
    sim900.println("AT+CREG?");	/* Network registration status */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT?\\r\\n");
    sim900.println("AT+CGATT?");	/* Attached to or detached from GPRS service */ 
    delay(5000); 
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CSTT=\"TATA.DOCOMO.INTERNET\",\"\",\"\"\\r\\n");
    sim900.println("AT+CSTT=\"TATA.DOCOMO.INTERNET\",\"\",\"\"");	/* Start task and set APN */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIICR\\r\\n");
    sim900.println("AT+CIICR");	/* Bring up wireless connection with GPRS */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIFSR\\r\\n");
    sim900.println("AT+CIFSR");	/* Get local IP address */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\\r\\n");
    sim900.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");	/* Start up TCP connection */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSEND\\r\\n");
    sim900.println("AT+CIPSEND");	/* Send data through TCP connection */
    delay(2000);
    ShowSerialData();
    delay(2000);
    Serial.print("GET https://api.thingspeak.com/channels/1597049/fields/1.json?results=1\\r\\n");
    sim900.print("GET https://api.thingspeak.com/channels/1597049/fields/1.json?results=1\r\n\x1A"); /* URL for data to be read from */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSHUT\\r\\n");
    sim900.println("AT+CIPSHUT");	/* Deactivate GPRS PDP content */
    delay(5000);
    ShowSerialData();
    delay(5000);
}

void ShowSerialData()
{
    while(sim900.available()!=0)	/* If data is available on serial port */
    Serial.write(char (sim900.read()));	/* Print character received on to the serial monitor */
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){
    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    //Initialice the string
    memset(response, '\0', 100);
    delay(100);
    //Clean the input buffer
    while( sim900.available() > 0) sim900.read();
    if (ATcommand[0] != '\0'){
      //Send the AT command 
      sim900.println(ATcommand);
    }

    x = 0;
    previous = millis();
    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim900.available() != 0){
            response[x] = sim900.read();
            //Serial.print(response[x]);
            x++;
            // check if the desired answer (OK) is in the response of the module
            if(strstr(response, expected_answer) != NULL){
                answer = 1;
            }
        }
    } while((answer == 0) && ((millis() - previous) < timeout));
    Serial.println(response);
    return answer;
}
