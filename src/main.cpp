/**
 * @file main.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_task_wdt.h>
#include "esp_err.h"
#include "SPIFFS.h"
#include "utils/callback.h"

#include "config.h"
#include "modules/mqttclient.h"
#include "modules/wificlient.h"
#include "modules/webserver.h"
#include "modules/ntp.h"

#include "vindriktning.h"

#define WDT_TIMEOUT 60

void setup() {
    setCpuFrequencyMhz( 240 );
    Serial.begin( 115200 );
    /*
     * enable watchdog
     */
    esp_task_wdt_init( WDT_TIMEOUT, true );
    esp_task_wdt_add( NULL );
    /**
     * mount SPIFFS
     */
    if ( !SPIFFS.begin() ) {        
        /*
         * format SPIFFS if not aviable
         */
        SPIFFS.format();
        log_i("formating SPIFFS");
    }  
    /**
     * init custom modules
     */
    vindriktning_init();
    /**
     * start services with own task
     */
    wificlient_init();
    mqtt_client_StartTask();
    ntp_StartTask();
    asyncwebserver_StartTask();
    /**
     * delay for 2.5 sec and put out callback table
     */
    vTaskDelay( 2500 );
    callback_print();
}

void loop() {
    /**
     * reset watchdag
     */
    esp_task_wdt_reset();
}