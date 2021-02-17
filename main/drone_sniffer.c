/*
  Drone Sniffer
  
*/

//#define configSUPPORT_STATIC_ALLOCATION 1

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

#define MIN(a, b) (a < b ? a : b)

//trash code to remove!
char ssids[7][20];
bool ssid_changed = false;

static const char *TAG = "SNIFFER";

static const uint8_t CID_french_defense[] = {0x6A, 0x5C, 0x35};

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  ESP_LOGI(TAG, "Event : %d", event->event_id);
  return ESP_OK;
}

uint8_t get_element_info(uint8_t *buf, int max_len, uint8_t *type)
{
  if (max_len < 2)
  {
    return 0;
  }
  *type = buf[0];
  uint8_t len = buf[1];

  if (max_len < len + 2)
  {
    return 0; //return max_len ???
  }

  return len;
}

char *read_SSID(uint8_t *buf, uint8_t ssid_len)
{

  char *ssid = malloc((ssid_len + 1) * sizeof(char));
  for (int i = 0; i < ssid_len; i++)
  {
    ssid[i] = buf[i];
  }
  ssid[ssid_len] = 0;
  return ssid;
}

#define fill_swap32(dst)                       \
  int32_t *be = (int32_t *)(buf + offset + 2); \
  dst = __builtin_bswap32(*be);                \
  payload.types |= (1 << field_type);

#define fill_swap16(dst)                       \
  int16_t *be = (int16_t *)(buf + offset + 2); \
  dst = __builtin_bswap16(*be);                \
  payload.types |= (1 << field_type);

#define fill_swap16u(dst)                        \
  uint16_t *be = (uint16_t *)(buf + offset + 2); \
  dst = __builtin_bswap16(*be);                  \
  payload.types |= (1 << field_type);

struct uas_raw_payload read_uav_info(uint8_t *buf, uint8_t vs_type, uint8_t len)
{

  struct uas_raw_payload payload = {0}; //init empty payload
  payload.id_fr[30] = '\0';
  int offset = 0;
  while (offset < len)
  {
    enum uas_type field_type = (enum uas_type)buf[offset];
    int field_len = buf[offset + 1];

    if (offset + field_len + 2 > len)
    { //is the announced field length compatible with the buffer length ?
      break;
    }

    if (field_type == UAS_ID_FR)
    {
      assert(field_len == 30);
      memcpy(payload.id_fr, buf + offset + 2, field_len); // copy data in structure
      payload.types |= (1 << field_type);                 // set field flag
    }
    else if (field_type == UAS_ID_ANSI_UAS)
    {
      // Max lenght is 4 (MFR) + 1 (len) + 15 (MFR's SN) = 20
      assert(field_len <= 20);
      memcpy(payload.id_fr, buf + offset + 2, field_len);
      payload.id_fr[field_len] = '\0';
      payload.types |= (1 << UAS_ID_ANSI_UAS);
    }
    else if (field_type == UAS_LAT)
    {
      assert(field_len == 4);
      fill_swap32(payload.lat);
    }
    else if (field_type == UAS_LON)
    {
      assert(field_len == 4);
      fill_swap32(payload.lon)
    }
    else if (field_type == UAS_HMSL)
    {
      assert(field_len == 2);
      fill_swap16(payload.hmsl)
    }
    else if (field_type == UAS_HAGL)
    {
      assert(field_len == 2);
      fill_swap16(payload.hagl)
    }
    else if (field_type == UAS_LAT_TO)
    {
      assert(field_len == 4);
      fill_swap32(payload.lat_to)
    }
    else if (field_type == UAS_LON_TO)
    {
      assert(field_len == 4);
      fill_swap32(payload.lon_to)
    }
    else if (field_type == UAS_H_SPEED)
    {
      assert(field_len == 1);
      payload.h_speed = buf[offset + 2];
      payload.types |= (1 << field_type);
    }
    else if (field_type == UAS_ROUTE)
    {
      assert(field_len == 2);
      fill_swap16u(payload.route)
    }
    else if (field_type == UAS_PROTOCOL_VERSION)
    {
      assert(field_len == 1);
      assert(buf[offset + 2] == 0x01);
    }

    offset += field_len + 2;
  }

