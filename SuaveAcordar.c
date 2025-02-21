#include <stdio.h>
#include "credentials.h"
#include "hardware/rtc.h"

#include "modules/displayText.h"
#include "modules/updateTime.h"
#include "modules/ws2812b.h"

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

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
        snprintf(buffer, sizeof(buffer), "SSID:\n %s", WIFI_SSID);
        displayText(buffer);
        if (cyw43_arch_wifi_connect_timeout_ms(
                WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0) {
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

int main() {
    stdio_init_all();
    initDisplay();
    clearDisplay();

    sleep_ms(2000);

    setPattern();

    displayText("Conectando");

    if (!connectToWifi()) {
        displayText("WiFi nao conectado");
        sleep_ms(1000);
        displayText("Usando horario padrao");
    }
    printf("Updating time!\n");
    displayText("Atualizando\n horario");
    setRTCTime();

    datetime_t t;
    char datetime_str[20];

    while (true) {
        // Get current time from RTC
        rtc_get_datetime(&t);

        // Format time as string
        snprintf(datetime_str, sizeof(datetime_str),
                 "%02d/%02d/%d\n%02d:%02d:%02d", t.day, t.month, t.year, t.hour,
                 t.min, t.sec);
        // Display the time
        displayText(datetime_str);

        sleep_ms(1000);
        tight_loop_contents();
    }
}
