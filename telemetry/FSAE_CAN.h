/* 
  ******************** CAN  ********************

  - Please use this for all TWAI/CAN variables and functions

 */

/* --------------------------- Imports --------------------------- */

  #include "driver/twai.h"          // API HERE --> [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html]


/* --------------------------- Variables --------------------------- */


  #define TX_GPIO_NUM     D6          // "TX2" on the ESP32-WROOM-32
  #define RX_GPIO_NUM     D5          // "RX2" on the ESP32-WROOM-32

  #define TELEMETRY_INPUT_ID    0x661          // From the C185 or steering wheel
  #define TELEMETRY_OUTPUT_ID   0x654          // From the C185 or steering wheel

  // #define CAN_TX_TASK_PRIO      6         // TX Task Priority 
  #define CAN_RX_TASK_PRIO      8         // RX Task Priority 

  // static const twai_filter_config_t   f_config =  {
  //                                                   .acceptance_code  = (TELEMETRY_INPUT_ID << 21),
  //                                                   .acceptance_mask  = ~(TWAI_STD_ID_MASK << 21),
  //                                                   .single_filter    = true
  //                                                 };
  static const twai_filter_config_t   f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  static const twai_timing_config_t   t_config = TWAI_TIMING_CONFIG_1MBITS();     // CAN SPEED 
  static const twai_general_config_t  g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TX_GPIO_NUM, (gpio_num_t)RX_GPIO_NUM, TWAI_MODE_NO_ACK);

  twai_message_t message;

  twai_message_t transmit_msg = {
                                  .identifier = TELEMETRY_OUTPUT_ID, 
                                  .data_length_code = 8,
                                  .data = {
                                    1,          // ONLINE BIT
                                    0,          // DRS CONTROL BIT
                                    0,0,0,0,0,0 // Unusedr
                                  } 
                                };

  int k = 0;

  static SemaphoreHandle_t can_rx_sem;
  static SemaphoreHandle_t can_tx_sem;


/* --------------------------- Tasks --------------------------- */


void setup_canbus(){
    // Install and start TWAI driver            -   This will force the ESP32 to restart if there is a CAN error - good
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(BASE_TAG, "CAN Driver installed");
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(BASE_TAG, "CAN Driver started");
    Serial.print("TWAI started\n");
}


// // CAN/TWAI Transmit Task
// static void twai_transmit_task(void *arg){
//     xSemaphoreTake(can_tx_sem, portMAX_DELAY);
//     ESP_LOGI(BASE_TAG, "Transmit Thread Started");

//     while(1){
//       digitalWrite(LED_BUILTIN, HIGH);

//       ESP_ERROR_CHECK(twai_transmit(&transmit_msg , pdMS_TO_TICKS(5)));  

//       digitalWrite(LED_BUILTIN, LOW);
//       vTaskDelay(pdMS_TO_TICKS(10));    // Send updates @ 100 Hz
//     }

//     xSemaphoreGive(can_tx_sem);
//     vTaskDelete(NULL);
// }

// CAN/TWAI Recieve Task
static void twai_receive_task(void *arg){

    xSemaphoreTake(can_rx_sem, portMAX_DELAY);
    ESP_LOGI(BASE_TAG, "Receive Task Started");

    while(1){

        if (twai_receive(&message, pdMS_TO_TICKS(5)) == ESP_OK);
        else {
            printf(".\n");
            vTaskDelay(1);
            continue;
        }

        // Print received message
        if (Serial){
          printf("\nID: 0x%.03x\t", message.identifier);
          if (!(message.rtr)) {
              printf("Data (%d) (hex): ", message.data_length_code);
              for (int i = 0; i < message.data_length_code; i++) {
                  printf("0x%.02x ", message.data[i]);
              }
          }
        }

        // Process received message
        // if(message.identifier == TELEMETRY_INPUT_ID){
        //   data_out.identifier = TELEMETRY_INPUT_ID;
        //   data_out.data_length_code = message.data_length_code;
        //   for(k = 0; k< message.data_length_code; k++){
        //     data_out.data[k] = message.data[k];
        //   }
        // }
        
        data_out.identifier = message.identifier;
        data_out.data_length_code = message.data_length_code;
        for(k = 0; k< message.data_length_code; k++){
          data_out.data[k] = message.data[k];
        }
        
        vTaskDelay(1); 
    }

}