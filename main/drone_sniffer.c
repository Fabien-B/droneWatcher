/*
  Drone Sniffer
  
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "sdkconfig.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "string.h"
#include "drone_sniffer.h"
#include "wifi.h"
#include "uas_rid_fr.h"
#include <assert.h>

#define MIN(a, b) (a < b ? a : b)

//trash code to remove!
char ssids[7][20];
bool ssid_changed = false;

static const char *TAG = "SNIFFER";

static const uint8_t CID_french_defense[] = {0x6A, 0x5C, 0x35};

// static esp_err_t event_handler(void *ctx, system_event_t *event)
// {
//   ESP_LOGI(TAG, "Event : %d", event->event_id);
//   return ESP_OK;
// }


void wifi_promiscuous_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
  if (type == WIFI_PKT_MGMT)
  {
    wifi_header_t* header = (wifi_header_t*)pkt->payload;

    if (header->type == 0 && header->subtype == SUBTYPE_BEACON)
    {
      management_frame_t* mgnt_frame = (management_frame_t*) pkt->payload;
      beacon_frame_body_t* bfb = (beacon_frame_body_t*) mgnt_frame->frame_body;
      vfp_t vfp;
      wifi_element_t* element_p = NULL;
      
      //SSID is 32 bytes long max. Make it 33 to add \0.
      char ssid[33] = {0};

      variable_frame_parser_init(&vfp, pkt->rx_ctrl.sig_len, (uint8_t*)mgnt_frame, bfb->variable);

      while((element_p = vfp_pop(&vfp)) != NULL) {
        // printf("[DS] element at addr %p\n", &element_p);
        // printf("[DS] type %d at addr %p\n", element_p->type, &(element_p->type));
        if (element_p->type == ELT_TYPE_SSID)
        { //SSID
          memcpy(ssid, element_p->p_data, element_p->length);
          ssid[element_p->length] = '\0';
          printf("%d SSID = %s\n", pkt->rx_ctrl.channel, ssid);
        }
        else if (element_p->type == ELT_TYPE_VENDOR_SPECIFIC)
        { //Vendor Specific
          if(element_p->length < 3) {
            printf("WTF VS sans OUI/CID ???\r\n");
            continue;
          }

          uint8_t CID[3];
          memcpy(CID, element_p->p_data, 3);
          uint8_t vs_type = element_p->p_data[3];         
          if (memcmp(CID, CID_french_defense, 3) == 0)
          {
            uas_payload_t payload;
            parse_uav_info(&payload, element_p->p_data + 4, vs_type, element_p->length - 4);
            //printf("SSID = %s\n", ssid);
            display_uas_info(&payload);
          }
        }
      }
    }
  }
}

#define DEFAULT_SCAN_LIST_SIZE 3

void hop_channel(void* param) {
  (void)param;
  int channel = 0;
  while(true) {
    channel = (channel + 1) % 12;
    ESP_ERROR_CHECK(esp_wifi_set_channel(channel + 1, WIFI_SECOND_CHAN_NONE));
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}


void drone_sniffer_start()
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  for(int i=0; i<7; i++) {
      ssids[i][0] = '\0';
  }


  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));


  wifi_country_t country_conf;
  country_conf.schan = 1;
  country_conf.nchan = 13;
  country_conf.policy = WIFI_COUNTRY_POLICY_AUTO;
  ESP_ERROR_CHECK(esp_wifi_set_country(&country_conf));

  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous_cb));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE));

  // xTaskCreate(hop_channel, "channel hoping", 2000, NULL, 1, NULL);
}

