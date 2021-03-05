#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

const int led1 = 2;

void setup() {
  pinMode(led1, OUTPUT);
  Serial.begin(9600);
}

void toggleLED(void * parameter){
  for(;;){
    digitalWrite(led1, HIGH);
    
    vTaskDelay(500 / portTICK_PERIOD_MS);

    digitalWrite(led1, LOW);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void printTask(void * parameter){
  Serial.println("Hello!");
  vTaskDelay(500 / portTICK_PERIOD_MS);
}

void loop() {
  xTaskCreate(
    toggleLED,    // Function that should be called
    "Toggle LED",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
  xTaskCreate(
    printTask,
    "print task",
    1000,
    NULL,
    2,
    NULL
  );
}