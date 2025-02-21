#include "updateTime.h"
#include <stdio.h>
#include <time.h>
#include "lwip/apps/sntp.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"

// Indicates if time has been successfully updated
volatile int sntp_time_updated = 0;
// NTP timestamp
volatile time_t ntp_time = 0;

// SNTP callback to set current ntp_time
void setSystemTimeCallback(unsigned int sec) {
    if (sec == 0) {
        // Received an invalid time; do not update the RTC.
        printf("SNTP: Received invalid time (0 seconds).\n");
        sntp_time_updated = 0;
        return;
    } else {
        sntp_time_updated = 1;
        ntp_time = sec;
    }
}

// Set the RTC time using SNTP
void setRTCTime() {

    // Configure and start the SNTP client.
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();

    // Retry SNTP update if it fails
    int retries = 0;
    while (!sntp_time_updated && retries < NTP_MAX_RETRIES) {
        sleep_ms(NTP_RETRY_DELAY_MS);
        if (!sntp_time_updated) {
            printf("SNTP update failed, retrying\n");
            // Reinitialize SNTP to try again.
            sntp_init();
        }
        retries++;
    }

    // Prepare Pico RTC datetime_t structure.
    datetime_t dt = {0};

    if (sntp_time_updated) {
        ntp_time += (UTC_OFFSET_HOURS * 3600);
        const time_t current_time = ntp_time;
        struct tm *utc = gmtime(&current_time);
        printf("Local time at GMT %d: %02d/%02d/%04d %02d:%02d:%02d\n", 
               UTC_OFFSET_HOURS, 
               utc->tm_mday, utc->tm_mon + 1, utc->tm_year + 1900,
               utc->tm_hour, utc->tm_min, utc->tm_sec);
               
        dt.year = utc->tm_year + 1900;
        dt.month = utc->tm_mon + 1;
        dt.day = utc->tm_mday;
        dt.dotw = utc->tm_wday;
        dt.hour = utc->tm_hour;
        dt.min = utc->tm_min;
        dt.sec = utc->tm_sec;
    } else {
        printf("NTP failed. Using default time\n");
        dt.year = 2024;
        dt.month = 3;
        dt.day = 15;
        dt.dotw = 5;
        dt.hour = 12;
        dt.min = 0;
        dt.sec = 0;
    }

    // Initialize RTC and set the time
    rtc_init();
    if (!rtc_set_datetime(&dt)) {
        printf("Failed to set RTC time\n");
    }

    sntp_stop();
}