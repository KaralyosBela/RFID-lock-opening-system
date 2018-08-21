#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
MFRC522 RFIDMODUL(10, 9);   
Servo szervomotor;
const int trigPin = 7, echoPin = 6;
const int RED = 3, GREEN = 4, buzzer = 5;
int pozicio = 0, id = 0, distance;
boolean nyitvaerzekelo = false, nyitva = false;
boolean ujkartya = false, x = false, error = false;
String hexakod = "";
long duration;

char kartyak[3][100] =
{ "C3 6F 02 A3",
  "D0 15 E3 B9",
};

char nevek[3][100] =
{ "Karalyos Béla",
  "Other user",
};

char benn[4] = "000";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  RFIDMODUL.PCD_Init();
  szervomotor.attach(A2);
  pinMode(buzzer, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {



  if (RFIDMODUL.PICC_IsNewCardPresent())
  {ujkartya = true;}
  if (ujkartya)
  {RFIDMODUL.PICC_ReadCardSerial();}

  if (ujkartya) {
    for (byte i = 0; i < RFIDMODUL.uid.size; i++) 
    {
      if (RFIDMODUL.uid.uidByte[i] > 0x10) 
      {
        if (x) {
          hexakod.concat(' ');
        }
        x = true;
      }
      else
      {
        hexakod.concat(" 0");
      }
      hexakod.concat(String(RFIDMODUL.uid.uidByte[i], HEX));
    }
  }

  hexakod.toUpperCase();
  for (int j = 0; j < 2; j++)
  {
    if (hexakod == kartyak[j])
    {
      pozicio = j;
      if (benn[pozicio] == '0')
      {
        benn[pozicio] = '1';
      }
      else
      {
        benn[pozicio] = '0';
      }
      nyitva = true;
    }
  }

  if (ujkartya)
    if (!nyitva)
      error = true;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  if (distance < 10)
  {
    nyitvaerzekelo = true;
  }

  if (nyitva)
  {

    Serial.println("----------Sikeres belepes!---------");
    if (benn[pozicio] == '1')
    {
      Serial.print("Üdvözlünk, ");
      benn[pozicio] = '1';
    }
    else
    {
      Serial.print("Viszlát, ");
      benn[pozicio] = '0';
    }
    szervomotor.write(180);
    Serial.print(nevek[pozicio]);
    Serial.print(" (");
    Serial.print(kartyak[pozicio]);
    Serial.println(")");
    Serial.println(benn);

    digitalWrite(GREEN, HIGH);
    tone(buzzer, 2000);
    delay(500);
    digitalWrite(GREEN, LOW);
    noTone(buzzer);
    delay(1000);
    szervomotor.write(90);
    nyitva = false;
  }

  if (nyitvaerzekelo)
  {
    Serial.println("Sikeres kilepes!");
    szervomotor.write(180);
    digitalWrite(GREEN, HIGH);
    tone(buzzer, 2000);
    delay(2000);
    digitalWrite(GREEN, LOW);
    noTone(buzzer);
    szervomotor.write(90);
    nyitvaerzekelo = false;
  }

  if (error)
  {
    Serial.println("Sikertelen belepes!");
    digitalWrite(RED, HIGH);
    tone(buzzer, 2000);
    delay(250);
    digitalWrite(RED, LOW);
    noTone(buzzer);
    delay(250);
    digitalWrite(RED, HIGH);
    tone(buzzer, 1000);
    delay(250);
    digitalWrite(RED, LOW);
    noTone(buzzer);
    error = false;
  }

  hexakod = ""; ujkartya = false; x = false;
  Serial.println(distance);

  delay(500);
}

