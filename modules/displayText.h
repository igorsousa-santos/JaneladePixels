#ifndef displayText_H
#define displayText_H

#include <string.h>
#include "hardware/i2c.h"
#include "ssd1306.h"

void initDisplay(void);
void clearDisplay(void);
void displayText(char text[]);
#endif