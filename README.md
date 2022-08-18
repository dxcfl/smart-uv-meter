# uv-monitoring_wio-terminal
Solar ultraviolet radiation exposure monitoring with Seed Wio Terminal

# Introduction
For many years there has been a growing concern that anthropogenic damage to the Earth's stratospheric ozone layer will lead to an increase of solar ultraviolet (UV) radiation reaching the Earth's surface, with a consequent adverse impact on human health. UV-related diseases, especially skin cancer, are causing problems on public well-being around the world, as well as resulting in high and increasing healthcare costs. Ozone depletion and climate change are aggravating this situation, Temperature changes and an increasing number of cloudless, sunny days not only result in heatwaves and droughts but are also exposing humans to higher levels of solar UV radiation. The risk of developing skin cancer is increasing.

The solution implements an UV radiation exposure monitoring system to assist in the prevention of diseases caused by over exposition to solar UV radiation through warning messages with preventive regarding the monitored exposure level.

It can act either as a remote sensor unit or as personal dosimeter, continuously measuring the UV index and other environmental parameters (humidity, temperature). It can calculate the actual individual risk for individuals in the monitored area or for the person wearing it. The resulting data can be used to issue categorized advisory messages for preventive measures (e.g., apply sunscreen, wearing protective clothes, limit exposure time etc.).

The Wio Terminal connected to the appropriate sensors (UV radiation, temperature, humidity, etc.) can be used to collect and process the regarded environmental data. The display can be used to show the aggregated data, risk analysis and protection advice to the user. Additionally, aggregated data could be transmitted (e.g., via LoRaWAN) to an edge gateway device for long time data recording and evaluation.

## Components

### Hardware

