# ESP32-S3 Colour Sensor

A portable colour scanning device using ESP32-S3 microcontroller and TCS34725 colour sensor with a 1.28" round touch LCD display. Scan colours and get HEX codes on the go!
![wireframe](/assets/wireframe.png)

## Features

- **Real-time colour Scanning**: Capture RGB values and convert to HEX codes
- **Touch Interface**: Intuitive menu navigation on 1.28" round LCD
- **colour History**: Store up to 5 recent scans
- **Calibration System**: White/black point calibration for accuracy
- **Battery Monitor**: Real-time battery voltage display
- **Compact Design**: Portable form factor with 3D printable case

## Hardware Components

- **ESP32-S3 Touch LCD 1.28** ESP32-S3 Development Board, with 1.28inch Round Touch LCD [Link to Purchase](core-electronics.com.au/catalog/product/view/sku/WS-25098)
- **TCS34725** RGB colour sensor [Link to Purchase](https://core-electronics.com.au/tcs34725-rgb-colour-sensor-for-arduino.html)
- **Battery** LiPo 3.7V 400mAh [Link to Purchase](https://core-electronics.com.au/polymer-lithium-ion-battery-400mah-38456.html)
- **Slide Switch** SPDT Micro Slide Switch [Link to Purchase](https://www.jaycar.com.au/spdt-micro-slide-switch/p/SS0834)
- **screws** 4 x 5mm M3 Screws, 2 x 18mm M3 Screws, 2 x 8mm M3 Screws [Link to Purchase](https://www.amazon.com.au/dp/B0B38GL3F8?ref=ppx_yo2ov_dt_b_fed_asin_title)
- **Pitch Connector** 1.25mm JST 2pin connectors [Link to Purchase](https://www.amazon.com.au/dp/B0D9YH9H4G?ref=ppx_yo2ov_dt_b_fed_asin_title) (hard to find batteries with the same JST connector so i soldered this smaller connector to the battery)

## Pin Configuration

```
TCS34725 colour Sensor:
- SDA: GPIO 21
- SCL: GPIO 18
- I2C Address: 0x29
```

## Installation

### Prerequisites

1. **Arduino IDE** with ESP32 board support
2. **Required Libraries**:
   - Adafruit_TCS34725
   - Wire (built-in)

### Setup

1. Clone this repository:
   ```bash
   git clone <repository-url>
   ```

2. Install required libraries via Arduino Library Manager:
   - Search and install "Adafruit TCS34725"

3. Open `ESP32-S3-Touch-LCD-1.28-Colour-Sensor.ino` in Arduino IDE

4. Select board: **ESP32S3 Dev Module**

5. Tools: `Flash Size: "16MB (128MB)"`

6. Tools: `Partition Scheme: "16M Flash (3MB APP/9.9MB FATFS)"`

7. Tools: `PSRAM: "QSPI PSRAM"`

8. Upload the code to your ESP32-S3

## Usage

### Main Menu

The device boots to a main menu with three options:

1. **Scan** - Capture colour readings
2. **History** - View recent scans
3. **Calibrate** - Calibrate sensor

### colour Scanning

1. Select "scan" from main menu
2. Place object near sensor
3. View RGB values and HEX code
4. colour is automatically saved to history

### Calibration

1. Select "calibrate" from main menu
2. Place white reference sample and touch screen
3. Place black reference sample and touch screen
4. Calibration complete!

### History

- View up to 5 recent colour scans
- Shows HEX code and RGB values
- Automatically manages storage (FIFO)

## Technical Specifications

- **colour Sensor**: TCS34725 with 600ms integration time
- **Display**: 240x240 pixel round LCD
- **Touch**: Capacitive touch with gesture support
- **Memory**: PSRAM enabled for display buffer
- **Power**: Battery voltage monitoring
- **Communication**: I2C protocol

## File Structure

```
├── ESP32-S3-Touch-LCD-1.28-Colour-Sensor.ino  # Main application
├── LCD_Test.h                                   # LCD driver header
├── LCD_1in28.cpp/.h                            # LCD implementation
├── GUI_Paint.cpp/.h                            # Graphics library
├── CST816S.cpp/.h                              # Touch controller
├── QMI8658.cpp/.h                              # IMU sensor
├── DEV_Config.cpp/.h                           # Hardware configuration
├── font*.cpp                                   # Font files
├── ImageData.cpp/.h                            # Image resources
└── assets/                                     # 3D models and media
    ├── colour-sensor-lcd v26.stl               # 3D printable case
    ├── colour-sensor-lcd v26.step              # CAD file
    └── colour-sensor-lcd-v26.mp4               # Demo video
```

## 3D Printing

STL and STEP files are included in the `assets/` folder for 3D printing a custom case.

## Troubleshooting

### Common Issues

**TCS34725 not found**
- Check I2C wiring (SDA: GPIO21, SCL: GPIO18)
- Verify sensor power supply
- Check I2C address (0x29)

**Touch not responding**
- Verify touch controller wiring
- Check CST816S initialization
- Ensure proper grounding

**Display issues**
- Confirm PSRAM initialization
- Check SPI connections
- Verify display power

### Calibration Tips

- Use pure white and black references
- Ensure consistent lighting
- Recalibrate if colours appear inaccurate

## Code Structure

### Key Functions

- `setup()` - Initialize hardware and display
- `drawMenu()` - Render main menu interface
- `handleTouch()` - Process touch input
- `executeMenuItem()` - Handle menu selections
- `RGBmap()` - Map raw sensor values to RGB
- `rgbToHex()` - Convert RGB to HEX string

### colour Processing

The device uses calibrated mapping to convert raw TCS34725 values to standard RGB (0-255) range, then generates HEX colour codes for easy use in design applications.

## CAD

I have included the CAD files for the device, these have tolerances for my 3d printer and may be slightly off for your own printer *the screen is press fit and may not hold if tolerances are off*.
**These are printed on a flashforge adventurer 3 with eSUN PLA-Matte**

## Contributing

1. Fork the repository
2. Create feature branch
3. Commit changes
4. Push to branch
5. Create Pull Request

## License

This project is open source.

## Acknowledgments

- Adafruit for TCS34725 library
- Waveshare for LCD driver reference
- ESP32 community for hardware support / Waveshare

## Notes

- The Slide Switch is placed between the battery and JST connector on the ESP32-S3, this will need soldering. *otherwise the device will always be on*
- The device has a calibration already set from the my prototype you can manually update this with these values: 
```
    redCal.blackValue = 730;
    redCal.whiteValue = 7147;
    greenCal.blackValue = 1020;
    greenCal.whiteValue = 12217;
    blueCal.blackValue = 1729;
    blueCal.whiteValue = 20051;
```
- I am using pins 21 and 18 for SDA and SCL for the colour sensor you can change these if needed.
```
#define SDA_PIN 21
#define SCL_PIN 18
```