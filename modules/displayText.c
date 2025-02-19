#include "displayText.h"
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

    const int CHARS_PER_LINE = 15; // 128 pixels / 8 pixels per char - 1 for readability
    const int LINE_HEIGHT = 8; // Each character is 8 pixels tall
    
    size_t len = strlen(text);
    int line = 0;
    
    for (size_t i = 0; i < len; i += CHARS_PER_LINE) {
        char line_buffer[CHARS_PER_LINE + 1];
        size_t chars_to_copy = len - i > CHARS_PER_LINE ? CHARS_PER_LINE : len - i;
        
        strncpy(line_buffer, &text[i], chars_to_copy);
        line_buffer[chars_to_copy] = '\0';
        
        ssd1306_draw_string(ssd, 5, line * LINE_HEIGHT, line_buffer);
        line++;
        
        // Prevent writing beyond display height (64 pixels = 8 lines)
        if (line >= 8) break;
    }
    
    render_on_display(ssd, &frame_area);
}