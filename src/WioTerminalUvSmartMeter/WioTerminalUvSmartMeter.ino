
/*
  WioTerminalUvSmartMeter.ino

    Read more: https://https://github.com/dxcfl/uv-monitoring_wio-terminal#readme
    Git: https://github.com/dxcfl/uv-monitoring_wio-terminal
    (c) 2022 by dxcfl
*/

// #define DEBUG 1
#include "debug2serial.h"

/* GUI
*/

#include "GUI.cpp"

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{
  DEBUG_SERIAL_PRINTF("%s@%d->%s\r\n", file, line, dsc);
  delay(100);
}
#endif

/* INCLUDES
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML6070.h>
#include <math.h>

/* GLOBALS
*/

// Sensors
Adafruit_BME280 bme280;
Adafruit_VEML6070 veml6070 = Adafruit_VEML6070();

// Miscellaneous

#define CYCLE_TIME 1000 // ms

#define CYCLE_TIME_TELEMETRY 60000 // ms


#define SEALEVELPRESSURE_HPA (1013.25) // For altitude meassuerment with the BME280

/*
  VEML6070:

  There are 4 different 'integration' times used to calculate the intensity.
  The longer the integration time, the more light is collected.
  Use shorter integration times if you want to get measurements quickly with less precision.
  Longer times will give you more precision but of course, take longer!

    VEML6070_HALF_T ~62.5ms
    VEML6070_1_T ~125ms
    VEML6070_2_T ~250ms
    VEML6070_4_T ~500ms
*/
#define VEML6070_INTEGRATION_TIME VEML6070_4_T

uint32_t updateTime = 0; // time for next update
uint32_t telemetryTime = 0; // time for next update

/* SETUP
*/

