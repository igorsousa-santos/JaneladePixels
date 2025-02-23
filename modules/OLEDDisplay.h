#ifndef OLEDDisplay_H
#define OLEDDisplay_H

#include <string.h>
#include "hardware/i2c.h"
#include "ssd1306.h"

void initDisplay(void);
void clearDisplay(void);
void displayText(char text[]);
void displayBitmap(uint8_t bitmap[]);
#endif