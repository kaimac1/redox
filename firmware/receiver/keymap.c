#include "keyboard.h"

const uint8_t keymap[10][8] = 
MAP(
    
ESC ,1   ,2   ,3   ,4   ,5   ,MINS,                       EQ  ,6   ,7   ,8   ,9   ,0   ,BKSP,
TAB ,Q   ,W   ,E   ,R   ,T   ,LBRA,                       RBRA,Y   ,U   ,I   ,O   ,P   ,HASH,
CAPS,A   ,S   ,D   ,F   ,G   ,DEL,                        BKSL,H   ,J   ,K   ,L   ,SMCL,QUOT,
SHFT,Z   ,X   ,C   ,V   ,B   ,                                 N   ,M   ,CMMA,STOP,UP  ,SLSH,
NONE,NONE,NONE,NONE,NONE,          TILD,NONE,   NONE,NONE,          HOME,END, LEFT,DOWN,RGHT,
                                        PGUP,   PGDN,
                              NONE,SPC ,ALT ,   CTRL,SHFT,ENTR
);
