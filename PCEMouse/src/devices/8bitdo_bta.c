// 8bitdo_bta.c
#include "8bitdo_bta.h"
#include "globals.h"

// check if device is 8BitDo Wireless Adapter (D-input)
bool is_8bitdo_bta(uint16_t vid, uint16_t pid)
{
  return ((vid == 0x2dc8 && (
    pid == 0x3100 || // 8BitDo Wireless Adapter (Red)
    pid == 0x3105 || // 8BitDo Wireless Adapter (Black) [05:HID_MODE]
    pid == 0x3106 || // 8BitDo Wireless Adapter (Black) [06:RECV_MODE]
    pid == 0x3107    // 8BitDo Wireless Adapter (Black) [07:IDLE_MODE]
  )));
}

// check if 2 reports are different enough
bool diff_report_bta(bitdo_bta_report_t const* rpt1, bitdo_bta_report_t const* rpt2)
{
  bool result;

  // x1, y1, x2, y2, rx, ry must different than 2 to be counted
  result = diff_than_n(rpt1->x1, rpt2->x1, 2) || diff_than_n(rpt1->y1, rpt2->y1, 2) ||
           diff_than_n(rpt1->x2, rpt2->x2, 2) || diff_than_n(rpt1->y2, rpt2->y2, 2) ||
           diff_than_n(rpt1->l2_trigger, rpt2->l2_trigger, 2) ||
           diff_than_n(rpt1->r2_trigger, rpt2->r2_trigger, 2);

  // check the reset with mem compare
  result |= memcmp(&rpt1->reportId + 1, &rpt2->reportId + 1, sizeof(bitdo_bta_report_t)-6);

  return result;
}

// process usb hid input reports
void process_8bitdo_bta(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
  // previous report used to compare for changes
  static bitdo_bta_report_t prev_report[5] = { 0 };

  bitdo_bta_report_t input_report;
  memcpy(&input_report, report, sizeof(input_report));

  if ( diff_report_bta(&prev_report[dev_addr-1], &input_report) )
  {
    printf("(x1, y1, x2, y2, l2, r2) = (%u, %u, %u, %u, %u, %u)\r\n",
      input_report.x1, input_report.y1,
      input_report.x2, input_report.y2,
      input_report.l2_trigger, input_report.r2_trigger
    );
    printf("DPad = %d ", input_report.dpad);

    if (input_report.a) printf("A ");
    if (input_report.b) printf("B ");
    if (input_report.r) printf("R (C) ");
    if (input_report.x) printf("X ");
    if (input_report.y) printf("Y ");
    if (input_report.l) printf("L (Z) ");
    if (input_report.l2) printf("L2 ");
    if (input_report.r2) printf("R2 ");
    if (input_report.l3) printf("L3 ");
    if (input_report.r3) printf("R3 ");
    if (input_report.cap) printf("Capture ");
    if (input_report.select) printf("Select ");
    if (input_report.start) printf("Start ");
    if (input_report.home) printf("Home ");

    printf("\r\n");
    bool dpad_up    = (input_report.dpad == 0 || input_report.dpad == 1 || input_report.dpad == 7);
    bool dpad_right = (input_report.dpad >= 1 && input_report.dpad <= 3);
    bool dpad_down  = (input_report.dpad >= 3 && input_report.dpad <= 5);
    bool dpad_left  = (input_report.dpad >= 5 && input_report.dpad <= 7);
    bool has_6btns = true;

    buttons = (((input_report.r3)     ? 0x00 : 0x20000) |
               ((input_report.l3)     ? 0x00 : 0x10000) |
               ((input_report.r)      ? 0x00 : 0x8000) |
               ((input_report.l)      ? 0x00 : 0x4000) |
               ((input_report.y)      ? 0x00 : 0x2000) |
               ((input_report.x)      ? 0x00 : 0x1000) |
               ((has_6btns)           ? 0x00 : 0x0800) |
               ((input_report.home)   ? 0x00 : 0x0400) |
               ((input_report.r2)     ? 0x00 : 0x0200) |
               ((input_report.l2)     ? 0x00 : 0x0100) |
               ((dpad_left)           ? 0x00 : 0x08) |
               ((dpad_down)           ? 0x00 : 0x04) |
               ((dpad_right)          ? 0x00 : 0x02) |
               ((dpad_up)             ? 0x00 : 0x01) |
               ((input_report.start)  ? 0x00 : 0x80) |
               ((input_report.select) ? 0x00 : 0x40) |
               ((input_report.b)      ? 0x00 : 0x20) |
               ((input_report.a)      ? 0x00 : 0x10));

    uint8_t analog_1x = input_report.x1;
    uint8_t analog_1y = (input_report.y1 == 0) ? 255 : 256 - input_report.y1;
    uint8_t analog_2x = input_report.x2;
    uint8_t analog_2y = (input_report.y2 == 0) ? 255 : 256 - input_report.y2;
    uint8_t l2_trigger = input_report.l2_trigger;
    uint8_t r2_trigger = input_report.r2_trigger;

    // keep analog within range [1-255]
    ensureAllNonZero(&analog_1x, &analog_1y, &analog_2x, &analog_2y);

    // add to accumulator and post to the state machine
    // if a scan from the host machine is ongoing, wait
    post_globals(dev_addr, instance, buttons, analog_1x, analog_1y, analog_2x, analog_2y, l2_trigger, r2_trigger, 0, 0);

    prev_report[dev_addr-1] = input_report;
  }
}

DeviceInterface bitdo_bta_interface = {
  .name = "8BitDo Wireless Adapter",
  .is_device = is_8bitdo_bta,
  .process = process_8bitdo_bta,
  .task = NULL,
  .init = NULL
};