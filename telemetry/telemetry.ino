/* --------------------- Imports ------------------ */
  // External
    #include <stdio.h>                // Yummy OS Stuff
    #include <stdlib.h>               // Alot of helpful yummy libs
    #include "freertos/FreeRTOS.h"    // Task Management
    #include "freertos/task.h"        // Task Management
    #include "freertos/queue.h"       // Task Management 
    #include "freertos/semphr.h"      // Task Management
    #include "esp_err.h"              // ESP32 Errors     --  used for debugging
    #include "esp_log.h"              // ESP32 Logging    --  used for debugging
  
  // NODE SELECTION - CHANGE BEFORE UPLOADING
    #define CAR_NODE    /* Select Between CAR_NODE and BASE_NODE */
  //
    
  // Custom Imports
    #include "constants.h"
    #include "RADIO.h"
    #include "FSAE_CAN.h"


void app_main(void){

    // Create Semaphore(s)
    can_rx_sem = xSemaphoreCreateBinary();  
    can_tx_sem = xSemaphoreCreateBinary(); 
    radio_sem = xSemaphoreCreateBinary();  

    // CREATE THREADS (TASKS)
  
    #ifdef CAR_NODE           
      xTaskCreatePinnedToCore(twai_receive_task,  "TWAI_rx",    4096, NULL, CAN_RX_TASK_PRIO,   NULL, tskNO_AFFINITY);
      xTaskCreatePinnedToCore(radio_transmit,   "rf_tx",        4096, NULL, RADIO_TX_TASK_PRIO, NULL, tskNO_AFFINITY); Serial.println("TRANSMIT TASK GENERATED");
    #else                     
      xTaskCreatePinnedToCore(radio_receive,   "rf_rx",        4096, NULL, RADIO_RX_TASK_PRIO,     NULL, tskNO_AFFINITY);
      Serial.println("RECEIVE TASK GENERATED");
    #endif

    xSemaphoreGive(radio_sem);                     // Allow Start of any Radio Task
    xSemaphoreGive(can_rx_sem);                     // Allow start of rf RX task   
    xSemaphoreGive(can_tx_sem);                     // Allow Start of rf TX task


    vTaskDelay(pdMS_TO_TICKS(100));             

}

void setup() {
  Serial.begin(115200);
  setup_radio(); 
  #ifdef CAR_NODE 
    setup_canbus();
  #endif 

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  app_main();
  vTaskDelete(NULL);
}

void loop() {
  // vTaskDelay(portMAX_DELAY); // Never wanna be in the main loop :)
  vTaskDelete(NULL);
}
