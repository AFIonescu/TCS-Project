#include <CAN.h>                 //includem libraria can
 
#define pedala_acceleratie A1  //definim potentiometrul
 
#define punte_fata_PWM 3     //definim pinii PWM
#define punte_spate_PWM 9          
 
#define punte_fata_pin1 4               //definim pinii motoarelor
#define punte_fata_pin2 5
#define punte_spate_pin1 6
#define punte_spate_pin2 7
 
float punte_spate_viteza = 0.0f;        //valori turatie
float punte_fata_viteza = 0.0f;                    
 
int punte_spate_modifier = 0;    //cat am scazut din PWM pentru a egala turatiile
int punte_fata_modifier = 0;

float xAccl = 0.0f;
float yAccl = 0.0f;          //valori accelerometru
float zAccl = 0.0f;
 
int clamp_int(int input, int min_val, int max_val){ //compar valoarea potentiometrului dintre diferenta rps2 - rps1 
    int result = input < min_val ? min_val : input; //returneaza valoarea intre minim si maxim
    return result > max_val ? max_val : result;
}
 
void setup() {
    Serial.begin(9600);       //setam viteza baud
 
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");    //se initializeaza receiver-ul CAN la viteza de 500 kbps
        while (1);
    }
 
    pinMode(pedala_acceleratie, INPUT);
    pinMode(punte_fata_PWM, OUTPUT);
    pinMode(punte_spate_PWM, OUTPUT);
    pinMode(punte_fata_pin1, OUTPUT);
    pinMode(punte_fata_pin2, OUTPUT);
    pinMode(punte_spate_pin1, OUTPUT);            //definim statutul pinilor
    pinMode(punte_spate_pin2, OUTPUT);
 
    digitalWrite(punte_fata_pin1, HIGH);
    digitalWrite(punte_fata_pin2, LOW);
    digitalWrite(punte_spate_pin1, LOW);          //setam motoarele ca sa se rotească in același sens
    digitalWrite(punte_spate_pin2, HIGH);
 
    CAN.onReceive(onReceivePacket); // inregistram apelul de primire
}
 
void loop() {
    int speed_factor = map(analogRead(pedala_acceleratie), 0, 1023, 0, 255);      //citim potentiometrul si il mapam 0 - 255
 
    if(speed_factor > 10){ //nu poti sa ii dai 0(10 valoare arbitrara)
        analogWrite(punte_fata_PWM, clamp_int(speed_factor+punte_fata_modifier, 0, 255));          
        analogWrite(punte_spate_PWM, clamp_int(speed_factor+punte_spate_modifier, 0, 255));            
    } else {
        //speed_factor = 0;
        //analogWrite(punte_fata_PWM, speed_factor);
        //analogWrite(punte_spate_PWM, speed_factor);
    }
}
 
void onReceivePacket(int packetSize){        
    Serial.println("Received packet!");
 
    if(CAN.packetId() == 0x12){  //Pachet turatie
        int viteza_punte_spate = CAN.read();                   //citim valori turatie
        int viteza_punte_fata = CAN.read();
        int pedala = map(analogRead(pedala_acceleratie), 0, 1023, 0, 255);
        float viteza_robot = 0.204203522; //viteza = raza * 2 * pi * rps  //raza = diametru / 2 = 65 / 2 = 32.5 mm

        punte_spate_viteza = (viteza_punte_spate / 10.f);     
        punte_fata_viteza = (viteza_punte_fata / 10.f);

        if(punte_spate_viteza - punte_fata_viteza > 1.0f){
            //Scadem viteza UNO                                            //logica TCS
            punte_spate_modifier -= 25;
        }else if(punte_fata_viteza - punte_spate_viteza > 1.0f){
            //Scadem viteza NANO
            punte_fata_modifier -= 25;
        } else {
            punte_spate_modifier = 0;
            punte_fata_modifier = 0;
        }

        if(punte_spate_viteza > punte_fata_viteza){
          viteza_robot *= punte_fata_viteza;
        } else if(punte_spate_viteza < punte_fata_viteza){
          viteza_robot *= punte_spate_viteza;
        } else{
          viteza_robot *= punte_spate_viteza;
        }
 
        Serial.println("Potentiometru: " + String(map(pedala, 0, 255, 0, 100)) + "%");
        Serial.println("punte_spate_modifier: " + String(punte_spate_modifier));
        Serial.println("punte_fata_modifier: " + String(punte_fata_modifier));
        Serial.println("punte_spate_viteza: " + String(punte_spate_viteza));              
        Serial.println("punte_fata_viteza: " + String(punte_fata_viteza));   //afisam datele pe serial monitor
        Serial.println("viteza_robot: " + String(viteza_robot) + " m/s");  
     }else if(CAN.packetId() == 0x11){
        // Pachet accelerometru
        unsigned int data[5];
        data[1] =  CAN.read(); 
        data[2] =  CAN.read();
        data[3] =  CAN.read();
        data[4] =  CAN.read();
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
        Serial.println("X       Y");
        Serial.print(xAccl*(-1));             // citim si afisam datele de la accelelometru pe serial monitor
        Serial.print("    ");
        Serial.print(yAccl*(-1));
        Serial.print("\n");
    }
}
