#pragma once
#include "stdint.h"
#include "stddef.h"

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t p_data[0];
} tlv_t;

/**
 * Structure holding info about TLV buffer
 */
typedef struct {
    size_t remaining_length;
    uint8_t* head;
} tlv_info_t;

void tlv_info_init(tlv_info_t* tlvi, uint8_t* buf, size_t len);

tlv_t* tlv_pop(tlv_info_t* tlvi);
