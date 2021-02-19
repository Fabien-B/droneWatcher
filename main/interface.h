#pragma once

#include "twatch.h"
#include "ui/ui.h"
#include "ui/widget.h"
#include "ui/label.h"
#include "ui/button.h"

typedef struct {
    widget_label_t ssid;
    widget_label_t id;
    widget_label_t lat;
    widget_label_t lon;
    widget_label_t hmsl;
    widget_label_t lat_to;
    widget_label_t lon_to;
    widget_label_t hspeed;
    widget_label_t route;
} record_labels_t;

void setup_ui(void);

