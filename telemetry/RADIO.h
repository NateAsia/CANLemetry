/* 
  ******************** RADIO  ********************

  - Please use this for all RADIO variables and functions

 */

/* --------------------------- Imports --------------------------- */
  
  #include <SPI.h>
  #include <RF24.h>

/* --------------------------- Variables --------------------------- */

  typedef struct {
    uint32_t identifier;
    uint8_t data_length_code;
    uint8_t data[8];
  } CAN_RF_PACKET;

  #define RADIO_TX_TASK_PRIO    9         // Task Priority
  #define RADIO_RX_TASK_PRIO    9         // Task Priority


  #ifdef CAR_NODE
    // Define the SPI pins for the NRF24L01 module
    #define CE_PIN    D8
    #define CSN_PIN   D1

    #define SPI_MISO  D10
    #define SPI_MOSI  D0
    #define SPI_SCK   D9
    #define SPI_SS    -1

    #define RADIO_NUMBER 0

  #elif defined(BASE_NODE) 
        // Define the SPI pins for the NRF24L01 module
    #define CE_PIN    D3
    #define CSN_PIN   D1

    #define SPI_MOSI  D2
    #define SPI_MISO  D5
    #define SPI_SCK   D4
    #define SPI_SS    -1

    #define RADIO_NUMBER 1
  #endif
  

  // Define the address for NRF24L01 module
  uint8_t address[][6] = { "CNode", "BNode" };

  // Define the radio object for NRF24L01
  RF24 radio(CE_PIN, CSN_PIN);

  CAN_RF_PACKET data_out = {
    .identifier = 0x64F,
    .data_length_code = 8,
    .data = {
      1,2,3,4,5,6,7,8
    }
  };

  CAN_RF_PACKET data_in = {
    .identifier = 0,
    .data_length_code = 8,
    .data = {
      0,0,0,0,0,0,0,0
    }
  };

  uint8_t counter = 0;

  int i=0;
  uint8_t bytes;
  
  static SemaphoreHandle_t radio_sem;

/* --------------------------- Functions --------------------------- */


void setup_radio(){
  
  // Start the NRF24L01 module 

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SS);
  Serial.println("\nSPI STARTED");

  if (!radio.begin()){
    Serial.println("!!!!!!! RADIO NOT STARTED !!!!!!!!!!");
    ESP_ERROR_CHECK(ESP_FAIL);                                // Restart ESP until radio starts 
  }
  
  radio.setPALevel(RF24_PA_MAX, true); // ** Maximize range
  radio.setPayloadSize(sizeof(CAN_RF_PACKET));
  radio.setAutoAck(false); // just need speed, not perfect reliability
  // radio.setChannel(120);
  // radio.setCRCLength(RF24_CRC_DISABLED);

  
  

  #ifdef CAR_NODE
    radio.openWritingPipe(address[RADIO_NUMBER]);  // always uses pipe 0
    radio.stopListening();                         // put radio in TX mode
  #elif defined(BASE_NODE)
   radio.openReadingPipe(1, address[!RADIO_NUMBER]);
   radio.startListening();                         // put radio in RX mode
  #endif

  Serial.println("END RADIO SETUP");

}

static void radio_receive(void *args){
  ESP_LOGI(BASE_TAG, "Radio Receive Thread Started");
  while(1){
    if (xSemaphoreTake(radio_sem, pdMS_TO_TICKS(1))){
      
      uint8_t pipe;
      if (radio.available(&pipe)) { 
        bytes = radio.getPayloadSize();
        radio.read(&data_in, bytes);
        if(data_in.identifier && Serial){
          Serial.print("0x");
          Serial.print(data_in.identifier, HEX);
          Serial.print(" :");
          for(i=0; i<data_in.data_length_code; i++){
            Serial.print(" ");
            if(data_in.data[i]<16) Serial.print(0);
            Serial.print(data_in.data[i], HEX);
          }
          Serial.println();
          digitalWrite(LED_BUILTIN, counter & 1 << 6);
          counter++;
        } 
      }
      xSemaphoreGive(radio_sem);
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }


  vTaskDelete(NULL);
}

static void radio_transmit(void *args){
  ESP_LOGI(BASE_TAG, "Radio Transmit Thread Started");

  while(1){
    if (xSemaphoreTake(radio_sem, pdMS_TO_TICKS(1))){

      radio.write(&data_out, sizeof(CAN_RF_PACKET));

      xSemaphoreGive(radio_sem);
    }
    digitalWrite(LED_BUILTIN, counter & 1 << 6);
    counter++;

    vTaskDelay(pdMS_TO_TICKS(2));
  }

}