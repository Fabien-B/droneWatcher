
#include "uas_rid_fr.h"
#include <stdio.h>
#include "string.h"
#include <assert.h>
#include "tlv.h"

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



struct uas_payload parse_uav_info(uint8_t *buf, uint8_t vs_type, uint8_t len)
{

  struct uas_payload payload = {0}; //init empty payload
  payload.id_fr[30] = '\0';
  int offset = 0;
  while (offset < len)
  {
    enum uas_field_type field_type = (enum uas_field_type)buf[offset];
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


void display_uas_info(struct uas_payload *info)
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
