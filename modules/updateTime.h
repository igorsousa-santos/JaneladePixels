#ifndef UPDATE_TIME_H
#define UPDATE_TIME_H

#define UTC_OFFSET_HOURS -3 //UTC-3 for Brazil
#define NTP_RETRY_DELAY_MS 3000
#define NTP_MAX_RETRIES 3

void setRTCTime();

#endif