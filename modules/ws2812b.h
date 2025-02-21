#ifndef WS2812B_H
#define WS2812B_H

#define WS2812B_LED_COUNT 25
#define WS2812B_PIN 7
#define TWO_PI 6.28318530718

#include "hardware/pio.h"

typedef struct {
    PIO pio;
    uint sm;
    uint8_t r, g, b; // Color targets
    uint8_t r_current, g_current, b_current; // Current color
    float animation_progress;
    absolute_time_t next_update;
    uint8_t max_brightness, min_brightness;
    uint8_t update_interval_ms;
    uint32_t animation_duration_ms;
} breathing_animation_state_t;

breathing_animation_state_t initBreathingAnimation();
void playBreathingAnimation(breathing_animation_state_t *state);
void breathingAnimationSetTarget(breathing_animation_state_t *state, uint8_t r, uint8_t g, uint8_t b);

#endif