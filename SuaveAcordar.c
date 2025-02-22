#include <stdio.h>
#include <stdbool.h>
#include "credentials.h"
#include "hardware/rtc.h"

#include "modules/displayText.h"
#include "modules/updateTime.h"
#include "modules/ws2812b.h"

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "images/moon.h"
#include "images/stars.h"
#include "images/sun.h"

#define BITDOGLAB_BUTTON_A 5
#define BITDOGLAB_BUTTON_B 6

int connectToWifi() {
    // Initialise Wi-Fi
    if (cyw43_arch_init()) {
        displayText("Não foi possível inicializar o Wi-Fi");
        return 0;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    const int MAX_RETRIES = 3;
    int retry_count = 0;
    while (retry_count < MAX_RETRIES) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Conectando!\n SSID:\n %s", WIFI_SSID);
        displayText(buffer);
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD,
                                               CYW43_AUTH_WPA2_AES_PSK,
                                               30000) == 0) {
            // Read the ip address in a human readable way
            uint8_t* ip_address =
                (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
            printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1],
                   ip_address[2], ip_address[3]);
            return 1;
        }
        retry_count++;
        if (retry_count < MAX_RETRIES) {
            printf("Failed to connect to Wi-Fi, retrying...\n");
            sleep_ms(2000);  // Wait 2 seconds before retrying
        }
    }
    return 0;
}

typedef struct {
    uint8_t start_hour;
    uint8_t end_hour;
    uint8_t r, g, b;
} TimeColorMapping;

void setLedColorBasedOnTime(uint8_t hour, breathing_animation_state_t *anim) {
    static const TimeColorMapping color_map[] = {
        // Start hour, end hour, R, G, B
        {22,  6,   0,   0, 0},      // Late night       #000000
        { 6,  7, 255, 255, 255},    // Early morning    #ffffff
        { 7,  9, 135, 215, 250},    // Morning          #87d7fa
        { 9, 11, 235, 255, 161},    // Late Morning     #ebffa1
        {11, 13, 255, 240,  77},    // Noon             #fff04d
        {13, 15,  41, 141, 255},    // Early afternoon  #298dff
        {15, 17, 255, 159,  41},    // Late afternoon   #ff9f29
        {17, 18,  66,  52, 250},    // Early evening    #4234fa
        {18, 20,  51,  20, 250},    // Late evening     #3314fa
        {20, 22,  14,   5, 128},    // Night            #0e0580
    };

    const int mappings = sizeof(color_map) / sizeof(color_map[0]);

    for (int i = 0; i < mappings; i++) {
        if (hour >= color_map[i].start_hour && hour < color_map[i].end_hour) {
            breathingAnimationSetTarget(anim, color_map[i].r, color_map[i].g,
                                        color_map[i].b);
            return;
        }
    }

    // Default values if no range matches
    breathingAnimationSetTarget(anim, 0, 0, 0);
}

void setDisplayBasedOnTime(uint8_t hour) {
    if (hour >= 6 && hour < 18) {
        displayBitmap(sun_bits);
    } else if (hour >= 18 && hour < 22) {
        displayBitmap(moon_bits);
    } else {
        displayBitmap(stars_bits);
    }
}

bool isButtonPressed(uint gpio) {
    // One entry for each button
    static bool buttonPrevState[2] = { false, false };
    static uint64_t lastPressTime[2] = { 0, 0 };
    const uint64_t debounceDelayMs = 50;

    // Map gpio number to index (0 for Button A, 1 for Button B) kinda hacky but it works ¯\_(ツ)_/¯
    int index = gpio - BITDOGLAB_BUTTON_A;  
    
    uint64_t now = to_ms_since_boot(get_absolute_time());
    bool currentlyPressed = (gpio_get(gpio) == 0);

    // Only register a press if the button state changes from not pressed to pressed and debounce elapsed
    if (currentlyPressed && !buttonPrevState[index] && (now - lastPressTime[index] > debounceDelayMs)) {
        buttonPrevState[index] = true;
        lastPressTime[index] = now;
        return true;
    }
    if (!currentlyPressed) {
        buttonPrevState[index] = false;
    }
    return false;
}

void initButtons() {
    gpio_init(BITDOGLAB_BUTTON_A);
    gpio_set_dir(BITDOGLAB_BUTTON_A, GPIO_IN);
    gpio_pull_up(BITDOGLAB_BUTTON_A);

    gpio_init(BITDOGLAB_BUTTON_B);
    gpio_set_dir(BITDOGLAB_BUTTON_B, GPIO_IN);
    gpio_pull_up(BITDOGLAB_BUTTON_B);

}

int main() {
    
    stdio_init_all();
    initDisplay();
    clearDisplay();
    
    initButtons();
    uint8_t isAnimationPlaying = 1;

    // Short delay to wait for serial connection
    sleep_ms(2000);

    displayText("Conectando");

    if (!connectToWifi()) {
        displayText("WiFi nao conectado");
        sleep_ms(1000);
        displayText("Usando horario padrao");
    }
    printf("Updating time!\n");
    displayText("Atualizando\n horario");
    setRTCTime();

    // Disable WiFi after updating the time
    cyw43_arch_deinit();

    datetime_t t;
    char datetime_str[20];
    breathing_animation_state_t anim = initBreathingAnimation();
    breathingAnimationSetTarget(&anim, 255, 1, 128);

    uint8_t test_hour = 0;
    absolute_time_t next_hour_change = make_timeout_time_ms(1000);

    while (true) {
        // Get current time from RTC
        rtc_get_datetime(&t);

        if(isAnimationPlaying) {
            setLedColorBasedOnTime(t.hour, &anim);
            setDisplayBasedOnTime(t.hour);
        } else {
            clearDisplay();
            breathingAnimationSetTarget(&anim, 0,0,0);
        }

        if(isButtonPressed(BITDOGLAB_BUTTON_A) || isButtonPressed(BITDOGLAB_BUTTON_B)) {
            isAnimationPlaying = !isAnimationPlaying;
        }

        // Poll animation loop
        playBreathingAnimation(&anim);

        // Short sleep to reduce CPU usage
        sleep_ms(1);
    }
}
