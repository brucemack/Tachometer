/**
 * Copyright (C) 2024, Bruce MacKinnon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * NOT FOR COMMERCIAL USE WITHOUT PERMISSION.
 */
#include <fix_fft.h>

#define FFT_SIZE 64
#define FFT_SIZE_BITS 6

unsigned long samplesPerSecond = 256;
unsigned long microsBetweenSamples = 1000000UL / samplesPerSecond;
unsigned long lastSample = micros();

unsigned long lastCycle = 0;
unsigned long msBetweenCycles = 500;

int samplePtr = 0;
char dataR[FFT_SIZE];
char dataI[FFT_SIZE];

void setup() {

  Serial.begin(9600);
  delay(100);
  Serial.println("Tachometer");

  /*
  float step = 2.0 * 3.14159 / 64.0;
  for (int i = 0; i < 64; i++) {
    float rad = step * (float)i;    
    // Sample centered around 512 (0->1023)
    float sample = 512.0  + cos(rad * 63.0) * (float)511.0;
    // Shift down so that we are centerd around zero (-127->127)
    dataR[i] = ((sample - 512.0) / 512.0) * 128.0;
    dataI[i] = 0;
    Serial.print(i);
    Serial.print(" ");
    Serial.println((int)dataR[i]);
  }
  Serial.println("FFT");
  fix_fft(dataR,dataI,6,0);
  for (int i = 0; i < 32; i++) {
    float real = dataR[i];
    float imag = dataI[i];
    float mag = sqrt(real * real + imag * imag);
    Serial.print(i);
    Serial.print(" ");
    Serial.println(mag);
  }
  */
}

int mode = 0;

void loop() {

  if (mode == 0) {
    if (millis() - lastCycle > msBetweenCycles) {
      lastCycle = millis();
      mode = 1;
      samplePtr = 0;
    }
  } 
  else if (mode == 1) { 
    unsigned long now = micros();
    if (now - lastSample > microsBetweenSamples) {
      lastSample = now;
      // Sample centered around 512 (0->1023)
      float sample = analogRead(0);
      // Shift down so that we are centerd around zero (-127->127)
      dataR[samplePtr] = ((sample - 512.0) / 512.0) * 127.0;
      dataI[samplePtr] = 0;
      // Advance the counter
      samplePtr++;
      // Full window?
      if (samplePtr == FFT_SIZE) {
        fix_fft(dataR,dataI,FFT_SIZE_BITS,0);
        // Find the biggest bucket
        float maxMag = 0;
        float maxMagBucket = 0;
        for (int i = 0; i < FFT_SIZE / 2; i++) {
          float real = dataR[i];
          float imag = dataI[i];
          float mag = sqrt(real * real + imag * imag);
          if (mag > maxMag) {
            maxMag = mag;
            maxMagBucket = i;
          }
        }
        Serial.print(maxMagBucket);
        Serial.print(" ");
        Serial.println(maxMag);
        mode = 0; 
      }
    }
  }
}

