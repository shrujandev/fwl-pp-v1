#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

 
#define SS_PIN 53
#define RST_PIN 5

const int ledPin = 13;
const int ldrPin = A0;
const int laserPin = 7;


int locked = 0;
Servo myservo;

#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
void setup() 
{
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init(); 
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode( laserPin , OUTPUT);
  digitalWrite( laserPin , HIGH);// Initiate MFRC522
  locked = 1;
  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
  Serial.println("Put your card to the reader...");
  Serial.println();

}

/*
Cases

1. RFID successful activation  -> disables RFID, LASER, PRESSURE
2. RFID unsuccessful activation -> alert for RFID
3. LASER block -> alert for LASER
4. LASER noblock -> keep running
5. PRESSURE PLATE weight removed -> alert for PRESSURE PLATE
6. PRESSURE PLATE weight not removed -> keep running

*/


void alertLED () {}
void alertRFID () {
    Serial.println(" Access denied");
    
   
     digitalWrite(ledPin, HIGH);
    delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  Serial.println(" ALARM ACTIVATED ");
  locked =1;
   delay(DENIED_DELAY);
  }
void alertPressure () {}
void rfidSuccess(){
  Serial.println("Authorized access");
    locked = 0;
    digitalWrite( laserPin , LOW);
    digitalWrite(3,HIGH);
    digitalWrite(2,LOW);
    delay(3000);
    digitalWrite(3,LOW);
    digitalWrite(2,HIGH);
    delay(3000);
    digitalWrite( laserPin , HIGH);
    locked = 1;
}




void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  int ldrStatus = analogRead(ldrPin); 
  if (ldrStatus < 150 && locked ==1) {
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  Serial.println(" ALARM ACTIVATED ");
  } 
  else if(ldrStatus < 150 && locked == 0){
    Serial.println("Authorized access");
    digitalWrite(3,HIGH);
    digitalWrite(2,LOW);
    delay(3000);
    digitalWrite(3,LOW);
    digitalWrite(2,HIGH);
    delay(3000);
  }
  else {
    digitalWrite(ledPin, LOW);
    Serial.println("ALARM DEACTIVATED");
  }

  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
   // Analog read of ldr value
  
  
  
  Serial.println( ldrStatus );

  if (content.substring(1) == "54 02 10 23") //change here the UID of the card
  {
    rfidSuccess();
  }
 
 else  alertRFID();
}

void loop() {
  manageRFID()
  manageLED()
  managePRESSURE()
}