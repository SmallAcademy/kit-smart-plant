#include <SoftwareSerial.h>

// LED
int ledPin = 13;

// variable pentru senzorul de umiditate a solului
int sensor_pin0 = A0;
int sensor_pin1 = A1;

// variable pentru luminozitate și umiditate
int umiditate;
int luminozitate;

// variabile pentru conexiune la API
// !! contacteaza-ne pe https://small.academy/contact/
// !! pentru a primi datele tale
String apiIP    = "schimba_ip_aici";
String apiToken = "schimba_token_aici";
int apiUserID   = 0; // numarul de identificare al utilizatorului
int apiDeviceID = 0; // numarul de identificare al device-ului (plantei)

// conectare 2 la TX si 3 la RX, in Serial USB
SoftwareSerial ser(2,3); // RX, TX

// functia de initializare. se executa la inceput
void setup() {
  // initializare pin digital ca output
  pinMode(ledPin, OUTPUT);

  // porneste debug serial
  Serial.begin(115200);

  // porneste software serial
  ser.begin(115200);

  // resetare modul Wi-Fi ESP8266
  ser.println("AT+RST");
}


// functie ce se executa non-stop
void loop() {

  // aprindem si stingem led-ul de pe placa
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);

  // functie ce preia informatiile de la senzori si le trimite la server
  esp_8266();
}

void esp_8266() {

  // transformăm valorile primite in procente
  // si le afisam in serial
  umiditate = analogRead(sensor_pin0);
  umiditate = map(umiditate,700,190,0,100);
  Serial.print("Umiditate: ");
  Serial.print(umiditate);
  Serial.println("%");
  luminozitate = analogRead(sensor_pin1);
  luminozitate = map(luminozitate,0,800,0,100);
  Serial.print("Luminozitate: ");
  Serial.print(luminozitate);
  Serial.println("%");

  // ne conectam la server
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += apiIP;
  cmd += "\",80";
  ser.println(cmd);
  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }

  // trimitem datele către server
  String getStr = "GET /online/api/iot/";
  getStr += apiUserID;
  getStr += "/";
  getStr += apiDeviceID;
  getStr += "/?";
  getStr +="umiditate=";
  getStr += String(umiditate);
  // pentru urmatoarele variabile, se adauga "&" inainte
  getStr +="&luminozitate=";
  getStr += String(luminozitate);
  getStr += "\r\n\r\n";
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);

  //verificăm daca se trimit datele sau afisam un mesaj de eroare
  if(ser.find(">")){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");
  }

  // asteptam un minut pana sa retrimitem valorile
  delay(60000);
}
