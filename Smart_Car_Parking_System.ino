#define BLYNK_TEMPLATE_ID "TMPL3Yq9MXEie"
#define BLYNK_TEMPLATE_NAME "Smart Parking"
#define BLYNK_AUTH_TOKEN "itj4Nkp5ozTGLydnGMer9w7NfkTPMfhX"

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32Servo.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);


#define pir1 16
#define pir2 15
#define pir3 4


#define pirin 5
#define pirout 17

#define ser_pin_in 19
#define ser_pin_out 18

int slots = 3;

boolean ir1Toggle = false;
boolean ir2Toggle = false;
boolean ir3Toggle = false;

boolean irinToggle = false;
boolean iroutoggle = false;

boolean ir1Avail = false;
boolean ir2Avail = false;
boolean ir3Avail = false;


char ssid[] = "UwU";
char password[] = "123456789";

Servo Servo_in;   // create servo object to control a servo
Servo Servo_out;  // create servo object to control a servo
long count = 0;
BLYNK_CONNECTED() {
  // Change Web Link Button message to "Congratulations!"
  Serial.println("Blynk Connected");
}
void setup() {
  Serial.begin(115200);
  Serial.println("System Starting");
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
  lcd.setCursor(4, 1);
  lcd.print("System");
  // put your setup code here, to run once:
  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);
  pinMode(pir3, INPUT);

  pinMode(pirin, INPUT);
  pinMode(pirout, INPUT);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  WiFi.mode(WIFI_STA);
  delay(1500);
  lcd.clear();
  lcd.print("Waiting for wifi");
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 1);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print("*");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  delay(500);
  Servo_in.setPeriodHertz(50);   // standard 50 hz servo
  Servo_out.setPeriodHertz(50);  // standard 50 hz servo

  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 3);

  Servo_in.attach(ser_pin_in, 500, 2400);    // attaches the servo on pin 9 to the servo object
  Servo_out.attach(ser_pin_out, 500, 2400);  // attaches the servo on pin 9 to the servo object



  Servo_in.write(0);
  Servo_out.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:
  readIr();
  checkSlots();

  Blynk.run();
  count++;
  // Serial.println(".");

  if (count > 1000) {
    lcd.clear();

    lcd.setCursor(0, 0);

    lcd.print("Slot Left:");
    lcd.print(slots);
    lcd.setCursor(0, 1);
    lcd.print(ir2Toggle == true ? "Free" : "Used");
    lcd.setCursor(5, 1);
    lcd.print(ir3Toggle == true ? "Free" : "Used");
    lcd.setCursor(10, 1);
    lcd.print(ir1Toggle == true ? "Free" : "Used");

    count = 0;
  }
  if (slots <= 0) {

    if (iroutoggle == false) {
      Serial.println("Slot Full");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No Slots Free");
      lcd.setCursor(0, 1);
      for (int i = 0; i < 16; i++) {
        lcd.print("*");
        delay(50);
      }
      delay(100);
      return;
    }
  }

  if (slots <= 3 && slots >= 1) {
    if (iroutoggle == false) {
      Servo_out.write(0);
      Serial.println("Open out");
      delay(1000);
    } else {
      Servo_out.write(90);
    }
  }
  //pins age pieche hai
  if (irinToggle == false) {
    Servo_in.write(90);
    Serial.println("Open in");
    delay(1000);
  } else {
    Servo_in.write(0);
  }
}

void checkSlots() {

  if (ir1Toggle == false && ir1Avail == false) {
    ir1Avail = true;
    slots--;
    Blynk.virtualWrite(V0, 1);
    Blynk.virtualWrite(V3, slots);
    Serial.println("Slot Occupied 1");
    delay(1000);
  }
  if (ir2Toggle == false && ir2Avail == false) {
    ir2Avail = true;
    slots--;
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V3, slots);
    Serial.println("Slot Occupied 2");
    delay(1000);
  }
  if (ir3Toggle == false && ir3Avail == false) {
    ir3Avail = true;
    slots--;
    Blynk.virtualWrite(V2, 1);
    Blynk.virtualWrite(V3, slots);
    Serial.println("Slot Occupied 3");
    delay(1000);
  }


  if (ir1Toggle == true && ir1Avail == true) {
    ir1Avail = false;
    slots++;
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V3, slots);
    Serial.println("Slot UnOccupied 1");
    delay(1000);
  }
  if (ir2Toggle == true && ir2Avail == true) {
    ir2Avail = false;
    slots++;
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V3, slots);
    Serial.println("Slot UnOccupied 2");
    delay(1000);
  }
  if (ir3Toggle == true && ir3Avail == true) {
    ir3Avail = false;
    slots++;
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, slots);
    Serial.println("Slot UnOccupied 3");
    delay(1000);
  }
}

void readIr() {

  ir1Toggle = digitalRead(pir1);
  ir2Toggle = digitalRead(pir2);
  ir3Toggle = digitalRead(pir3);
  irinToggle = digitalRead(pirin);
  iroutoggle = digitalRead(pirout);
  // Serial.print("ir 1 ");
  // Serial.print(ir1Toggle);
  // Serial.print(" ir 2 ");
  // Serial.print(ir2Toggle);
  // Serial.print(" ir 3 ");
  // Serial.print(ir3Toggle);
  // Serial.print(" ir in ");
  // Serial.print(irinToggle);
  // Serial.print(" ir out ");
  // Serial.println(iroutoggle);
}