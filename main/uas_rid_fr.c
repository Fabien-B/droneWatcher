
#include "uas_rid_fr.h"
#include <stdio.h>
#include "string.h"
#include <assert.h>
#include "tlv.h"

#define fill_swap32(dst)                       \
  int32_t *be = (int32_t *)(tlv->p_data); \
  dst = __builtin_bswap32(*be);                \
  payload->types |= (1 << field_type);

#define fill_swap16(dst)                       \
  int16_t *be = (int16_t *)(tlv->p_data); \
  dst = __builtin_bswap16(*be);                \
  payload->types |= (1 << field_type);

#define fill_swap16u(dst)                        \
  uint16_t *be = (uint16_t *)(tlv->p_data); \
  dst = __builtin_bswap16(*be);                  \
  payload->types |= (1 << field_type);


void parse_uav_info(uas_payload_t* payload, uint8_t *buf, uint8_t vs_type, uint8_t len)
{

  payload->id_fr[30] = '\0';
  
  tlv_info_t tlv_info;
  tlv_info_init(&tlv_info, buf, len);
  tlv_t* tlv = NULL;

  while ((tlv = tlv_pop(&tlv_info)) != NULL)
  {
    enum uas_field_type field_type = (enum uas_field_type)tlv->type;

    if (field_type == UAS_ID_FR)
    {
      assert(tlv->length == 30);
      memcpy(payload->id_fr, tlv->p_data, tlv->length); // copy data in structure
      payload->types |= (1 << field_type);                 // set field flag
    }
    else if (field_type == UAS_ID_ANSI_UAS)
    {
      // Max lenght is 4 (MFR) + 1 (len) + 15 (MFR's SN) = 20
      assert(tlv->length <= 20);
      memcpy(payload->id_fr, tlv->p_data, tlv->length);
      payload->id_fr[tlv->length] = '\0';
      payload->types |= (1 << UAS_ID_ANSI_UAS);
    }
    else if (field_type == UAS_LAT)
    {
      assert(tlv->length == 4);
      fill_swap32(payload->lat);
    }
    else if (field_type == UAS_LON)
    {
      assert(tlv->length == 4);
      fill_swap32(payload->lon)
    }
    else if (field_type == UAS_HMSL)
    {
      assert(tlv->length == 2);
      fill_swap16(payload->hmsl)
    }
    else if (field_type == UAS_HAGL)
    {
      assert(tlv->length == 2);
      fill_swap16(payload->hagl)
    }
    else if (field_type == UAS_LAT_TO)
    {
      assert(tlv->length == 4);
      fill_swap32(payload->lat_to)
    }
    else if (field_type == UAS_LON_TO)
    {
      assert(tlv->length == 4);
      fill_swap32(payload->lon_to)
    }
    else if (field_type == UAS_H_SPEED)
    {
      assert(tlv->length == 1);
      payload->h_speed = tlv->p_data[0];
      payload->types |= (1 << field_type);
    }
    else if (field_type == UAS_ROUTE)
    {
      assert(tlv->length == 2);
      fill_swap16u(payload->route)
    }
    else if (field_type == UAS_PROTOCOL_VERSION)
    {
      assert(tlv->length == 1);
      assert(tlv->p_data[0] == 0x01);
    }
  }
}


void display_uas_info(uas_payload_t *info)
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
