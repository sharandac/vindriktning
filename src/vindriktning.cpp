/**
 * @file vindriktning.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "config.h"
#include "utils/alloc.h"
#include "modules/mqttclient.h"
#include "modules/webserver.h"

#include "vindriktning.h"
#include "config/vindriktning_config.h"

#include "pm1006.hpp"
/**
 * local variables
 */
vindriktning pm1006;                            /** @brief pm1006 object */
TaskHandle_t _Vindriktning_Task;                /** @brief handle for vindriktning task */
vindriktning_config_t vindriktning_config;      /** @brief vindriktning config */
/**
 * local static funtions
 */
static bool vindriktning_mqtt_cb( EventBits_t event, void *arg );
static bool vindriktning_webserver_cb( EventBits_t event, void *arg );
static void vindriktning_Task( void * pvParameters );
/**
 * @brief start the task
 */
void vindriktning_init( void ) {
    /**
     * load config
     */
    vindriktning_config.load();
    /**
     * start pm1006
     */
    pm1006.begin( vindriktning_config.RXpin );
    /**
     * start task
     */
    xTaskCreatePinnedToCore(    vindriktning_Task,          /* Function to implement the task */
                                "vindriktning Task",        /* Name of the task */
                                2500,                       /* Stack size in words */
                                NULL,                       /* Task input parameter */
                                1,                          /* Priority of the task */
                                &_Vindriktning_Task,        /* Task handle. */
                                1 );                        /* Core where the task should run */   
    /**
     * register mqtt and webserver callback functions
     */
    mqtt_client_register_cb( MQTTSTAT_JSON_DATA, vindriktning_mqtt_cb, "vindriktning" );
    asyncwebserver_register_cb( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, vindriktning_webserver_cb, "/vindriktning.htm" );
}

/**
 * @brief vindriktning task
 * 
 * @param pvParameters 
 */
static void vindriktning_Task( void * pvParameters ) {    
    log_i("Start Vindriktning Task on Core: %d", xPortGetCoreID() );
    /**
     * loop forever
     */
    while(1) {
        pm1006.loop();
        vTaskDelay( 10 );
    }
}

static bool vindriktning_mqtt_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    mqttData_t *mqttData = (mqttData_t*)arg;

    switch( event ) {
        case MQTTSTAT_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, "vindriktning: mqttstat json data is NULL");
            DynamicJsonDocument& doc = *mqttData->doc;
            /**
             * insert pm1006 data
             */
            doc["sensor"]["pm1006"]["pm1"] = pm1006.isPm1Valid() ? pm1006.getPm1Value() : 0; 
            doc["sensor"]["pm1006"]["pm25"] = pm1006.isPm25Valid() ? pm1006.getPm25Value() : 0; 
            doc["sensor"]["pm1006"]["pm10"] = pm1006.isPm10Valid() ? pm1006.getPm10Value() : 0; 
            retval = true;
            break;
    }
    return( retval );
}

static bool vindriktning_webserver_cb( EventBits_t event, void *arg ) {
    bool retval = true;
    wsData_t *ws_data = (wsData_t *)arg;
    AsyncWebSocketClient * client = ws_data->client;
    AsyncWebServerRequest * request = ws_data->request;
    const char *cmd = ws_data->cmd ? ws_data->cmd : "";
    const char *value = ws_data->value ? ws_data->value : "";
    String html;
    
    switch( event ) {
        case WS_DATA:
            ASSERT( client, "client is NULL, check your code!");
            /**
             * check all commands
             */
            if ( !strcmp( "SAV", cmd ) ) {
                vindriktning_config.save();
            }
            else if ( !strcmp("get_vindriktning_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg("vindriktning_rxpin\\%d", vindriktning_config.RXpin );
            }
            else if ( !strcmp( "vindriktning_rxpin", cmd ) ) {
                vindriktning_config.RXpin = atoi( value );
                asyncwebserver_send_websocket_msg("vindriktning_rxpin\\%d", vindriktning_config.RXpin );
            }
            else if ( !strcmp( "get_vindriktning_data", cmd ) ) {
                asyncwebserver_send_websocket_msg("vindriktning_pm1\\%d", pm1006.isPm1Valid() ? pm1006.getPm1Value() : 0 );
                asyncwebserver_send_websocket_msg("vindriktning_pm25\\%d", pm1006.isPm25Valid() ? pm1006.getPm25Value() : 0 );
                asyncwebserver_send_websocket_msg("vindriktning_pm10\\%d", pm1006.isPm10Valid() ? pm1006.getPm10Value() : 0 );
                asyncwebserver_send_websocket_msg("vindriktning_pm1006_packet\\%s", pm1006.getDataFrame() );
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( request, "request is NULL, check your code!");
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += vindriktning_config_page;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/vindriktning.htm", "vindriktning" );
            retval = true;
            break;
        case SAVE_CONFIG:
            vindriktning_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            vindriktning_config.resetToDefault();
            retval = true;
            break;
    }
    return( retval );
}