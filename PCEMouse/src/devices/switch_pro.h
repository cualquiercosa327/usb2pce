// switch_pro.h
#ifndef SWITCH_PRO_HEADER_H
#define SWITCH_PRO_HEADER_H

#include "device_interface.h"
#include "device_utils.h"
#include "tusb.h"

// Switch Pro Output Commands
#define PROCON_REPORT_SEND_USB 0x80
#define PROCON_USB_HANDSHAKE   0x02
#define PROCON_USB_BAUD        0x03
#define PROCON_USB_ENABLE      0x04
#define PROCON_USB_DO_CMD      0x92
#define PROCON_CMD_AND_RUMBLE  0x01
#define PROCON_CMD_MODE        0x03
#define PROCON_CMD_LED_HOME    0x38
#define PROCON_ARG_INPUT_FULL  0x30

extern DeviceInterface switch_pro_interface;

// Switch Pro Input Report
typedef struct {
  uint8_t  report_id;   // The first byte is always the report ID
  uint8_t  timer;       // Timer tick (1 tick = 5ms)
  uint8_t  battery_level_and_connection_info; // Battery level and connection info
  struct {
    uint8_t y    : 1;
    uint8_t x    : 1;
    uint8_t b    : 1;
    uint8_t a    : 1;
    uint8_t sr_r : 1;
    uint8_t sl_r : 1;
    uint8_t r    : 1;
    uint8_t zr   : 1;
  };

  struct {
    uint8_t select  : 1;
    uint8_t start   : 1;
    uint8_t rstick  : 1;
    uint8_t lstick  : 1;
    uint8_t home    : 1;
    uint8_t cap     : 1;
    uint8_t padding : 2;
  };

  struct {
    uint8_t down  : 1;
    uint8_t up    : 1;
    uint8_t right : 1;
    uint8_t left  : 1;
    uint8_t sr_l  : 1;
    uint8_t sl_l  : 1;
    uint8_t l     : 1;
    uint8_t zl    : 1;
  };
  uint8_t  left_stick[3]; // 12 bits for X and Y each (little endian)
  uint8_t  right_stick[3]; // 12 bits for X and Y each (little endian)
  uint8_t  vibration_ack; // Acknowledge output reports that trigger vibration
  uint8_t  subcommand_ack; // Acknowledge if a subcommand was executed
  uint8_t  subcommand_reply_data[35]; // Reply data for executed subcommands

  uint16_t left_x, left_y, right_x, right_y;
} switch_pro_report_t;

typedef union
{
  switch_pro_report_t data;
  uint8_t buf[sizeof(switch_pro_report_t)];
} switch_pro_report_01_t;

uint32_t buttons;

#endif
