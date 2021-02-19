
#include "wifi.h"
#include "tlv.h"

#include <stdio.h>

void variable_frame_parser_init(vfp_t* vfp, uint16_t pkt_len, uint8_t* pkt_start, uint8_t* var_start) {
  size_t len = pkt_len - (var_start-pkt_start);
  tlv_info_init(vfp, var_start, len);
}

// wifi_element_t* vfp_pop(vfp_t* vfp) {
//   if(vfp->remaining_length <= 4) {  //FCS is 4 bytes
//     return NULL;
//   } else {
//     wifi_element_t* element_p = (wifi_element_t*) vfp->head;
//     vfp->head = vfp->head + element_p->length + 2;
//     vfp->remaining_length -= element_p->length + 2;
//     return element_p;
//   }
// }

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