#pragma once

#include "stdint.h"

typedef enum {
    SUBTYPE_PROBE_REQ = 4,
    SUBTYPE_BEACON = 8,
    SUBTYPE_AUTH = 11,
    SUBTYPE_DEAUTH = 12,
} wifi_subtype_t;



typedef struct __attribute__((packed)) {
    unsigned int protocol: 2;
    unsigned int type: 2;
    wifi_subtype_t subtype: 4;
    uint8_t stuff;
}  wifi_header_t;

typedef struct __attribute__((packed)) {
    uint8_t FC[2];
    uint8_t duration[2];
    uint8_t DA[6];
    uint8_t SA[6];
    uint8_t BSSID[6];
    uint8_t seq_ctl[2];
    uint8_t frame_body[0];
} management_frame_t;

typedef struct __attribute__((packed)) {
    uint8_t timestamp[8];
    uint8_t beacon_interval[2];
    uint8_t capability_info[2];
    uint8_t variable[0];
} beacon_frame_body_t;

typedef enum {
    ELT_TYPE_SSID = 0,
    ELT_TYPE_SUPPORTED_RATES = 1,
    ELT_TYPE_VENDOR_SPECIFIC = 0xDD,
} element_type_t;


typedef struct {
    uint8_t type;
    uint8_t length;
    uint8_t* p_data;
} wifi_element_t;

uint8_t get_element_info(uint8_t *buf, int max_len, uint8_t *type);
