
#include "wifi.h"

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