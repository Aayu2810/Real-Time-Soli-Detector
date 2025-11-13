#define BLYNK_TEMPLATE_ID           "..."
#define BLYNK_TEMPLATE_NAME         "..."
#define BLYNK_AUTH_TOKEN            "..."
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char ssid[] = "Aayushi Oppo";
char pass[] = "anayanusha";

// Blynk Auth Token
char auth[] = "qRfJZKOWlBpCwvcjOYjcs06aV7-XwGdg";

// Pins
#define SOIL_PIN 34   // VP
#define PH_PIN 35     // VN
#define RELAY_PIN 27
#define BUZZER_PIN 26
#define GREEN_LED 25
#define RED_LED 33

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables
int soilValue;
float moisturePercent;
int phRaw;
float phValue;
bool pumpStatus = false;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);

  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("AGRI PROJECT");
  delay(3000);
  lcd.clear();
}

void loop() {
  Blynk.run();

  // Read sensors
  soilValue = analogRead(SOIL_PIN);
  moisturePercent = map(soilValue, 4095, 0, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  phRaw = analogRead(PH_PIN);
  phValue = map(phRaw, 0, 6955, 0, 140) / 11.0;  // Adjust if needed

  // Check and control pump
  if (moisturePercent < 30) {
    digitalWrite(RELAY_PIN, HIGH);  // Pump ON
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    pumpStatus = true;
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(RELAY_PIN, LOW);   // Pump OFF
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    pumpStatus = false;
  }

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Moist:");
  lcd.print((int)moisturePercent);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  lcd.print("pH:");
  lcd.print(phValue, 1);
  lcd.print(" ");

  lcd.setCursor(8, 1);
  if (pumpStatus)
    lcd.print("Pump:ON ");
  else
    lcd.print("Pump:OFF");

  // Send to Blynk
  Blynk.virtualWrite(V0, (int)moisturePercent);
  Blynk.virtualWrite(V1, phValue);
  Blynk.virtualWrite(V2, pumpStatus ? "Pump ON" : "Pump OFF");

  delay(500);
}
