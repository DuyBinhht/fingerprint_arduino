#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
SoftwareSerial fingerPrint(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);
uint8_t id;
#include <Servo.h>     // THU VIEN DONG CO
Servo myservo; 
#define enroll A0
#define del A1
#define up A2
#define down A3
int out = 4;  //chan noi buzzer
int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

int cambien = A6;
int gtcambien;
float vol_out;
float vol_in;

const byte Seg[11] = {
  0b11000000,//0 - các thanh từ a-f sáng
  0b11111001,//1 - chỉ có 2 thanh b,c sáng
  0b10100100,//2
  0b10110000,//3
  0b10011001,//4
  0b10010010,//5
  0b10000010,//6
  0b11111000,//7
  0b10000000,//8
  0b10010000,//9
};

void HienThiLED7doan(unsigned long Giatri, byte SoLed = 2) {
  
  byte *array= new byte[SoLed];
  for (byte i = 0; i < SoLed; i++) {
    //Lấy các chữ số từ phải quá trái
    array[i] = (byte)(Giatri % 10UL);
    Giatri = (unsigned long)(Giatri /10UL);
  }
  digitalWrite(latchPin, LOW);
  for (int i = SoLed - 1; i >= 0; i--)
    shiftOut(dataPin, clockPin, MSBFIRST, Seg[array[i]]); 
  
  digitalWrite(latchPin, HIGH);
  free(array);
}

void setup() {
  // put your setup code here, to run once:
  lcd.init();                    
  lcd.backlight();
  Serial.begin(9600);
  
  myservo.attach(9);
  myservo.write(180);

  pinMode(enroll, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP); 
  pinMode(down, INPUT_PULLUP); 
  pinMode(del, INPUT_PULLUP); 

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  //khai bao buzzer
  pinMode(out, OUTPUT); 

  lcd.init();                    
  lcd.backlight();
  lcd.print("HUA DUY BINH");
  lcd.setCursor(0,1);
  lcd.print("21161286");
  delay(2000);
  finger.begin(57600);
  Serial.begin(9600);
  lcd.clear();
  lcd.print("Finding Module");
  lcd.setCursor(0,1);
  //chan analog đọc điện áp Battery
  pinMode(cambien, INPUT);
  delay(1000);
  if (finger.verifyPassword()) 
  {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.print("Found Module ");
    delay(1000);
  } 
  else 
  {
    lcd.clear();
    lcd.print("Module not Found");
    lcd.setCursor(0,1);
    lcd.print("Check Connections");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,0);
  battery();
       lcd.print("Press UP/Down ");
       lcd.setCursor(0,1);
       lcd.print("to start System");
       if(digitalRead(up)==0 || digitalRead(down)==0)
       {
        for(int i=0;i<5;i++)
        {
          lcd.clear();
          lcd.print("Place Finger");
          delay(2000);
          int result=getFingerprintIDez();
          if(result>=0)
          {
            winsound();
            myservo.write(80); // door open
            lcd.clear();
            lcd.print("Allowed");
            lcd.setCursor(0,1);
            lcd.print("Gate Opened   ");
            delay(5000);
            myservo.write(180); //dong cua
            lcd.setCursor(0,1);
            lcd.print("Gate Closed   ");
            return;
           }
         }
       }
       checkKeys();
       delay(1000);          
}

void battery() 
{
  gtcambien = analogRead(cambien);
  // Serial.print("Cảm biến: "); Serial.print(gtcambien); Serial.print("   ");
  
  vol_out = (gtcambien * 5) / 1024.0; // Chuyển từ analog sang V; 0-1023 ~ 0-5v
  // Serial.print("Volt out: "); Serial.print(vol_out); Serial.print("   ");
  
  vol_in = float(vol_out / float(390.0/float(3100.0+390.0))); // Tính vol nguồn cần đo
  // Serial.print("Volt nguồn: "); Serial.println(vol_in);
  /*
   * Thevenin Norton
   * 
   * Vth = (Vcc x R2) / R1 + R2
   * Vout = (Vin x R2) / R1 + R2 => Vin = Vout / (R2/(R1+R2));
   */
  int percent = vol_in/11.1*100;
  HienThiLED7doan(percent, 2);
  // Serial.print("percent_BAT: ");  Serial.println(percent_BAT);
  delay(2000);
}

