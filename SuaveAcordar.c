#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "modules/displayText.h"

int main() {
    stdio_init_all();
    initDisplay();
    clearDisplay();

    sleep_ms(2000);

    displayText("HELLO WORLDSSSSSSSSSS");

    // Initialise Wi-Fi
    if (cyw43_arch_init()) {
        displayText("Não foi possível inicializar o Wi-Fi");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    displayText("Connecting to Wi-Fi...");
    if (cyw43_arch_wifi_connect_timeout_ms("Your Wi-Fi SSID", "Your Wi-Fi Password", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        displayText("Failed to connect.");
        return 1;
    } else {
        displayText("Connected.");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    while (true) {
        displayText("Hello, world!");
        sleep_ms(1000);
    }
}
