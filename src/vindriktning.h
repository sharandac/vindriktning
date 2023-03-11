/**
 * @file vindriktning.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _VINDRIKTNING_H
    #define _VINDRIKTNING_H
    /**
     * @brief start the task
     */
    void vindriktning_init( void );
    /**
     * @brief vindriktning config page
     */
    static const char vindriktning_config_page[] =
        "<h1>vindriktning settings</h1>\n"
        "<script>set_settings_namespace(\"get_vindriktning_settings\");setInterval(function () {sendCMD(\"get_vindriktning_data\");}, 5000);</script>\n"
        "<div id=\"vindriktning_settings\" class=\"hbox\">\n"
        "  <div class=\"vbox\">\n"
        "    <label>vindriktning RX pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='63' id='vindriktning_rxpin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <h2>vindriktning hardware monitor</h2>"
        "  <div class=\"vbox\">\n"
        "    <label>1.0µm</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='63' id='vindriktning_pm1' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>2.5µm</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='63' id='vindriktning_pm25' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>10µm</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='63' id='vindriktning_pm10' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>pm1006 packet</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='63' id='vindriktning_pm1006_packet' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"vindriktning_rxpin\");SaveSettings();get_settings();' class='button'>save</button>";

#endif // _VINDRIKTNING_H