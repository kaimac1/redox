#pragma once

// Constants
#define HAND_LEFT 0
#define HAND_RIGHT 1

// Number of logical rows
// Each row is represented by a byte, so the number of columns is fixed at 8
#define NUM_ROWS 5

// Radio config
#define RECEIVER_ADDRESS    {0x4E,0xD0,0xBA,0x5E,0x00}
#define HAND_ADDRESS        {0x4E,0xD0,0x00,0x00,0x00}  // Last byte is set to 0/1 for left/right hand
#define RF_CHANNEL          20

typedef struct {
    uint8_t hand;
    uint8_t row;
    uint8_t cols;
} Message;
