//------------------------------------------------------------------------------
/**
 * @file main.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief odroid mac address printer app with the net label printer.
 * @version 0.1
 * @date 2024-01-08
 *
 * @copyright Copyright (c) 2022
 *
 */
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//------------------------------------------------------------------------------
#include "lib_fbui/lib_fb.h"
#include "lib_fbui/lib_ui.h"
#include "lib_fbui/lib_ts.h"

#include "lib_nlp/lib_nlp.h"

//------------------------------------------------------------------------------
#define DEVICE_FB   "/dev/fb0"
#define DEVICE_TS   "/input/event0"
#define UI_CFG_FILE "ui.cfg"

#define NLP_IFACE_NAME  "eth0"

//------------------------------------------------------------------------------
enum {
    eID_0 = 140,
    eID_1 = 142,
    eID_2 = 144,
    eID_3 = 146,
    eID_4 = 120,
    eID_5 = 122,
    eID_6 = 124,
    eID_7 = 126,
    eID_8 = 100,
    eID_9 = 102,
    eID_A = 104,
    eID_B = 106,
    eID_C = 80,
    eID_D = 82,
    eID_E = 84,
    eID_F = 86,
    eID_BS = 128,
    eID_CLR = 88,
    eID_PRINT = 160,
    eID_END
};

//------------------------------------------------------------------------------
static int run_interval_check (struct timeval *t, double interval_ms)
{
    struct timeval base_time;
    double difftime;

    gettimeofday(&base_time, NULL);

    if (interval_ms) {
        /* 현재 시간이 interval시간보다 크면 양수가 나옴 */
        difftime = (base_time.tv_sec - t->tv_sec) +
                    ((base_time.tv_usec - (t->tv_usec + interval_ms * 1000)) / 1000000);

        if (difftime > 0) {
            t->tv_sec  = base_time.tv_sec;
            t->tv_usec = base_time.tv_usec;
            return 1;
        }
        return 0;
    }
    /* 현재 시간 저장 */
    t->tv_sec  = base_time.tv_sec;
    t->tv_usec = base_time.tv_usec;
    return 1;
}

//------------------------------------------------------------------------------
#define ALIVE_CHECK_INTERVAL    1000

void alive_display (fb_info_t *pfb, ui_grp_t *pui)
{
    static struct timeval i_time;
    static int onoff = 0;

    if (run_interval_check(&i_time, ALIVE_CHECK_INTERVAL)) {
        ui_set_ritem (pfb, pui, 0,
                    onoff ? COLOR_GREEN : pui->bc.uint, -1);

        if (onoff)  ui_update (pfb, pui, -1);

        onoff = !onoff;
    }
}

//------------------------------------------------------------------------------
int main (void)
{
    fb_info_t   *pfb;
    ui_grp_t    *pui;
    ts_t        *p_ts;
    struct nlp_info nlp_info;

    if ((pfb = fb_init (DEVICE_FB)) == NULL) {
        fprintf(stdout, "ERROR: frame buffer init fail!\n");
        exit(1);
    } else {
        fb_cursor (0);
        if ((pui = ui_init (pfb, UI_CFG_FILE)) == NULL) {
            fprintf(stdout, "ERROR: User interface create fail!\n");
            exit(1);
        }
        ui_update(pfb, pui, -1);

        // nlp init
        memset (&nlp_info, 0, sizeof(nlp_info));
        if (nlp_init (&nlp_info, NLP_IFACE_NAME)) {
            char msg[100];
            memset (msg, 0, sizeof(msg));
            sprintf (msg, "%s,%s,%s", nlp_info.mac, nlp_info.ip, "ODROID MAC PRINTER");
            nlp_printf (&nlp_info, MSG_TYPE_ERR, msg, CH_NONE);
            ui_set_printf   (pfb, pui, 23, "%s", nlp_info.ip);

        } else {
            fprintf(stdout, "ERROR: NLP init fail!\n");
            exit(1);
        }

        // ts init
        if ((p_ts = ts_init ("/dev/input/event0")) == NULL) {
            fprintf(stdout, "ERROR: Touchscreen init fail!\n");
            exit(1);
        }
    }

    // ts input test
    {
        ts_event_t event;

        char mac_addr[7], print_mac[20], num = 0;
        int mac_input_pos = 0, ui_id;

        memset (mac_addr, '-', sizeof(mac_addr));

        while (1) {
            usleep (10000);
            alive_display(pfb, pui);
            if (ts_get_event (p_ts, &event)) {
                ui_id = ui_get_titem (pfb, pui, &event);
                if (event.status == eTS_STATUS_RELEASE) {
                    num = 0;
                    switch (ui_id) {
                        case eID_0: num = '0';  break;
                        case eID_1: num = '1';  break;
                        case eID_2: num = '2';  break;
                        case eID_3: num = '3';  break;
                        case eID_4: num = '4';  break;
                        case eID_5: num = '5';  break;
                        case eID_6: num = '6';  break;
                        case eID_7: num = '7';  break;
                        case eID_8: num = '8';  break;
                        case eID_9: num = '9';  break;
                        case eID_A: num = 'A';  break;
                        case eID_B: num = 'B';  break;
                        case eID_C: num = 'C';  break;
                        case eID_D: num = 'D';  break;
                        case eID_E: num = 'E';  break;
                        case eID_F: num = 'F';  break;
                        case eID_BS:
                            if (mac_input_pos)
                                mac_input_pos--;
                            mac_addr [mac_input_pos] = '-';
                            break;
                        case eID_CLR:
                            memset (mac_addr, '-', sizeof(mac_addr));
                            mac_input_pos = 0;
                            break;
                        case eID_PRINT:
                            memset  (print_mac, 0, sizeof(print_mac));
                            sprintf (print_mac, "001E06%s", mac_addr);
                            nlp_printf (&nlp_info, MSG_TYPE_MAC, print_mac, CH_LEFT);
                            break;
                        default :
                            break;
                    }
                    if (num != 0) {
                        mac_addr [mac_input_pos] = num;
                        if (mac_input_pos < sizeof(mac_addr) -1)
                            mac_input_pos++;
                    }
                    memset  (print_mac, 0, sizeof(print_mac));
                    sprintf (print_mac, "%c%c:%c%c:%c%c",   mac_addr[0], mac_addr[1], mac_addr[2],
                                                            mac_addr[3], mac_addr[4], mac_addr[5]);
                    ui_set_printf (pfb, pui, 62, "%s", print_mac);
                }
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
