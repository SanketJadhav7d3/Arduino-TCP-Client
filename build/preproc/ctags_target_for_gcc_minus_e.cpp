# 1 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino"
# 2 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino" 2
# 3 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino" 2
# 4 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino" 2
# 5 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino" 2




# 8 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino"
SoftwareSerial sim900l(2,3);

//GPS Module RX pin to Arduino 9
//GPS Module TX pin to Arduino 8
AltSoftSerial neogps;

TinyGPSPlus gps;

unsigned long previousMillis = 0;
long interval = 60000;

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);

  //Begin serial communication with Arduino and SIM900L
  sim900l.begin(9600);

  //Begin serial communication with Arduino and SIM800L
  neogps.begin(9600);

  Serial.println("Initializing...");
  //delay(10000);


  //Once the handshake test is successful, it will back to OK
  sendATcommand("AT", "OK", 2000);
  sendATcommand("AT+CMGF=1", "OK", 2000);
  //sim900l.print("AT+CMGR=40\r");
}

void loop()
{
    while(sim900l.available()){
        Serial.println(sim900l.readString());
    }
    while(Serial.available()) {
        sim900l.println(Serial.readString());
    }
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;
        sendGpsToServer();
    }
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
    if(true){
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
      //if (latitude == 0) {return 0;}
      String url, temp;
      url = "http://ENTER_YOUR_WEBSITE/gpsdata.php?lat=";
      url += latitude;
      url += "&lng=";
      url += longitude;

      sendDataToServer(latitude, longitude);
  }
  return 1;
}

void sendDataToServer(String latitude, String longitude) {
    String url;
      url = "http://ENTER_YOUR_WEBSITE/gpsdata.php?lat=";
      url += latitude;
      url += "&lng=";
      url += longitude;
}

int readDataFromServer() {
    // to get geofenced status from the server
    return 0;
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0, answer=0;
    char response[100];
    unsigned long previous;

    //Initialice the string
    memset(response, '\0', 100);
    delay(100);

    //Clean the input buffer
    while( sim900l.available() > 0) sim900l.read();

    if (ATcommand[0] != '\0'){
      //Send the AT command 
      sim900l.println(ATcommand);
    }

    x = 0;
    previous = millis();

    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim900l.available() != 0){
            response[x] = sim900l.read();
            //Serial.print(response[x]);
            x++;
            // check if the desired answer (OK) is in the response of the module
            if(strstr(response, expected_answer) != 
# 138 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino" 3 4
                                                   __null
# 138 "/Users/sanketjadhav/Documents/Arduino/tcp_client/tcp_client.ino"
                                                       ){
                answer = 1;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}

int getGeoFenceStatus() {

}
