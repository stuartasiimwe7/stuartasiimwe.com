// Untested bring-up scaffold for Adafruit ESP32 Feather V2 + SHT31-D.
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <math.h>

Adafruit_SHT31 sensor;
bool sensorReady = false;
uint32_t lastSample = 0;
const uint32_t samplePeriod = 5000;

void setup() {
  Serial.begin(115200);
  pinMode(NEOPIXEL_I2C_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_I2C_POWER, HIGH);
  delay(100);
  Wire.begin();
  Wire.setClock(100000);
  Wire.setTimeOut(100);
  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("# i2c_address=0x%02X\n", address);
    }
  }
  sensorReady = sensor.begin(0x44);
  Serial.println("uptime_ms,temperature_c,relative_humidity_pct,status");
}

void loop() {
  const uint32_t now = millis();
  if (uint32_t(now - lastSample) < samplePeriod) return;
  lastSample = now;
  if (!sensorReady) sensorReady = sensor.begin(0x44);
  if (!sensorReady) {
    Serial.printf("%lu,,,sensor_missing\n", (unsigned long)now);
    return;
  }
  float temperature = sensor.readTemperature();
  float humidity = sensor.readHumidity();
  if (!isfinite(temperature) || !isfinite(humidity) || humidity < 0 || humidity > 100) {
    Serial.printf("%lu,,,invalid_read\n", (unsigned long)now);
    sensorReady = false;
    return;
  }
  Serial.printf("%lu,%.2f,%.2f,ok\n", (unsigned long)now, temperature, humidity);
}
