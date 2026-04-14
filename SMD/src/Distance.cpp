#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "Distance.h"

VL53L0X capteurs[NBR_CAPTEURS];

int uiAdd[NBR_CAPTEURS] = {0x31, 0x32};
int pins[NBR_CAPTEURS]  = {14, 32};

int distances[NBR_CAPTEURS];

void scanI2C()
{
  Serial.println("Scan I2C...");

  for (byte address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);

    if (Wire.endTransmission() == 0)
    {
      Serial.print("Trouve: 0x");
      Serial.println(address, HEX);
    }
  }
}

void InitCapteur()
{
  Wire.begin();

  for (int i = 0; i < NBR_CAPTEURS; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  delay(100);

  for (int i = 0; i < NBR_CAPTEURS; i++)
  {
    digitalWrite(pins[i], HIGH);
    delay(100);

    capteurs[i].init();
    capteurs[i].setAddress(uiAdd[i]);
    capteurs[i].setTimeout(500);
  }

  delay(100);
  scanI2C();
  delay(100);

  Serial.println("Sensor OK");
}

void LireDistances()
{
  for (int i = 0; i < NBR_CAPTEURS; i++)
  {
    distances[i] = capteurs[i].readRangeSingleMillimeters();

    if (capteurs[i].timeoutOccurred())
    {
      distances[i] = 9999;
    }
  }
}

int IndexDistanceMin()
{
  int indexMin = 0;

  for (int i = 1; i < NBR_CAPTEURS; i++)
  {
    if (distances[i] < distances[indexMin])
    {
      indexMin = i;
    }
  }

  return indexMin;
}