void setup()
{
  DEBUG_SERIAL_INIT(115200);
  DEBUG_SERIAL_PRINTLN("SETUP: Initialization ...");

  DEBUG_SERIAL_PRINTLN("SETUP: GUI ...");
  GUI::setup();

#if USE_LV_LOG != 0
  lv_log_register_print(my_print); /* register print function for debugging */
#endif

  GUI::screen();
  DEBUG_SERIAL_PRINTLN("SETUP: GUI ready.");

  DEBUG_SERIAL_PRINTLN("SETUP: BME280 ...");
  unsigned status;
  status = bme280.begin(0x76);
  if (!status)
  {
    DEBUG_SERIAL_PRINTLN("SETUP: Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    DEBUG_SERIAL_PRINT("SETUP: SensorID was: 0x");
    Serial.println(bme280.sensorID(), 16);
    DEBUG_SERIAL_PRINT("SETUP:         ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    DEBUG_SERIAL_PRINT("SETUP:    ID of 0x56-0x58 represents a BMP 280,\n");
    DEBUG_SERIAL_PRINT("SETUP:         ID of 0x64 represents a BME 280.\n");
    DEBUG_SERIAL_PRINT("SETUP:         ID of 0x61 represents a BME 680.\n");
    while (1)
      delay(10);
  }
  DEBUG_SERIAL_PRINTLN("SETUP: BME280 initialized.");

  DEBUG_SERIAL_PRINTLN("SETUP: VEML6070 ...");
  veml6070.begin(VEML6070_INTEGRATION_TIME); // pass in the integration time constant
  DEBUG_SERIAL_PRINTLN("SETUP: VEML6070 initialized.");

  DEBUG_SERIAL_PRINT("SETUP: Cycle time (ms) = ");
  DEBUG_SERIAL_PRINT(CYCLE_TIME);

  DEBUG_SERIAL_PRINT("SETUP: Telemetry cycle time (ms) = ");
  DEBUG_SERIAL_PRINT(CYCLE_TIME_TELEMETRY); 

  DEBUG_SERIAL_PRINTLN("SETUP: Telemetry ...");
  telemetrySetup();

  DEBUG_SERIAL_PRINTLN("SETUP: initialized.");
}

void loop()
{
  float temperature, pressure, altitude, humidity;
  unsigned short uv, uvi = 0;
  unsigned short risk = 0;

  if (updateTime <= millis())
  {
    updateTime = millis() + CYCLE_TIME; // time for next update

    DEBUG_SERIAL_PRINTLN("LOOP: BME280: Reading values  ...");
    temperature = bme280.readTemperature();
    pressure = bme280.readPressure() / 100.0F;
    altitude = bme280.readAltitude(SEALEVELPRESSURE_HPA);
    humidity = bme280.readHumidity();
    DEBUG_SERIAL_PRINTF("LOOP: BME280: Temperature = %.2f *C, Pressure = %.2f hPA, Humidity = %.2f %%", temperature, pressure, humidity);
    DEBUG_SERIAL_PRINTLN("");
    DEBUG_SERIAL_PRINTLN("LOOP: VEML6070: Reading values  ...");
    uv = veml6070.readUV();
    DEBUG_SERIAL_PRINTF("LOOP: VEML6070: UV reading  = %d\n", uv);

    DEBUG_SERIAL_PRINTLN("LOOP: Calculating UVI and risk level ...");
    risk = uv_index(VEML6070_INTEGRATION_TIME, uv, &uvi);
    DEBUG_SERIAL_PRINTF("LOOP: UVI = %d, Risk level = %d\n", uvi, risk);

    DEBUG_SERIAL_PRINTLN("LOOP: GUI: Updating display  ...");
    GUI::update_labels(temperature, pressure, humidity, uv, uvi);
    GUI::update_meter(risk);

    if (telemetryTime <= millis())
    {
      telemetryTime = millis() + CYCLE_TIME_TELEMETRY; // time for next update
      telemetrySendValues(temperature, humidity, uv, uvi);
    }
  }

#ifdef DEBUG_VERBOSE
  DEBUG_SERIAL_PRINTLN("LOOP: GUI: Updating ...");
#endif
  GUI::update();
}

/*
   uv_index()

   expects integration time and the measurement value from the UV sensor as input
   and returns the corresponding risk level:
   0: Low
   1: Moderate
   2: High
   3: Very High
   4: Extreme
*/
unsigned short uv_index(veml6070_integrationtime_t it, unsigned uv, unsigned short *uvi)
{
  /*
    See https://www.vishay.com/docs/84310/designingveml6070.pdf , Table 3:
    Together with the RSET value of 270 kΩ and the integration time 1T,
    the parameters below map UV light data values to  UV index (risk level)
    and UVI values.
  */
  const unsigned uv_high_1t[4] = {560, 1120, 1494, 2054};
  const unsigned index_low[4] = {0, 3, 6, 8};
  const unsigned index_high[4] = {2, 5, 7, 10};

  unsigned k = 0;
  switch (it)
  {
    case VEML6070_HALF_T:
    case VEML6070_1_T:
      k = 1;
      break;
    case VEML6070_2_T:
      k = 2;
      break;
    case VEML6070_4_T:
      k = 3;
      break;
  }

  *uvi = 11; // if above any range

  int i = 0;
  for (i = 0; i < 4; i++)
  {
    unsigned uv_high = uv_high_1t[i] * k;
    unsigned uv_low = i < 1 ? 0 : uv_high_1t[i - 1] * k + 1;

    DEBUG_SERIAL_PRINTF("UV_INDEX: %d : UV = %d uw/cm^2 in [%d,%d] (UVI [%d,%d])?", i, uv, uv_low, uv_high, index_low[i], index_high[i]);
    DEBUG_SERIAL_PRINTLN("");
    if (uv <= uv_high)
    {
      DEBUG_SERIAL_PRINTF("UV_INDEX: UV index is %d ... ", i);
      double r = (double)(uv_high - uv) / (double)(uv_high - uv_low);    // relative in range (1-r)
      double d_uvi = index_high[i] - r * (index_high[i] - index_low[i]); // map to UVI range
      *uvi = round(d_uvi);
      DEBUG_SERIAL_PRINTF("  (UVI = %.3f , relative in range = %.3f)  ", d_uvi, 1 - r);
      DEBUG_SERIAL_PRINTF("UVI = %d", *uvi);
      DEBUG_SERIAL_PRINTLN(".");
      break;
    }
  }

  return i;
}
