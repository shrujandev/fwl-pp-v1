#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>


 
#define SS_PIN 53
#define RST_PIN 5
#define LASER 13 // Pin controlling the laser 
#define LDR A0 // Analog input of the LDR 
#define LED_RED 15 //Manages all the Red LEDs
#define LED_GREEN 16 // Manages all green LEDs
#define LED_WHITE 17 // Manages all White LEDs
#define PRESSURE A1 // Analog input of the Pressure sensor
#define PRESSURE_INIT 200 //Set the initial weight of the object


// Global states
static int locked = 1; //rfid
static int laserState = 1;
static int pressureState = 1;


static void manageLaser();
static void manageLDR();
static void manageLED();
static void manageRFID();
static void alertBREACH();
static void unlockRFID(char * id);
static void lockSystem(char * id);


void setup(void){
    pinMode(LASER,OUTPUT);
    pinMode(LED_GREEN,OUTPUT);
    pinMode(LED_RED,OUTPUT);
    pinMode(LED_WHITE,OUTPUT);
    pinMode(LDR,INPUT);
    pinMode(PRESSURE,INPUT);
    Serial.begin(9600);
    SPI.begin();
    mrfc522.PCD_Init();
}

void loop(void){
    manageRFID();
    manageLDR();
    manageLaser();
    managePRESSURE();
    manageLED();
}



void manageLED(){ //Controls all LEDS during all states
    if(locked){
        digitalWrite(LED_GREEN,LOW);
        digitalWrite(LED_WHITE,LOW);
        digitalWrite(LED_RED,HIGH);
    }
    else{
        digitalWrite(LED_GREEN,LOW);
        digitalWrite(LED_WHITE,HIGH);
        digitalWrite(LED_RED,LOW);
    }

}

void manageLDR(){   //analog read the LDR data and send laser breach data 

    int ldrStatus = analogRead(LDR); 
    if (ldrStatus < 150 && locked ==1) {
        Serial.println(" ALARM ACTIVATED ");
        laserState = 0;
        alertBREACH();
    } 
    else if(ldrStatus < 150 && locked == 0){
        Serial.println("Authorized access");
        laserState = 1;
    }
    else {
        Serial.println("ALARM DEACTIVATED");
    }

}

void manageRFID(){  // Waits for card read constantly and send unlock data 
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())   
    {
        return;
    }

    // GET the id from Card reader
    String id= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
        id.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ")); // copies the vlaue read into id as a hex formatt
        id.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    id.toUpperCase();

    // if system is already locked, unlock it
    if (locked) {
        unlockRFID(id);
    }
    // if system is unlocked, lock it
    else {
        lockSystem(id);
    }
}

void managePRESSURE(){  // serial read pressure plate and alert breach if any
   int pressureRead = analogRead(PRESSURE);
   if(pressureRead < PRESSURE_INIT && locked == 1){
       pressureState = 0;
       alertBREACH();
   }
   else if(pressureRead >= PRESSURE_INIT && locked ==0){
       Serial.println("Safe to remove the Object")
   }

}

void alertBREACH(){ //sends data to node MCU.
        // This part of the code is the data sending to the node mcu
        //SendData(laserState,PressureState,locked)
}

void unlockRFID(char * id){ // just verifies if the card is valid or not
    if(id.substring(1) == "54 02 10 23")
        locked  = 1;
    else 
        locked  = 0;
}

void lockSystem(char * id){ // just verifies if the card is valid or not
    if(id.substring(1) == "54 02 10 23")
        locked  = 0;
    else 
        locked  = 1;

    digitalWrite(LASER,HIGH);
    digitalWrite(PRESSURE,HIGH);
    
}

void manageLaser(){
    if(locked){
        digitalWrite(LASER,HIGH);
    }
    else
        digitalWrite(LASER,LOW);
}
