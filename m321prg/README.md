# t02+t04+t08 mix test

ざっくり書くと、  
センサで温度湿度を収集しつつDisplayで表示しつつ、EthernetでUECSパケットを送出するプログラム

UECSのデータ送信CCMと機器動作状態CCMを送信する。SCANには応答しない。

  - InAirTemp.mIC
  - InAirHumid.mIC
  - cnd.mIC


## Hardware install

### LCD

  SC1602 (16x2)

  - I2C Address = 0x27
  - Vcc
  - GND
  - SDA=#A4
  - SCL=#A5

### Ethernet Adapter

 W5500 series

  - Vcc
  - GND
  - RST=RST
  - MISO=#12
  - MOSI=#11
  - SCS=#10
  - SCLK=#13

### Thermal and Humidity Sensor

  SHT2x series

  - I2C Address = 0x40
  - Vcc
  - GND
  - SDA=#A4
  - SCL=#A5
  

## How to compile and software install

    arduino-cli compile -b arduino:avr:nano:cpu=atmega328old t0e
    arduino-cli upload -p /dev/ttyUSB0 -b arduino:avr:nano:cpu=atmega328old t0e

## EEPROM Layout

Arduino NANO(328)は、1024bytesのEEPROMを内蔵している。

| Address | Data | Remarks  |
|:-------:|:----:|:-----:|
|  0x00   | 0x10 | Fixed |
|  0x01   | 0x0c | Fixed |
|  0x02   | XX   | Don't care |
|   :     | :    |    :   :   |
|  0x0f   | XX   | Don't care |
|  0x10<br>0x11   | 0xM0<br>0xM1 | MAC Address |
