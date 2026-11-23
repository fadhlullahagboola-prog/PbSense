#include <Wire.h>

#define MCP_ADDR 0x20
#define IODIR 0x00
#define GPIO_REG 0x09

#define SDA_PIN 21
#define SCL_PIN 22
#define SENSE_PIN 34

#define MODE_NORMAL 0x01
#define MODE_LEAD 0x02
#define MODE_OTHER 0x04

#define THRESHOLD_V 1.0
#define SAMPLES 64
#define BBM_MS 50
#define SETTLE_MS 200
#define MODE_TIME_MS 5000

int modes[3] = {MODE_NORMAL, MODE_LEAD, MODE_OTHER};
const char* names[3] = {"normal", "lead check", "other metals"};

void mcpWrite(byte reg, byte val) {
  Wire.beginTransmission(MCP_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void setMode(int m) {
  mcpWrite(GPIO_REG, 0x00);
  delay(BBM_MS);
  mcpWrite(GPIO_REG, m);
  delay(SETTLE_MS);
}

float readVolts() {
  long total = 0;
  for (int i = 0; i < SAMPLES; i++) {
    total += analogReadMilliVolts(SENSE_PIN);
    delay(2);
  }
  return (total / (float)SAMPLES) / 1000.0;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  mcpWrite(IODIR, 0x00);
  mcpWrite(GPIO_REG, 0x00);
  analogSetPinAttenuation(SENSE_PIN, ADC_11db);
  Serial.println("PbSense breadboard demo");
}

void loop() {
  for (int i = 0; i < 3; i++) {
    setMode(modes[i]);
    unsigned long start = millis();
    while (millis() - start < MODE_TIME_MS) {
      float v = readVolts();
      Serial.print(names[i]);
      Serial.print(": ");
      Serial.print(v, 3);
      Serial.print(" V  ");
      if (v > THRESHOLD_V) {
        Serial.println("ABOVE threshold");
      } else {
        Serial.println("below threshold");
      }
      delay(500);
    }
  }
}
