/*
  Telemetry.ino

    Read more: https://https://github.com/dxcfl/uv-monitoring_wio-terminal#readme
    Git: https://github.com/dxcfl/uv-monitoring_wio-terminal
    (c) 2022 by dxcfl
*/

#if defined(TELEMETRY) && defined(TELEMETRY_LORAWAN_HELIUM)

#include <Arduino.h>
#include "disk91_LoRaE5.h"

Disk91_LoRaE5 lorae5(&Serial); // Where the AT command and debut traces are printed

#define Frequency DSKLORAE5_ZONE_EU868
/*
Select your frequency band here.
DSKLORAE5_ZONE_EU868
DSKLORAE5_ZONE_US915
DSKLORAE5_ZONE_AS923_1
DSKLORAE5_ZONE_AS923_2
DSKLORAE5_ZONE_AS923_3
DSKLORAE5_ZONE_AS923_4
DSKLORAE5_ZONE_KR920
DSKLORAE5_ZONE_IN865
DSKLORAE5_ZONE_AU915
 */

char deveui[] = "6081F97A6178DA10";
char appeui[] = "6081F96DD5894DAA";
char appkey[] = "77D92AAE47B8B686B41707A3E9301CA3";

void data_encode(int val_1, int val_2, int val_3, int val_4, uint8_t data[8])
{
    int val[] = {val_1, val_2, val_3, val_4};

    for (int i = 0, j = 0; i < 4; i++, j += 2)
    {
        if (val[i] < 0)
        {
            val[i] = ~val[i] + 1;
            data[j] = val[i] >> 8 | 0x80;
            data[j + 1] = val[i] & 0xFF;
        }
        else
        {
            data[j] = val[i] >> 8 & 0xFF;
            data[j + 1] = val[i] & 0xFF;
        }
    }
}

void loraSetup(void)
{
    DEBUG_SERIAL_PRINTLN("TELEMETRY: Initializing LoRa E5 ...");
    // init the library, search the LORAE5 over the different WIO port available
    if (!lorae5.begin(DSKLORAE5_SEARCH_WIO))
    {
        DEBUG_SERIAL_PRINTLN("LORA: LoRa E5 Init Failed");
        while (1)
            ;
    }

    // Setup the LoRaWan Credentials
    if (!lorae5.setup(
            Frequency,
            deveui,
            appeui,
            appkey))
    {
        DEBUG_SERIAL_PRINTLN("LORA: LoRa E5 Setup Failed");
        while (1)
            ;
    }
}

void telemetrySetup()
{
    DEBUG_SERIAL_PRINTLN("TELEMETRY: Initializing LoRaWAN connection ...");
    loraSetup();
}

void telemetrySendValues(float temperature, float humidity, unsigned uv, unsigned short uvi)
{

    int int_temperature = temperature * 100;
    int int_humidity = humidity * 100;
    static uint8_t data[8] = {0x00}; // Use the data[] to store the values of the sensors

#ifdef TELEMETRY_REPORT_UVI_CHANGES_ONLY
    static unsigned short previous_uvi = -1;
    if (previous_uvi == uvi)
    {
        DEBUG_SERIAL_PRINTLN("TELEMETRY: UVI not chhanged since last transmission - skipping ...");
        return;
    }
    else
    {
        previous_uvi = uvi;
    }
#endif

    DEBUG_SERIAL_PRINTLN("TELEMETRY: Encoding data ...");
    data_encode(int_temperature, int_humidity, uv, uvi, data);

    DEBUG_SERIAL_PRINTLN("TELEMETRY: Sending data ...");
    if (lorae5.send_sync( // Sending the sensor values out
            8,            // LoRaWan Port
            data,         // data array
            sizeof(data), // size of the data
            false,        // we are not expecting a ack
            7,            // Spread Factor
            14            // Tx Power in dBm
            ))
    {
        DEBUG_SERIAL_PRINTLN("LORA: Uplink done");
        if (lorae5.isDownlinkReceived())
        {
            DEBUG_SERIAL_PRINTLN("LORA: A downlink has been received");
            if (lorae5.isDownlinkPending())
            {
                DEBUG_SERIAL_PRINTLN("LORA: More downlink are pending");
            }
        }
        DEBUG_SERIAL_PRINTLN("TELEMETRY: Data sent.");
    }
    else
    {
        DEBUG_SERIAL_PRINTLN("TELEMETRY: Send data failed.");
    }
}

#endif // TELEMETRY_LORAWAN_HELIUM
