//Bibliotecas Necessárias
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"

//Declarando posição
#define DHTPIN 7
#define DHTTYPE DHT22
#define DS1307_ADDRESS 0x68

//Declarando os componentes
LiquidCrystal lcd(9,8,5,6,3,2);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
DHT dht(DHTPIN, DHTTYPE);
File myFile;
byte zero = 0x00;

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  dht.begin();
  Wire.begin();
  //selecionaRTC(); 
 
  //Inicialização dos sensores
  if(!bmp.begin()){
    Serial.print("Erro BMP180...Verifique as conexoes");
    while(1);
  }
  
  Serial.print("Inicicializando cartao...");
  pinMode(10, OUTPUT);
  if(!SD.begin(10)){
    Serial.println("Inicializacao falhou");
    return;
  }
  Serial.println("Inicializacao feita");
  Serial.print("Inicializando arquivo...");
  myFile = SD.open("test1.txt", FILE_WRITE);
  if(myFile){
    Serial.println("OK");
    myFile.print("Data \t\t");
    myFile.print("Hora \t\t");
    myFile.print("Humidade \t");
    myFile.print("Temperatura \t");
    myFile.print("PressaoAtm \t");
    myFile.println("Altitude");
    myFile.close();
  }
  else{
      Serial.println("ERRO");
  }   
}

void loop() {
  sensors_event_t event;
  bmp.getEvent(&event);
  float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
  float temperature;  
  
  Serial.println("--------------");
  
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  int segundos = ConverteparaDecimal(Wire.read());
  int minutos = ConverteparaDecimal(Wire.read());
  int horas = ConverteparaDecimal(Wire.read() & 0b111111); 
  int diadasemana = ConverteparaDecimal(Wire.read()); 
  int diadomes = ConverteparaDecimal(Wire.read());
  int mes = ConverteparaDecimal(Wire.read());
  int ano = ConverteparaDecimal(Wire.read());
  //Mostra a data no Serial Monitor
  //Serial.print("Data: ");
  Serial.print(diadomes);
  Serial.print("/");
  Serial.print(mes);
  Serial.print("/");
  Serial.println(ano);
  
  //Serial.print("Hora: ");
  Serial.print(horas);
  Serial.print(":");
  Serial.print(minutos);
  Serial.print(":");
  Serial.print(segundos);
  Serial.println();
  lcd.setCursor(0,0);
  lcd.print(horas);
  lcd.print(":");
  lcd.print(minutos);
  lcd.print(":");
  lcd.print(segundos);
  lcd.print("-");
  lcd.print(diadomes);
  lcd.print("/");
  lcd.print(mes);
  lcd.print("/");
  lcd.print(ano);  
  delay(3000);
    
  if(event.pressure){
      //Serial.print("Pressao: ");
      Serial.print(event.pressure);
      Serial.println(" hPa");
      lcd.setCursor(0,0);
      lcd.print("PA: ");      
      lcd.print(event.pressure);      
      lcd.print(" hPa  ");         
            
      bmp.getTemperature(&temperature);
      //Serial.print("Temperatura: ");
      Serial.print(temperature);
      Serial.println(" C");
      lcd.setCursor(0,1);
      lcd.print("Temp: ");
      lcd.print(temperature);
      lcd.print(" C   ");
      delay(3000);
            
      //Serial.print("Altitude: ");
      Serial.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure));
      Serial.println(" m");           
  } 
  
  float h = dht.readHumidity();
  if(isnan(h)){
    Serial.print("Erro DHT22...Verifique as conexoes");
    return;
  }
   
  //Serial.print("Humidade: ");
  Serial.print(h);
  Serial.println(" %");
  lcd.setCursor(0,1);
  lcd.print("Humid: ");  
  lcd.print(h);  
  lcd.print(" %");
 
  Serial.println("---------------"); 
      
  //Verificando se existe o arquivo no cartão
  myFile = SD.open("test1.txt", FILE_WRITE);
  //SALVAR DADOS
  if (myFile) {
    Serial.print("Salvando dados..");
    //Data
    myFile.print(diadomes);
    myFile.print("/");
    myFile.print(mes);
    myFile.print("/");
    myFile.print(ano);
    myFile.print("\t");
    //Hora
    myFile.print(horas);
    myFile.print(":");
    myFile.print(minutos);
    myFile.print(":");
    myFile.print(segundos);
    myFile.print("\t\t");
    //Humidade do ar
    myFile.print(h);
    myFile.print("\t\t");
    //Temperatura
    myFile.print(temperature);
    myFile.print("\t\t");
    //Pressao
    myFile.print(event.pressure);
    myFile.print("\t\t");
    //Altitude    
    myFile.println(bmp.pressureToAltitude(seaLevelPressure, event.pressure));
    myFile.flush();
    myFile.close();
    Serial.println("Salvo.");
  } else {
    Serial.println("Erro ao abrir o test.txt");
  }
  delay(5000);
}

void selecionaRTC(void){
  byte segundos = 00; //Valores de 0 a 59
  byte minutos = 43 ; //Valores de 0 a 59
  byte horas = 11; //Valores de 0 a 23
  byte diadasemana = 3; //Valores de 0 a 6 - 0=Domingo, 1 = Segunda, etc.
  byte diadomes = 26; //Valores de 1 a 31
  byte mes = 10; //Valores de 1 a 12
  byte ano = 16; //Valores de 0 a 99
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //Stop no CI para que o mesmo possa receber os dados

  //As linhas abaixo escrevem no CI os valores de 
  //data e hora que foram colocados nas variaveis acima
  Wire.write(ConverteParaBCD(segundos));
  Wire.write(ConverteParaBCD(minutos));
  Wire.write(ConverteParaBCD(horas));
  Wire.write(ConverteParaBCD(diadasemana));
  Wire.write(ConverteParaBCD(diadomes));
  Wire.write(ConverteParaBCD(mes));
  Wire.write(ConverteParaBCD(ano));
  Wire.write(zero); //Start no CI
  Wire.endTransmission(); 
}

byte ConverteParaBCD(byte val){ //Converte o número de decimal para BCD
  return ( (val/10*16) + (val%10) );
}

byte ConverteparaDecimal(byte val)  { //Converte de BCD para decimal
  return ( (val/16*10) + (val%16) );
}
