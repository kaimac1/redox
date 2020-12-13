#include <stdint.h>
#include <avr/pgmspace.h>
#include "usb.h"

void handle_row(uint8_t hand, uint8_t row, uint8_t cols);

#define MAP(                                                                \
                                                                            \
l07,l06,l05,l04,l03,l02,l01,                    r01,r02,r03,r04,r05,r06,r07,\
l17,l16,l15,l14,l13,l12,l11,                    r11,r12,r13,r14,r15,r16,r17,\
l27,l26,l25,l24,l23,l22,l21,                    r21,r22,r23,r24,r25,r26,r27,\
l37,l36,l35,l34,l33,l32,                            r32,r33,r34,r35,r36,r37,\
l47,l46,l45,l44,l43,        l10,l20,    r20,r10,        r43,r44,r45,r46,r47,\
                                l30,    r30,                                \
                        l42,l41,l40,    r40,r41,r42                         \
) {\
    {0,        KEY_##l01,KEY_##l02,KEY_##l03,KEY_##l04,KEY_##l05,KEY_##l06,KEY_##l07,}, \
    {KEY_##l10,KEY_##l11,KEY_##l12,KEY_##l13,KEY_##l14,KEY_##l15,KEY_##l16,KEY_##l17,}, \
    {KEY_##l20,KEY_##l21,KEY_##l22,KEY_##l23,KEY_##l24,KEY_##l25,KEY_##l26,KEY_##l27,}, \
    {KEY_##l30,0,        KEY_##l32,KEY_##l33,KEY_##l34,KEY_##l35,KEY_##l36,KEY_##l37,}, \
    {KEY_##l40,KEY_##l41,KEY_##l42,KEY_##l43,KEY_##l44,KEY_##l45,KEY_##l46,KEY_##l47,}, \
                                                                                        \
    {0,        KEY_##r01,KEY_##r02,KEY_##r03,KEY_##r04,KEY_##r05,KEY_##r06,KEY_##r07,}, \
    {KEY_##r10,KEY_##r11,KEY_##r12,KEY_##r13,KEY_##r14,KEY_##r15,KEY_##r16,KEY_##r17,}, \
    {KEY_##r20,KEY_##r21,KEY_##r22,KEY_##r23,KEY_##r24,KEY_##r25,KEY_##r26,KEY_##r27,}, \
    {KEY_##r30,0,        KEY_##r32,KEY_##r33,KEY_##r34,KEY_##r35,KEY_##r36,KEY_##r37,}, \
    {KEY_##r40,KEY_##r41,KEY_##r42,KEY_##r43,KEY_##r44,KEY_##r45,KEY_##r46,KEY_##r47,}, \
}

extern const uint8_t keymap[10][8];
