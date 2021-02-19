#include "tlv.h"
#include <stddef.h>

tlv_t* tlv_pop(tlv_info_t* tlvi) {
  if(tlvi->remaining_length <= 4) {  //FCS is 4 bytes
    return NULL;
  } else {
    tlv_t* tlv_p = (tlv_t*) tlvi->head;
    tlvi->head = tlvi->head + tlv_p->length + 2;
    tlvi->remaining_length -= tlv_p->length + 2;
    return tlv_p;
  }
}

void tlv_info_init(tlv_info_t* tlvi, uint8_t* buf, size_t len) {
  tlvi->remaining_length = len;
  tlvi->head = buf;
}