void checkKeys()
{
   if(digitalRead(enroll) == 0)
   {
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    while(digitalRead(enroll) == 0);
    Enroll();
   }

   else if(digitalRead(del) == 0)
   {
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    delet();
   }  
}

void Enroll()
{
   int count=0;
   lcd.clear();
   lcd.print("Enroll Finger    ");
   lcd.setCursor(0,1);
   lcd.print("Location:");
   while(1)
   {
     lcd.setCursor(9,1);
     lcd.print(count);
     lcd.print("  ");
     HienThiLED7doan(count, 2);
     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }

     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
     else if(digitalRead(del) == 0)
     {
          id=count;
          getFingerprintEnroll();
          return;
     }

       else if(digitalRead(enroll) == 0)
     {        
          return;
     }
 }
}

void delet()
{
   int count=0;
   lcd.clear();
   lcd.print("Delete Finger    ");
   lcd.setCursor(0,1);
   lcd.print("Location:");
   while(1)
   {
     lcd.setCursor(9,1);
     lcd.print(count);
     lcd.print("  ");
     HienThiLED7doan(count, 2);
     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }

     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
     else if(digitalRead(del) == 0)
     {
          id=count;
          deleteFingerprint(id);
          return;
     }

       else if(digitalRead(enroll) == 0)
     {        
          return;
     }
 }
}

uint8_t getFingerprintEnroll() 
{
  int p = -1;
  lcd.clear();
  lcd.print("finger ID:");
  lcd.print(id);
  lcd.setCursor(0,1);
  lcd.print("Place Finger");
  delay(2000);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      lcd.clear();
      lcd.print("No Finger");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Comm Error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcd.clear();
      lcd.print("Imaging Error");
      break;
    default:
       lcd.clear();
      lcd.print("Unknown Error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
       lcd.clear();
       lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Comm Error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    default:
      lcd.clear();
      lcd.print("Unknown Error");
      return p;
  }
  
  lcd.clear();
  lcd.print("Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
   lcd.clear();
      lcd.print("Place Finger");
      lcd.setCursor(0,1);
      lcd.print("   Again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  // OK success!

  p = finger.image2Tz(2);  
  p = finger.createModel();   
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Stored!");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    return p;
  }
  else {
    return p;
  }   
}

int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  
  if (p != FINGERPRINT_OK)  
  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  
  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    failsound();
    lcd.clear();
    lcd.print("Finger Not Found");
    lcd.setCursor(0,1);
    lcd.print("Try Later");
    delay(2000);  
    return -1;
  }
  return finger.fingerID; 
}

uint8_t deleteFingerprint(uint8_t id) 
{
  uint8_t p = -1;  
  lcd.clear();
  lcd.print("Please wait");
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) 
  {
    lcd.clear();
    lcd.print("Figer Deleted");
    lcd.setCursor(0,1);
    lcd.print("Successfully");
    delay(1000);
  } 
  
  else 
  { 
    lcd.clear();
    lcd.print("Something Wrong");
    lcd.setCursor(0,1);
    lcd.print("Try Again Later");
    delay(2000);
    return p;
  }   
}

void winsound()
{       
  tone(out, 3000 );  
  delay (100);
  noTone(out); 
  delay(30); 
  tone(out, 4000);  
  delay (100);
  noTone(out); 
  delay(30); 
  tone(out, 5000);  
  delay (150);
  noTone(out); 
  delay(30); 
}

void failsound()
{
  unsigned char f ;      
  for (f = 0; f < 3; f++)
  {             
    tone(out, 1000);  
    delay (100);
    noTone(out); 
    delay(30);          
  }
}
