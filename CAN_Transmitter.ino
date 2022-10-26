#include <CAN.h>
#include <Wire.h>             //includem libaria CAN si I2C
 
#define Addr 0x1C             //definim adresa I2C pentru accelerometru
 
#define senzor_viteza1 7        //definim pinii pentru senzorii de viteza
#define senzor_viteza2 8
 
void setup() {
  
  Wire.begin();                   //initializam comunicarea I2C ca master
  Serial.begin(9600);             //setam viteza baud
  
  Wire.beginTransmission(Addr);
  Wire.write(0x2A);
  Wire.write(0x00);               
  Wire.endTransmission();
  
  Wire.beginTransmission(Addr);
  Wire.write(0x2A);                           //configurare accelerometru
  Wire.write(0x01);
  Wire.endTransmission();
  
  Wire.beginTransmission(Addr);
  Wire.write(0x0E);
  Wire.write(0x00);
  Wire.endTransmission();
  
  delay(300);
  
  pinMode(senzor_viteza1, INPUT);       //definim pinii de viteza ca INPUT
  pinMode(senzor_viteza2, INPUT);
 
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");  //se initializeaza transmiter-ul CAN la viteza de 500 kbps
    while (1);
  }
}
 
unsigned long end_time = millis() + 500; //delay pentru multitasking
 
float senzor1_val = 0.0f;            //valori turatie
float senzor2_val = 0.0f;
 
int senzor1_steps = 0;
int senzor2_steps = 0;                //counteri pentru senzori
 
bool senzor1_oldstatus = false;      //starea initiala a senzorilor
bool senzor2_oldstatus = false;
 
float xAccl = 0.0f;          
float yAccl = 0.0f;                 //date accelerometru
float zAccl = 0.0f;
unsigned int data[7];

void getAcclData(){
                                //logica accelelometru
  Wire.requestFrom(Addr, 7);                                   
 
  // staus, xAccl msb, xAccl lsb, yAccl msb, yAccl lsb, zAccl msb, zAccl lsb
  if(Wire.available() == 7) 
  {
    data[0] = Wire.read();       
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
  }

// //Convertim datele in 12 biti
 xAccl = (((data[1] * 256) + data[2]) / 16.f) / 1024.f;
 xAccl += 0.044;
 if (xAccl > 2){
     xAccl -= 4;
     }
 yAccl = (((data[3] * 256) + data[4]) / 16.f) / 1024.f;   
 yAccl -= 0.031;         
 if (yAccl > 2){
     yAccl -= 4;
     }
 zAccl = (((data[5] * 256) + data[6]) / 16.f) / 1024.f;
 if (zAccl > 2){
     zAccl -= 4;
     }
}
 
void loop()
{
  if (millis() <= end_time) {
    if (digitalRead(senzor_viteza1) != senzor1_oldstatus) {
      senzor1_steps++; 
      senzor1_oldstatus = digitalRead(senzor_viteza1);                 
    }
                                                                 //calcul turatie 
    if (digitalRead(senzor_viteza2) != senzor2_oldstatus) {
      senzor2_steps++;
      senzor2_oldstatus = digitalRead(senzor_viteza2);
    }
  }
  if (millis() >= end_time) {
    senzor1_val = (senzor1_steps / 10.f); 
    senzor2_val = (senzor2_steps / 10.f);
    
//    Serial.print("Sensor 1: " + String(senzor1_val));
//    Serial.print(" Sensor 2: " + String(senzor2_val));    //afisare date senzori pe serial monitor
//    Serial.println();

    getAcclData();
    
    Serial.println("X       Y        Z");
    Serial.print(xAccl);             // citim si afisam datele de la accelelometru pe serial monitor
    Serial.print("    ");
    Serial.print(yAccl);
    Serial.print("    ");
    Serial.print(zAccl);
    Serial.print("\n");         
   
    CAN.beginPacket(0x12);
    CAN.write(senzor1_steps);         //trimitem datele de la senzori prin CAN
    CAN.write(senzor2_steps);
    CAN.endPacket();
 
    delay(10);
 
    CAN.beginPacket(0x11);
    CAN.write(data[1]);        
    CAN.write(data[2]);              //trimitem datele de la accelerometru prin CAN
    CAN.write(data[3]);
    CAN.write(data[4]);
    CAN.endPacket();
 
    end_time = millis() + 500;      //reinitializam variabilele de la senzori
    senzor1_steps = 0; 
    senzor2_steps = 0;
  }
}
