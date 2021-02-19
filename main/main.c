/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "twatch.h"
#include "ui/ui.h"
#include "ui/widget.h"
#include "ui/label.h"
#include "ui/button.h"
#include "drone_sniffer.h"
#include "interface.h"

void app_main(void)
{
    printf("Hello world!\n");

    twatch_pmu_init();
    twatch_pmu_power(true);
    twatch_pmu_screen_power(true);
    twatch_vibrate_init();

    st7789_init();
    ui_init();
    st7789_backlight_on();


    setup_ui();
    
    
    drone_sniffer_start();



    // touch_event_t te;
    // vibrate_pattern_t vp[4] = {
    //     {50, VIBRATE_ON},
    //     {200, VIBRATE_OFF},
    //     {50, VIBRATE_ON},
    //     {200, VIBRATE_OFF},
    // };

    for(;;) {
        //bool p = twatch_pmu_is_userbtn_pressed();

        ui_process_events();
        //twatch_vibrate_pattern(vp, 4);

        vTaskDelay(100 / portTICK_PERIOD_MS);

        // if(ssid_changed) {
        //     ssid_changed = false;
        //     for(int i=0; i<7; i++) {
        //         printf("%s", ssids[i]);
        //         widget_label_set_text(&label_ssids[i], ssids[i]);
            
        //     }
        // }
        
    }
}
