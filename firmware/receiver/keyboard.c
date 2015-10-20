#include <stdint.h>
#include "keyboard.h"
#include "usb.h"
#include "util/xprintf.h"


static void handle_key(uint8_t row, uint8_t col, uint8_t state);
static void set_usb_key(uint8_t scancode, uint8_t state);
uint8_t keystate[10];


/******************************************************************************/
// Turn the column state for the given hand and row into a series of calls to
// handle_key() to process the keys that have changed state.
void handle_row(uint8_t hand, uint8_t row, uint8_t cols) {

    // Row offset into the keystate array
    uint8_t offs = (hand == 0 ? 0 : 5);

    uint8_t change = keystate[row+offs] ^ cols;
    uint8_t i;

    // Check for changed keys on the given row
    for (i=0; i<8; i++) {
        if (change & (1<<i)) {
            handle_key(row+offs, i, cols & (1<<i));
        }
    }

    // Update the row with the current state
    keystate[row+offs] = cols;

}

/******************************************************************************/
// Convert keypresses into scancodes.
static void handle_key(uint8_t row, uint8_t col, uint8_t state) {

    set_usb_key(keymap[row][col], state);

    // Send the report
    usb_keyboard_send();

}

/******************************************************************************/
// Add the given scancode to the USB report if state is true,
// otherwise remove it.
static void set_usb_key(uint8_t scancode, uint8_t state) {

    // Handle modifier keys
    if (scancode & 0x80) {
        if (state) {
            keyboard_modifier_keys |= (scancode & 0x7F);
        } else {
            keyboard_modifier_keys &= ~(scancode & 0x7F);
        }
        return;
    }

    // If we are adding a new scancode we are looking for a blank slot to put it in.
    // If we are removing a scancode we are looking for that code to set to zero.
    uint8_t from = (state ? 0 : scancode);
    uint8_t to   = (state ? scancode : 0);
    uint8_t i;

    for (i=0; i<6; i++) {
        if (keyboard_keys[i] == from) {
            keyboard_keys[i] = to;
            xprintf("%02d %02d %02d %02d\r\n", keyboard_keys[0], keyboard_keys[1], keyboard_keys[2], keyboard_keys[3]);
            break;
        }
    }

}