#include <Arduino.h>
#include <EEPROM.h>

#define MEM_ADDR 0x00

#define a 2
#define b 3
#define c 4
#define d 5
#define e 6
#define f 8
#define g 7

#define unita       A2
#define decine      A1
#define centinaia   A0

#define pwmout 11

#define pinA 9
#define pinB 10
int pinALast;
int counter = 0; //Min of 0 and Max of 19 due to one full encoder turn
unsigned long currentTime;
unsigned long lasteepromwrite = 0;
unsigned long loopTime;
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;

volatile uint8_t numero = 0;
volatile uint8_t old_numero = 0;

static const uint8_t digits[10][7] = {
  //A     B     C     D     E     F     G
  {LOW,   LOW,  LOW,  LOW,  LOW,  LOW,  HIGH},  //0
  {HIGH,  LOW,  LOW,  HIGH, HIGH, HIGH, HIGH},  //1
  {LOW,   LOW,  HIGH, LOW,  LOW,  HIGH, LOW},   //2
  {LOW,   LOW,  LOW,  LOW,  HIGH, HIGH, LOW},   //3
  {HIGH,  LOW,  LOW,  HIGH, HIGH, LOW,  LOW},   //4
  {LOW,   HIGH, LOW,  LOW,  HIGH, LOW,  LOW},   //5
  {LOW,   HIGH, LOW,  LOW,  LOW,  LOW,  LOW},   //6
  {LOW,   LOW,  LOW,  HIGH, HIGH, HIGH, HIGH},  //7
  {LOW,   LOW,  LOW,  LOW,  LOW,  LOW,  LOW},   //8
  {LOW,   LOW,  LOW,  LOW,  HIGH,  LOW,  LOW}   //9
};

void writeIntIntoEEPROM(int address, uint8_t number){ 
  EEPROM.write(address, number);
}
uint8_t readIntFromEEPROM(int address){
  byte byte1 = EEPROM.read(address);
  return byte1;
}

void setup() {
  numero = readIntFromEEPROM(MEM_ADDR);
  if(numero == "0xFF" || numero > 0x64){
    numero = 0;
  }
  old_numero = numero;
  // put your setup code here, to run once:
  pinMode(unita, OUTPUT);
  pinMode(decine, OUTPUT);
  pinMode(centinaia, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);

  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);

  digitalWrite(unita, HIGH);
  digitalWrite(decine, HIGH);
  digitalWrite(centinaia, HIGH);

  pinMode (pinA,INPUT);
  pinMode (pinB,INPUT);
  pinALast = digitalRead(pinA);
  currentTime = micros();
  loopTime = currentTime; 
  lasteepromwrite = millis();

  pinMode(pwmout, OUTPUT);
}


void writeNumber(uint8_t number){
  digitalWrite(a, digits[number][0]);
  digitalWrite(b, digits[number][1]);
  digitalWrite(c, digits[number][2]);
  digitalWrite(d, digits[number][3]);
  digitalWrite(e, digits[number][4]);
  digitalWrite(f, digits[number][5]);
  digitalWrite(g, digits[number][6]);
}


void loop() {
  // put your main code here, to run repeatedly:
  uint8_t cents = numero / 100 % 10;
  uint8_t decin = numero / 10 % 10;
  uint8_t uints = numero % 10;

  writeNumber(cents);
  digitalWrite(centinaia, LOW);
  delay(2);
  digitalWrite(centinaia, HIGH);
  
  writeNumber(decin);
  digitalWrite(decine, LOW);
  delay(2);
  digitalWrite(decine, HIGH);
  
  writeNumber(uints);
  digitalWrite(unita, LOW);
  delay(2);
  digitalWrite(unita, HIGH);

  currentTime = micros();
  if (currentTime >= (loopTime + 2)) {
    encoder_A = digitalRead(pinA); // Read encoder pins
    encoder_B = digitalRead(pinB);
    if ((!encoder_A) && (encoder_A_prev)) {
      // A has gone from high to low
      if (encoder_B) {
        // B is high so counter-clockwise
        if (numero <= 0) {
          //numero = 20;
        } else { numero--; }
      } else {
        // B is low so clockwise
        if (numero >= 100) {
          //numero = 0;
        } else { numero++; }
      }
    }
    encoder_A_prev = encoder_A; // Store value of A for next time
    loopTime = currentTime; // Updates loopTime
  }

  int pwmvalue = map(numero, 0, 100, 0, 254);
  analogWrite(pwmout, pwmvalue);

  if(millis() - lasteepromwrite > 5000){
    if(old_numero != numero){
      writeIntIntoEEPROM(MEM_ADDR, numero);
      old_numero = numero;
    }
    lasteepromwrite = millis();
  }
}
