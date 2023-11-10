#include <stdio.h>
#include "../headers/message_window_status.h"
#include "../../parts.h"

int message_window_status() {
    if (message_window.visible == OUT_VISIBLE) {
        message_window.visible = IN_VISIBLE;
    } else {
        message_window.visible = OUT_VISIBLE;
    }

    return 0;
}
