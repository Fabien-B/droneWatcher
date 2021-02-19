
#include "wifi.h"
#include "tlv.h"

#include <stdio.h>

void variable_frame_parser_init(vfp_t* vfp, uint16_t pkt_len, uint8_t* pkt_start, uint8_t* var_start) {
  size_t len = pkt_len - (var_start-pkt_start);
  tlv_info_init(vfp, var_start, len);
}

