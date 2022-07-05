#include "SoftwareSerial.h"
#include "TinyGPSPlus.h"
#include "AltSoftSerial.h"
#include "String.h"

#define rxPin 2
#define txPin 3

// GSM Module RX pin to Arduino 3
// GSM Module TX pin to Arduino 2
SoftwareSerial sim900(rxPin,txPin);

// GPS Module RX pin to Arduino 9
// GPS Module TX pin to Arduino 8
AltSoftSerial neogps;

TinyGPSPlus gps;

unsigned long previousMillis = 0;
int geoFenceStatus = 0;
long interval = 60000;

void setup()
{
  // begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200); 

  // begin serial communication with Arduino and sim900
  sim900.begin(9600);

  // begin serial communication with Arduino and SIM900A
  neogps.begin(9600);


  Serial.println("Initializing...");
  delay(10000);

  // check intial connection with GSM SIM900A
  sendATcommand("AT", "OK", 2000);
  sendATcommand("AT+CMGF=1", "OK", 2000);
}

void loop()
{
    while(sim900.available()){
        Serial.println(sim900.readString());
    }

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;
        sendGpsToServer();
    }
}

int sendGpsToServer()
{
    // can take up to 60 seconds
    boolean newData = false;
    for (unsigned long start = millis(); millis() - start < 2000;){
      while (neogps.available()){
        if (gps.encode(neogps.read())){
          newData = true;
          break;
        }
      }
    }

    // if newData is true
    if(newData){
        newData = false;
        String latitude, longitude;
        float altitude;
        unsigned long date, time, speed, satellites;

        latitude = String(gps.location.lat(), 6); 
        longitude = String(gps.location.lng(), 6);
        altitude = gps.altitude.meters(); 
        date = gps.date.value(); 
        time = gps.time.value();
        speed = gps.speed.kmph();
        Serial.print("Latitude= "); 
        Serial.println(latitude);
        Serial.print(" Longitude= "); 
        Serial.println(longitude);

        // send data to the server
        sendDataToServer(latitude, longitude);
        return 0;
    }
    return 1;
}

void sendDataToServer(String latitude, String longitude) {
    String url;
    url = "GET /update?api_key=<your thingspeak api key>&field1=";
    url += latitude;
    url += "&field2=";
    url += longitude;
    url += "\r\n\x1a";

    Serial.println("TCP Send :");
    Serial.print("AT\\r\\n");
    sim900.println("AT"); 
    delay(5000);
    ShowSerialData();	
    delay(5000);
    Serial.print("AT+CIPMODE=0\\r\\n");
    sim900.println("AT+CIPMODE=0");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPMUX=0\\r\\n");
    sim900.println("AT+CIPMUX=0");
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT=1\\r\\n");
    sim900.println("AT+CGATT=1");
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CREG?\\r\\n");
    sim900.println("AT+CREG?");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT?\\r\\n");
    sim900.println("AT+CGATT?")
    delay(5000); 
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CSTT=\"airtelgprs.com\",\"\",\"\"\\r\\n");
    sim900.println("AT+CSTT=\"airtelgprs.com\",\"\",\"\"");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIICR\\r\\n");
    sim900.println("AT+CIICR");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIFSR\\r\\n");
    sim900.println("AT+CIFSR");
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\\r\\n");
    sim900.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSEND\\r\\n");
    sim900.println("AT+CIPSEND");
    delay(2000);
    ShowSerialData();
    delay(2000);
    Serial.println("Sending coordinates to the server");
    sim900.print(url);
    delay(10000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSHUT\\r\\n");
    sim900.println("AT+CIPSHUT");
    delay(5000);
    ShowSerialData();
    delay(5000);
}

void readDataFromServer() {
    // to get geofenced status from the server
    Serial.println("TCP Receive :");
    Serial.print("AT\\r\\n");
    sim900.println("AT"); 
    delay(5000);
    ShowSerialData();	
    delay(5000);
    Serial.print("AT+CIPMODE=0\\r\\n");	
    sim900.println("AT+CIPMODE=0");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPMUX=0\\r\\n");
    sim900.println("AT+CIPMUX=0");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT=1\\r\\n");
    sim900.println("AT+CGATT=1");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CREG?\\r\\n");
    sim900.println("AT+CREG?");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CGATT?\\r\\n");
    sim900.println("AT+CGATT?");	
    delay(5000); 
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CSTT=\"airtelgprs.com\",\"\",\"\"\\r\\n");     // insert your sim card apn
    sim900.println("AT+CSTT=\"aritelgprs.com\",\"\",\"\"");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIICR\\r\\n");
    sim900.println("AT+CIICR");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIFSR\\r\\n");
    sim900.println("AT+CIFSR");
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\\r\\n");
    sim900.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");	
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSEND\\r\\n");
    sim900.println("AT+CIPSEND");	
    delay(2000);
    ShowSerialData();
    delay(2000);
    Serial.print("GET https://api.thingspeak.com/channels/1597049/fields/1.json?results=1\\r\\n");
    sim900.print("GET https://api.thingspeak.com/channels/1597049/fields/1.json?results=1\r\n\x1A"); 
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print("AT+CIPSHUT\\r\\n");
    sim900.println("AT+CIPSHUT");	
    delay(5000);
    ShowSerialData();
    delay(5000);
}

void ShowSerialData()
{
    while(sim900.available()!=0)	
    Serial.write(char (sim900.read()));	
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){
    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    // intialize the string
    memset(response, '\0', 100);
    delay(100);
    // clean the input buffer
    while( sim900.available() > 0) sim900.read();
    if (ATcommand[0] != '\0'){
      // send the AT command 
      sim900.println(ATcommand);
    }

    x = 0;
    previous = millis();
    // this loop waits for the answer with time out
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim900.available() != 0){
            response[x] = sim900.read();
            // Serial.print(response[x]);
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
