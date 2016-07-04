//Libraries
#include <virtuabotixRTC.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"

//Defines the sensor model and port used
#define DHTPIN 7
#define DHTTYPE DHT22 // DHT 22 (AM2302)

LiquidCrystal lcd(25, 24, 6, 5, 3, 2);
virtuabotixRTC RTC(8, 9, 10);
DHT dht(DHTPIN, DHTTYPE);
File myFile;
int hygrometer1 = A8;
int hygrometer2 = A9;
int hygrometer3 = A10;
int solenoid = 22;

void setup() {
  Serial.begin(9600);
  pinMode(solenoid, OUTPUT);
  lcd.begin(20, 4);
  dht.begin();
  
  //seconds, minutes, hours, dayoftheweek
  RTC.setDS1302Time(30, 33, 13, 4, 29, 06, 2016);

  Serial.println("UENP-Agronomia/Ciencia da Computacao");
  Serial.println("---------------");

  lcd.setCursor(8, 0);
  lcd.print("UENP");
  lcd.setCursor(0, 1);
  lcd.print("Agronomia");
  lcd.setCursor(0, 2);
  lcd.print("Cien. da Computacao");
  lcd.setCursor(0, 3);
  lcd.print("Allan Pires");
  delay(5000);
  lcd.clear();

  Serial.print("Inicializating card...");
  if (!SD.begin(53)) {
    Serial.println("Inicialization failed!");
    return;
  }
  Serial.println("Inicialization done!");

  Serial.print("Inicializating archive...");
  myFile = SD.open("test.txt", FILE_WRITE);
  if (!myFile) {
    Serial.println("ERROR");
  }
  Serial.println("OK");
}

void loop() {
  RTC.updateTime();
  Serial.println("---------------");
  updateTimeLCD();
  updateHT();
  updateHygrometer();
  Serial.println("---------------");
}

void updateTimeLCD(){
   Serial.print(RTC.dayofmonth);
   Serial.print("/");
   Serial.print(RTC.month);
   Serial.print("/");
   Serial.print(RTC.year);
   Serial.print("   ");
   
   lcd.setCursor(0,0);
   lcd.print(RTC.dayofmonth);
   lcd.print("/");
   lcd.print(RTC.month);
   lcd.print("/");
   lcd.print(RTC.year);
   lcd.print("-");
   // Add 0 case the value of hours be < 10
   Serial.print("Time: ");
   if (RTC.hours < 10){
     Serial.print("0");
     lcd.print("0");
   }
   Serial.print(RTC.hours);
   Serial.print(":");
   lcd.print(RTC.hours);
   lcd.print(":");
   // Add 0 case the value of minutes be < 10
   if (RTC.minutes < 10){
     Serial.print("0");
     lcd.print("0");
   }
   Serial.print(RTC.minutes);
   Serial.print(":");
   lcd.print(RTC.minutes);
   lcd.print(":");
   // Add 0 case the value of seconds be < 10
   if (RTC.seconds < 10){
     Serial.print("0");
     lcd.print("0");
   }
   Serial.print(RTC.seconds);
   Serial.print(":");
   lcd.print(RTC.seconds);
   lcd.print(":");
   delay(2000);
   
   saveDate(RTC.dayofmonth, RTC.month, RTC.year, RTC.hours, RTC.minutes, RTC.seconds);
   delay(5000);
}

void updateHT(){
   float h = dht.readHumidity();
   float t = dht.readTemperature();
  
   if(isnan(h) || isnan(t)){
      Serial.println("DHT 22 error");
      lcd.print("DHT 22 error");
      return;
   }
     
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");
  
  lcd.setCursor(0,3);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print(" %");
  delay(2000);
  
  lcd.setCursor(0,3);
  lcd.print("Temperature: ");
  lcd.print(t);
  lcd.print(" C");
  delay(2000);
 
  myFile = SD.open("projectArduino.txt", FILE_WRITE);
  save("Humidity: ", h, " %");
  delay(2000);
 
  myFile = SD.open("projectArduino.txt", FILE_WRITE);
  save("Temperature: ", t, " C");
  delay(2000); 
}

void updateHygrometer(){
   hygrometer1 = analogRead(A8);
   hygrometer2 = analogRead(A9);
   hygrometer3 = analogRead(A10);
   
   int porcHyg1 = map(hygrometer1, 1023, 0, 0, 100);
   int porcHyg2 = map(hygrometer2, 1023, 0, 0, 100);
   int porcHyg3 = map(hygrometer3, 1023, 0, 0, 100);
   
  Serial.print("Hygrometer 1: ");
  Serial.print(porcHyg1);
  Serial.println(" %");
  Serial.print("Hygrometer 2: ");
  Serial.print(porcHyg2);
  Serial.println(" %");
  Serial.print("Hygrometer 3: ");
  Serial.print(porcHyg3);
  Serial.println(" %");
  
  lcd.setCursor(0, 1);
  lcd.print("Hygrometer 1: ");
  lcd.print(porcHyg1);
  lcd.print(" %");
  lcd.setCursor(0, 2);
  lcd.print("Hygrometer 2: ");
  lcd.print(porcHyg2);
  lcd.print(" %");
  lcd.setCursor(0, 2);
  lcd.print("Hygrometer 3: ");
  lcd.print(porcHyg3);
  lcd.print(" %");
  delay(2000);

  if (porcHyg1 >= 37 && porcHyg2 >= 37 && porcHyg3 >= 37) {
    Serial.println("...Optimal Level...");
    lcd.setCursor(0, 3);
    lcd.print("...Optimal Level....");
    digitalWrite(solenoid, LOW); //Turn off solenoid
  }
  else if (porcHyg1 >= 28 && porcHyg1 < 37) {
    Serial.println("...Middle Level...");
  }
  else if (porcHyg1 < 28 || porcHyg2 < 28 || porcHyg3 < 28) {
    Serial.println("...Irrigating...");
    lcd.setCursor(0, 3);
    lcd.print("...Irrigating...    ");
    digitalWrite(solenoid, HIGH); //Turn on solenoid
  }
  
  myFile = SD.open("projectArduino.txt", FILE_WRITE);
  save("Hygrometer 1: ", porcHyg1, " %");
  delay(2000);
  
  myFile = SD.open("projectArduino.txt", FILE_WRITE);
  save("Hygrometer 2: ", porcHyg2, " %");
  delay(2000);
  
  myFile = SD.open("projectArduino.txt", FILE_WRITE);
  save("Hygrometer 3: ", porcHyg3, " %");
  delay(2000);
}

void save(String text, float data, String text2){
  if(myFile){
    Serial.print("Saving data...");
    myFile.print(text);
    myFile.print(data);
    myFile.print(text2);
    myFile.flush();
    myFile.close();
    Serial.println("Saved");
  }
  else{
    Serial.println("Error");
  }
}

void saveDate(int day, int month, int year, int hours, int minutes, int seconds){
  myFile = SD.open("testFile.txt", FILE_WRITE);
  if(!SD.open("projectArduino.txt")){
    Serial.println("ERROR projectArduino.txt");
  }
  if(myFile){
    Serial.print("Saving date...");
    myFile.println("----------");
    myFile.print("Date: ");
    myFile.print(day);
    myFile.print("/");
    myFile.print(month);
    myFile.print("/");
    myFile.println(year);
    myFile.print("Hours: ");
    myFile.print(hours);
    myFile.print(":");
    myFile.print(minutes);
    myFile.print(":");
    myFile.println(seconds);
    myFile.flush();
    myFile.close();
    Serial.println("Saved.");
  } else {
    Serial.println("Error to open the projectArduino.txt");
  }
}
