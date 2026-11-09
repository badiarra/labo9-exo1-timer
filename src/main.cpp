#include <Arduino.h>

#include "hcsr04/HC_SR04.h"
#include "led/Led.h"

// Definition des broches

const uint8_t LED_RED_PIN = 41;
const uint8_t LED_GREEN_PIN = 40;

const uint8_t ECHO_PIN = 7;
const uint8_t TRIG_PIN = 8;

// Les Handlers

TaskHandle_t sensorHandle = nullptr;
TaskHandle_t ledGreenHandle = nullptr;
TaskHandle_t ledHandle = nullptr;

TimerHandle_t timerHandle = nullptr;

SemaphoreHandle_t sensorSemaphore = nullptr;

SemaphoreHandle_t distanceMutex = nullptr;

SemaphoreHandle_t serialMutex = nullptr;

// Les instanciations

Led ledGreen(LED_GREEN_PIN, "green");
Led ledRed(LED_RED_PIN, "red");

HC_SR04 sensor(TRIG_PIN, ECHO_PIN);

// Variable partagée

float sensorDistance;

void timerCallback(TimerHandle_t timer) { xSemaphoreGive(sensorSemaphore); }

void vTaskSensor(void *ptr) {
  float distance = -999;

  while (true) {
    if (xSemaphoreTake(sensorSemaphore, portMAX_DELAY)) {
      distance = sensor.getDistance();

      xSemaphoreTake(distanceMutex, portMAX_DELAY);
      sensorDistance = distance;
      xSemaphoreGive(distanceMutex);

      xSemaphoreTake(serialMutex, portMAX_DELAY);
      Serial.print("Distance : ");
      Serial.print(distance);
      Serial.println(" cm");
      xSemaphoreGive(serialMutex);
    }
  }
}

void vTaskLed(void *ptr) {
  float distance;
  bool ledGreenOn = false;

  while (true) {
    xSemaphoreTake(distanceMutex, portMAX_DELAY);
    distance = sensorDistance;
    xSemaphoreGive(distanceMutex);

    if (distance >= 0 && distance < 20) {
      ledGreen.off();
      ledRed.on();
      ledGreenOn = false;

    } else {
      ledGreen.on();
      ledRed.off();
      ledGreenOn = true;
    }

    xSemaphoreTake(serialMutex, portMAX_DELAY);
    Serial.println(ledGreenOn ? "Led verte en marche" : "Led rouge en marche");
    xSemaphoreGive(serialMutex);

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  // Les initialisations

  Serial.begin(115200);
  delay(1000);

  ledGreen.init();
  ledRed.init();
  sensor.init();

  distanceMutex = xSemaphoreCreateMutex();
  serialMutex = xSemaphoreCreateMutex();
  sensorSemaphore = xSemaphoreCreateBinary();

  if (distanceMutex == nullptr || serialMutex == nullptr || sensorSemaphore == nullptr) {
    Serial.println("Erreur creation synchronisation");
    while (1);
  }

  timerHandle = xTimerCreate("Timer", pdMS_TO_TICKS(500), pdTRUE, nullptr, timerCallback);

  xTaskCreate(vTaskSensor, "Mesure", 1024, nullptr, 2, &sensorHandle);

  xTaskCreate(vTaskLed, "DEL", 1024, nullptr, 2, &ledHandle);

  xTimerStart(timerHandle, 0);
}

void loop() {}
