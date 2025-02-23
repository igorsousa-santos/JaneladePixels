#include "OLEDDisplay.h"
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "ssd1306.h"

static struct render_area frame_area;

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

void initDisplay(void) {
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    frame_area = (struct render_area){.start_column = 0,
                                      .end_column = ssd1306_width - 1,
                                      .start_page = 0,
                                      .end_page = ssd1306_n_pages - 1};

    calculate_render_area_buffer_length(&frame_area);
}

void clearDisplay() {
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

void displayText(char text[]) {
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    const int CHARS_PER_LINE = 15;
    const int LINE_HEIGHT = 8;
    
    size_t len = strlen(text);
    int line = 0;
    size_t current_pos = 0;
    
    while (current_pos < len && line < 8) {
        char line_buffer[CHARS_PER_LINE + 1];
        size_t chars_in_line = 0;
        
        // Process characters until newline or CHARS_PER_LINE is reached
        while (current_pos < len && chars_in_line < CHARS_PER_LINE) {
            if (text[current_pos] == '\n') {
                current_pos++;
                break;
            }
            line_buffer[chars_in_line++] = text[current_pos++];
        }
        
        line_buffer[chars_in_line] = '\0';
        
        // Draw the line
        ssd1306_draw_string(ssd, 5, line * LINE_HEIGHT, line_buffer);
        line++;
        
        // Break if we've reached display height limit
        if (line >= 8) break;
    }
    
    render_on_display(ssd, &frame_area);
}

void displayBitmap(uint8_t *bitmap) {
    render_on_display(bitmap, &frame_area);
}