  return payload;
}

void display_info(struct uas_raw_payload *info)
{
  if (info->types & (1 << UAS_ID_FR))
  {
    printf("FR_ID: %s\n", info->id_fr);
  }
  if (info->types & (1 << UAS_ID_ANSI_UAS))
  {
    printf("ANSI SN: %s\n", info->id_fr);
  }
  if (info->types & (1 << UAS_LAT))
  {
    printf("LAT: %f\n", info->lat / 1e5);
  }
  if (info->types & (1 << UAS_LON))
  {
    printf("LON: %f\n", info->lon / 1e5);
  }
  if (info->types & (1 << UAS_HMSL))
  {
    printf("HMSL: %d\n", info->hmsl);
  }
  if (info->types & (1 << UAS_HAGL))
  {
    printf("HAGL: %d\n", info->hagl);
  }
  if (info->types & (1 << UAS_LAT_TO))
  {
    printf("LAT TO: %f\n", info->lat_to / 1e5);
  }
  if (info->types & (1 << UAS_LON_TO))
  {
    printf("LON TO: %f\n", info->lon_to / 1e5);
  }
  if (info->types & (1 << UAS_H_SPEED))
  {
    printf("H SPEED: %d\n", info->h_speed);
  }
  if (info->types & (1 << UAS_ROUTE))
  {
    printf("ROUTE: %d\n", info->route);
  }

  printf("\n");
}

void wifi_promiscuous_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
  if (type == WIFI_PKT_MGMT)
  {
    //uint8_t protocol_version = (pkt->payload[0]&0x03);
    uint8_t frame_type = (pkt->payload[0] & 0x0C) << 2;
    frame_type += (pkt->payload[0] & 0xF0) >> 4;

    if (frame_type == 0x08)
    {
      char *ssid = NULL;
      int offset = 36;
      while (true)
      {
        uint8_t e_type;
        uint8_t len = get_element_info(pkt->payload + offset, pkt->rx_ctrl.sig_len - offset, &e_type);
        if (len == 0)
        {
          break;
        }

        if (e_type == 0)
        { //SSID
          ssid = read_SSID(pkt->payload + offset + 2, len);
          printf("SSID = %s\n", ssid);

          for(int i=0; i<7; i++) {
              if(strcmp(ssids[i], ssid) == 0) {
                  // ssid already saved
                  break;
              }
              if(ssids[i][0] == '\0') {
                  //end of recorded SSIDs, we put it here
                  strcpy(ssids[i], ssid);
                  ssid_changed = true;
                  break;
              }
          }
        }
        else if (e_type == 0XDD)
        { //Vendor Specific
          uint8_t CID[3];
          memcpy(CID, pkt->payload + offset + 2, 3);
          uint8_t vs_type = pkt->payload[offset + 5];
          bool same = true;
          for (int i = 0; i < 3; i++)
          {
            if (CID[i] != CID_french_defense[i])
            {
              same = false;
            }
          }
          if (same)
          {
            struct uas_raw_payload raw_info = read_uav_info(pkt->payload + offset + 6, vs_type, len - 4);
            printf("SSID = %s\n", ssid);
            display_info(&raw_info);
          }
        }

        offset += len + 2;
      }
    }
  }
}

#define DEFAULT_SCAN_LIST_SIZE 3

void drone_sniffer_start(void *parameter)
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


  while (true)
  {
    vTaskDelay(2000 / portTICK_RATE_MS);
  }

  vTaskDelete( NULL );
}

#define DS_STACK_SIZE 5000

StaticTask_t t_buff;
StackType_t tStackBuff[DS_STACK_SIZE];


void drone_sniffer_init() {
    xTaskCreateStatic(drone_sniffer_start, "drone_sniffer", DS_STACK_SIZE, (void*)NULL, 1, tStackBuff, &t_buff);
}