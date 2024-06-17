/* 
  ******************** CONSTANTS FILE ********************

  - Please use this for all variables that remain constant :)
  - Please label definitions with useful information on the right
  - Add new sections as needed

 */



/* --------------------- ESP-IDF Setup ------------------ */
  // LOG TAGS
    #ifdef CAR_NODE
    #   define BASE_TAG    "CAR-Transceiver"
    #elif defined(BASE_NODE)
    #   define BASE_TAG    "BASE-Transceiver"  
    #endif 


/* ---------------------  IO ------------------ */
  // LEDS
    #ifndef LED_BUILTIN
      #ifdef CAR_NODE
        #define LED_BUILTIN   D2
      #elif defined(BASE_NODE)
        #define LED_BUILTIN   D8
      #endif
    #endif

/* --------------------- Logic Code ------------------ */





    