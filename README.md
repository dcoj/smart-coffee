## Smart - Coffee Software - ESP32 Version
Smart-Coffee software for the ESP32. If you want to experiment with using the software, you can purchase the software as a stand-alone item.

Please be aware that interfacing the various sensors to the ESP32 may require level shifters, voltage dividers, and pullup/down resistors. So if you are up for a challenge, this may be for you. Alternatively, you can purchase a Smart Coffee PCB kit from the Schematix Store which will make the installation much easier and cleaner

### Compatible sensors:
You can also find a list of hardware and sensors with purchasing links on the Smart – Coffee forum page by CLICK HERE

### Bug reporting:
If you find a bug in the code or have any hardware issues with the Smart Coffee PCB, please send an email to mail@schematix.co.nz

### Have a suggestion?
If there is a feature you'd like to see added to Smart - Coffee, let us know. You might be fortunate and see it in a future software update.

## Default pinout:

### Master ESP32:
| GPIO Pin | Function                                           |
|----------|----------------------------------------------------|
| GPIO13   | RELAY 3 (relay control output)                    |
| GPIO14   | RELAY 1 (relay control output)                    |
| GPIO16   | UART RX (Connected to slave UART TX pin)          |
| GPIO17   | UART TX (Connected to slave UART RX pin)          |
| GPIO19   | RELAY 2 (relay control output)                    |
| GPIO21   | SDA (to OLED display)                             |
| GPIO22   | SCL (to OLED display)                             |
| GPIO25   | HX711 DOUT input (Loadcell amplifier) or Flow meter 1 signal input |
| GPIO26   | HX711 SCK input (Loadcell amplifier) or Flow meter 2 signal input |
| GPIO27   | PUMP (relay control output)                       |
| GPIO32   | ELEMENT 1 (relay control output)                  |
| GPIO33   | RELAY 4 (relay control output)                    |
| GPIO34   | PRESSURE TRANSDUCER 3 (0~3.3VDC input signal)     |
| GPIO35   | PRESSURE TRANSDUCER 1 (0~3.3VDC input signal)     |
| GPIO36   | BREW/PUMP SWITCH (connect to GND to activate switch) |
| GPIO39   | ESPRESSO SWITCH (connect to GND to activate switch) |

### SLAVE ESP32:

| GPIO Pin | Function                                                                                 |
|----------|-----------------------------------------------------------------------------------------|
| GPIO14   | TEMP 2 CS (CS pin from MAX31865)                                                        |
| GPIO13   | RESERVOIR (Touch sensitive probe/float switch for water level detection)                |
| GPIO16   | UART TX (Connected to master UART RX pin)                                               |
| GPIO17   | UART RX (Connected to master UART TX pin)                                               |
| GPIO18   | TEMP CLK (connected to CLK pins from MAX31865 temp sensors)                             |
| GPIO19   | TEMP SDI (connected to SDI pins from MAX31865 temp sensors)                             |
| GPIO21   | TEMP 1 CS (CS pin from MAX31865)                                                        |
| GPIO23   | TEMP SDO (connected to SDO pins from MAX31865 temp sensors)                             |
| GPIO25   | TRIGGER (to PWM ultrasonic distance sensor)                                             |
| GPIO26   | ECHO (to PWM ultrasonic distance sensor)                                                |
| GPIO27   | ELEMENT 2 (relay control output)                                                        |
| GPIO32   | PROBE 1 (Boiler 1 touch sensitive probe for water level detection)                      |
| GPIO33   | PROBE 2 (Boiler 2 touch sensitive probe for water level detection)                      |
| GPIO35   | PRESSURE TRANSDUCER 2 (0~3.3VDC input signal)                                           |

## Sensors and Hardware:
## Sensors and Hardware:

### ESP32:
- **Model**: 30pin Development Board  
- **Quantity required**: 2  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DFc08KN)  

### Pressure Transducer:
- **Max voltage input**: 5V  
- **Recommended signal range**: 0.5 to 4.5V  
- **Recommended pressure rating**: 200 kPa (0.2mPa)  
  *(Pressure rating should be at least 20% higher than your running pressure)*  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DkQsvxd)  
- **Optional adapters**:  
  - G1/4 to 1/4 BSP thread adapter (Type 2): [Aliexpress](https://s.click.aliexpress.com/e/_DdueKDl)  
  - Right angle adapter: [Aliexpress](https://s.click.aliexpress.com/e/_DE8J34X)  

### Display Screen:
- **Model**: OLED SH1107  
- **Interface**: I2C  
- **Pixels**: 128x128  
- **Input power**: 5V  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DlH1C43)  

### Load Cell:
- **Weight rating**: 1kg  
- **Wiring**: 4-wire  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DdDwihl)  

### 4 Pin Pogo Connector:
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DB2fN7N)  

### Small Size Flow Meter:
- **Model**: USN-HS06P  
- **Voltage**: 5V  
- **Barb size**: 5mm OD  
- **Flow range**: 30 ~ 150ml/min  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DCCjQh5)  

### Medium Size Flow Meter:
- **Model**: USN-HS06PS  
- **Voltage**: 5V  
- **Barb size**: 7mm OD  
- **Flow range**: 0.1 ~ 1.5L/min  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DFVTRbV)  

### High-Pressure Gicar Flow Meter:
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_Dlr1vgB)  

### Thermocouple:
- **Compatible wiring**: 2/3/4 wire  
- **Type**: PT100 and PT1000 compatible  
- **Screw mount type** purchasing link: [Aliexpress](https://s.click.aliexpress.com/e/_DkV2nuB)  
- **Threaded mount type** purchasing link: [Aliexpress](https://s.click.aliexpress.com/e/_DD0nYYx)  

### Ultrasonic Distance Sensor:
- **Model**: A02YY  
- **Output signal**: PWM  
- **Input**: 5V  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DFYOL3z)  

### Switches:
- **Chrome LED lit momentary switch**: [Aliexpress](https://s.click.aliexpress.com/e/_Dd0BleB)  
  *(Can be used for brew and espresso mode switches)*  

### USB Panel Mount Connector:
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DksmHhh)  

### Shielded Multi-core Cable:
- **Type**: 28AWG, 2-meter  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_DkdDW87)  

### M3 Brass Standoff:
- **Use**: Handy for mounting the PCB  
- **Purchasing link**: [Aliexpress](https://s.click.aliexpress.com/e/_Dn3mUKB)  