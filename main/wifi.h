#pragma once

#include "stdint.h"
#include "esp_err.h"
#include "tlv.h"

typedef enum {
    SUBTYPE_PROBE_REQ = 4,
    SUBTYPE_BEACON = 8,
    SUBTYPE_AUTH = 11,
    SUBTYPE_DEAUTH = 12,
} wifi_subtype_t;

typedef enum {
    ELT_TYPE_SSID = 0,
    ELT_TYPE_SUPPORTED_RATES = 1,
    ELT_TYPE_VENDOR_SPECIFIC = 0xDD,
} element_type_t;

typedef tlv_t wifi_element_t;
typedef tlv_info_t vfp_t;

/**
 * Initialize a vfp_t structure
 * pkt_len: length of packet including Frame Check Sequence(FCS) (rx_ctrl.sig_len)
 * pkt_start: pointer to the start of the packet
 * var_start: pointer to the variable part of the packet
 */
void variable_frame_parser_init(vfp_t* vfp, uint16_t pkt_len, uint8_t* pkt_start, uint8_t* var_start);

/**
 * Get the next wifi element, and update vfp_t structure
 * @return: pointer to element, or NULL if no ,more elements
 */
#define vfp_pop tlv_pop

/**
 *  @return pointer to Frame Check Control of the packet, or NULL.
 */
uint8_t* vfp_fc(vfp_t* vfp);


typedef struct __attribute__((packed)) {
    unsigned int protocol: 2;
    unsigned int type: 2;
    wifi_subtype_t subtype: 4;
    uint8_t stuff;//TODO
}  wifi_header_t;

typedef struct __attribute__((packed)) {
    uint8_t FC[2];
    uint8_t duration[2];
    uint8_t DA[6];
    uint8_t SA[6];
    uint8_t BSSID[6];
    uint8_t seq_ctl[2];     //MAC header ends here
    uint8_t frame_body[0];
} management_frame_t;

typedef struct __attribute__((packed)) {
    uint8_t timestamp[8];
    uint8_t beacon_interval[2];
    uint8_t capability_info[2];
    uint8_t variable[0];
} beacon_frame_body_t;