* [Seeed Studio Wio Terminal](https://www.seeedstudio.com/Wio-Terminal-p-4509.html)
* [Grove - I2C UV Sensor (VEML6070)](https://wiki.seeedstudio.com/Grove-I2C_UV_Sensor-VEML6070/)
* [Grove - Barometer Sensor(BME280)](https://wiki.seeedstudio.com/Grove-Barometer_Sensor-BME280/) 
* [Grove - I2C Hub](https://wiki.seeedstudio.com/Grove-I2C_Hub/)

#### Seeed Studio Wio Terminal
![wio terminal](/assets/images/wio-terminal.jpg)
The Wio Terminal is a SAMD51-based microcontroller with Wireless Connectivity powered by Realtek RTL8720DN that’s compatible with Arduino and MicroPython. Currently, wireless connectivity is only supported by Arduino. It runs at 120MHz (Boost up to 200MHz), 4MB External Flash and 192KB RAM. It supports both Bluetooth and Wi-Fi providing backbone for IoT projects. The Wio Terminal itself is equipped with a 2.4” LCD Screen, onboard IMU(LIS3DHTR), Microphone, Buzzer, microSD card slot, Light sensor, and Infrared Emitter(IR 940nm). On top of that, it also has two multifunctional Grove ports for Grove Ecosystem and 40 Raspberry pi compatible pin GPIO for more add-ons.

Features:
* Highly Integrated Design: MCU, LCD, WIFI, BT, IMU, Microphone, Speaker, microSD Card, Light Sensor, 5-Way Switch, Infrared Emitter (IR 940nm), Crypto-authentication Ready
* Powered by Microchip ATSAMD51P19: ARM Cortex-M4F core running at 120MHz(Boost up to 200MHz) and 4 MB External Flash, 192 KB RAM
* Comprehensive Protocol Support: SPI, I2C, I2S, ADC, DAC, PWM, UART(Serial)
* Powerful Wireless Connectivity (supported only by Arduino)
powered by Realtek RTL8720DN: Dual Band 2.4Ghz / 5Ghz Wi-Fi (802.11 a/b/g/n) and BLE / BLE 5.0
* USB OTG Support: USB Host /USB Client
* Grove Ecosystem
* Software Support: Arduino, MicroPython, ArduPy and AT Firmware

For more details see the "[Get Started with Wio Terminal](https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/)" guide.

#### Grove - I2C UV Sensor (VEML6070)
![veml6070](/assets/images/veml6070.jpg)
The Grove - I2C UV Sensor(VEML6070) is an advanced ultraviolet (UV) light sensor with I2C protocol interface. Ultraviolet (UV) is electromagnetic radiation with a wavelength from 10 nm to 400 nm, shorter than that of visible light but longer than X-rays, this sensor detects 320-410nm light most effectively, and will converts solar UV light intensity to digital data. See wiki article "[Grove - I2C UV Sensor (VEML6070)](https://wiki.seeedstudio.com/Grove-I2C_UV_Sensor-VEML6070/)" for mor details.

#### Grove - Barometer Sensor(BME280)
![bme280](/assets/images/bme280.jpg)
Grove - Temp&Humi&Barometer Sensor (BME280) is a breakout board for Bosch BMP280 high-precision, low-power combined humidity, pressure, and temperature sensor. This module can be used to measure temperature, atmospheric pressure and humidity accurately and fast. See wiki article "[Grove - Barometer Sensor(BME280)](https://wiki.seeedstudio.com/Grove-Barometer_Sensor-BME280/)" for mor details.

### Software / Libraries / Services

* [Arduino IDE](https://www.arduino.cc/en/software)
* [LvGL Graphics Library for Wio Terminal](https://github.com/Seeed-Studio/Seeed_Arduino_LvGL)
* [Adafruit VEML6070 Library](https://github.com/adafruit/Adafruit_VEML6070)
* [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library)

#### LvGL Graphics Library for Wio Terminal

The LvGL (Light and Versatile Graphics Library) is an open-source graphics library providing everything you need to create embedded GUI with easy-to-use graphical elements, beautiful visual effects and low memory footprint. For instructions on how to use LvGL for Wio Terminal refer to this [guide](https://wiki.seeedstudio.com/Wio-Terminal-LVGL/).

### Additional components

* Wires

## Wiring

For Grove sensor boards, just connect both sensor (via hub) to the Grove I2C Port on Wio Terminal, for other sensor boards connect VIN,GND,SCL and SDA accordingly:
![wiring](/assets/images/wiring_diagramm.png)

![wiring](/assets/images/wiring_photo.jpg)


## Software

### UV Index & Risk Level

Ultraviolet (UV) "light" is a form of electromagnetic radiation with wavelength from 200 nm to 400 nm, shorter than that of visible light (400 nm to 750 nm), but longer than X-rays. UV radiation is present in sunlight and constitutes about 10% of the total electromagnetic radiation output from the Sun. Short-wave ultraviolet light can damage DNA and sterilize surfaces with which it comes into contact. For humans, suntan and sunburn are familiar effects of exposure of the skin to UV light, along with an increased risk of skin cancer.
UV radiation is divided into three bands of wavelength:

* UVA (315-400 nm)
* UVB (280-315 nm)
* UVC (100-280 nm).

Through absorption of the earth's atmosphere in the ozone layer, the UVC spectrum is completely blocked and the solar radiation in the UVB spectrum does barely reach the earth's surface. The less dangerous UVA radiation is far less absorbed by the atmosphere. UVA radiation is less powerful than UVB radiation, but highly penetrating. It can reach the skin and is responsible for photoaging and the onset of different forms of skin cancer 

UV radiation intensity is measured in micro-watts per square-centimeter (μW / cm²). The VEML6070 sensor measures radiation in a spectrum from  300 nm to  400 nm, so it can only detect UVA radiation.

In order to estimate the energy behind UV radiation and the risk level associated with it, the UV index was established. It is an international standard measurement of the strength of the sunburn-producing UV radiation at a particular place and time and it is primarily used in daily and hourly forecasts aimed at the general public. The UV index value appears on a linear scale of 0 to ≥ 11. 

![uvi scale](/assets/images/uviscale.jpg)

Since deriving the UV index directly from the irradiance measured by the sensor requires a quite complex calculation, weighted according to a curve and integrated over the whole spectrum, the [Designing the VEML6070 UV Light Sensor
Into Applications](https://www.vishay.com/docs/84310/designingveml6070.pdf) guide by Vishay Semiconductors proposes to estimate the energy behind UV radiation and the risk level associated with it, by simply reading out the irradiance value from the VEML6070 sensor and comparing it with pre-defined values.

This leads to the following function to derive UV index and risk level from the VEML6070 readings:

```
*
 * uv_index()
 *
 * expects integration time and the measurement value from the UV sensor as input
 * and returns the corresponding risk level:
 * 0: Low
 * 1: Moderate
 * 2: High
 * 3: Very High
 * 4: Extreme
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

    if (uv <= uv_high)
    {
      double r = (double)(uv_high - uv) / (double)(uv_high - uv_low);    // relative in range (1-r)
      double d_uvi = index_high[i] - r * (index_high[i] - index_low[i]); // map to UVI range
      *uvi = round(d_uvi);
      break;
    }
  }

  return i;
}
```


### GUI

The enviromental measurements obtained by the two sensors 
* Temperature
* Humidity
* Pressure
* UV irradiance
* and the estimated UV index.
are displayed as numerical values.

The derrived risk level is displayed as a line meter, changing its background color accordingly.

![screen](/assets/images/screen.JPG)

## Test

On a very sunny (and warm) Sunday afternoon tests have been performed with the setting RSET = 270 kΩ; IT = 2T.

![test](/assets/images/test1.jpg)

In the browser window the official readings (UV index 5) from a nearby ( ~ 30 km) weather station published by the "Office for Radiation Protection" is shown. The UV index shown on the devices was changing between 5 and 6:

![test](/assets/images/test2.jpg)
