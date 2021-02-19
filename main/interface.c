#include "interface.h"
#include "drone_sniffer.h"

tile_t tiles[NB_DRONES_MAX];
record_labels_t labels[NB_DRONES_MAX];

#define LABEL_H 26

void init_labels(record_labels_t* rec, tile_t* tile) {
    widget_label_init(&rec->ssid, tile, 0, 0, 240, 30, "ssid");
    widget_label_init(&rec->id, tile, 0, LABEL_H, 240, 30, "id");
    widget_label_init(&rec->lat, tile, 0, 2*LABEL_H, 240, 30, "lat");
    widget_label_init(&rec->lon, tile, 0, 3*LABEL_H, 240, 30, "lon");
    widget_label_init(&rec->hmsl, tile, 0, 4*LABEL_H, 240, 30, "hmsl");
    widget_label_init(&rec->lat_to, tile, 0, 5*LABEL_H, 240, 30, "lat_to");
    widget_label_init(&rec->lon_to, tile, 0, 6*LABEL_H, 240, 30, "lon_to");
    widget_label_init(&rec->hspeed, tile, 0, 7*LABEL_H, 240, 30, "hspeed");
    widget_label_init(&rec->route, tile, 0, 8*LABEL_H, 240, 30, "route");
}

void setup_ui() {
    for(int i=0; i<NB_DRONES_MAX; i++) {
        tile_init(&tiles[i], NULL);
        init_labels(&labels[i], &tiles[i]);
    }
    for(int i=0; i<NB_DRONES_MAX-1; i++) {
        tile_link_right(&tiles[i], &tiles[i+1]);
    }
    
    ui_select_tile(&tiles[0]);
}
