#include <Arduino.h>
#include "LCD_Test.h"
#include <Wire.h>
#include "Adafruit_TCS34725.h"


TwoWire I2C_1 = TwoWire(1);

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;
CST816S touch(6, 7, 13, 5);	// sda, scl, rst, irq

Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_600MS,  // integration time
  TCS34725_GAIN_1X                 // gain
);

struct colorCalibration {
  unsigned int blackValue;
  unsigned int whiteValue;
};

colorCalibration redCal, greenCal, blueCal;

const uint8_t TCS_ADDR = 0x29;

#define SDA_PIN 21
#define SCL_PIN 18

// Menu variables
int currentMenu = 0;
const int MENU_ITEMS = 3;
const char* menuItems[] = {"scan", "history", "calibrate"};
int selectedItem = 0;

struct ScanResult {
  int r, g, b;
  char hex[8];
};
const int MAX_HISTORY = 5;
ScanResult history[MAX_HISTORY];
int historyCount = 0;

void setup()
{    
    Serial.begin(115200);
    I2C_1.begin(21, 18, 100000);
    touch.begin();
    if(psramInit()){
      Serial.println("\nPSRAM is correctly initialized");
    }else{
      Serial.println("PSRAM not available");
    }
    if ((BlackImage = (UWORD *)ps_malloc(Imagesize)) == NULL){
        Serial.println("Failed to apply for black memory...");
        exit(0);
    }

    redCal.blackValue = 730;
    redCal.whiteValue = 7147;
    greenCal.blackValue = 1020;
    greenCal.whiteValue = 12217;
    blueCal.blackValue = 1729;
    blueCal.whiteValue = 20051;
    
    if (DEV_Module_Init() != 0)
      Serial.println("GPIO Init Fail!");
    else
      Serial.println("GPIO Init successful!");
      LCD_1IN28_Init(HORIZONTAL);
      LCD_1IN28_Clear(RED);  
      Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, WHITE);
      Paint_SetScale(65);
      Paint_SetRotate(ROTATE_0);
    
    delay(2000);
    if (tcs.begin(TCS_ADDR, &I2C_1)) {
        Serial.println("TCS34725 found!");
        drawMenu();
    } else {
        Serial.println("No TCS34725 found ... check wiring/power.");
        LCD_1IN28_Clear(RED);
    }
    delay(2000);
    
}

void loop()
{
    if (touch.available()) {
        
        handleTouch();
    }
    delay(50);
}


void drawMenu() {
    LCD_1IN28_Clear(BLACK);
    Paint_DrawString_EN(60, 20, "MAIN MENU", &Font20, BLACK, WHITE);
    const float conversion_factor = 3.3f / (1 << 12) * 3;
    uint16_t result = DEC_ADC_Read();
    
    Paint_DrawString_EN(45, 200, "BAT(V)=", &Font16, BLACK, WHITE);
    Paint_DrawNum(130, 200, result * conversion_factor, &Font16, 2, WHITE, BLACK);
    
    for (int i = 0; i < MENU_ITEMS; i++) {
        int y = 60 + (i * 35);
        UWORD bgColor = (i == selectedItem) ? BLUE : WHITE;
        UWORD textColor = (i == selectedItem) ? WHITE : BLACK;
        
        if (i == selectedItem) {
            Paint_DrawRectangle(20, y-5, 220, y+25, BLUE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        }
        Paint_DrawString_EN(30, y, menuItems[i], &Font16, textColor, bgColor);
    }
    
    LCD_1IN28_Display(BlackImage);
}

void handleTouch() {
    int x = touch.data.x;
    int y = touch.data.y;
    
    if (x > 20 && x < 220 && y > 55 && y < 200) {
        int touchedItem = (y - 55) / 35;
        if (touchedItem >= 0 && touchedItem < MENU_ITEMS) {
            selectedItem = touchedItem;
            drawMenu();
            
            delay(200); 
            if (touch.available() && touch.data.x == x && touch.data.y == y) {
                executeMenuItem(selectedItem);
            }
        }
    }
}

void executeMenuItem(int item) {
    Paint_Clear(WHITE);
    
    switch(item) {
        case 0: {
            uint16_t r,g,b,c;
            tcs.getRawData(&r, &g, &b, &c);

            int redValue = RGBmap(r, redCal.blackValue, redCal.whiteValue, 0, 255);
            int greenValue = RGBmap(g, greenCal.blackValue, greenCal.whiteValue, 0, 255);
            int blueValue = RGBmap(b, blueCal.blackValue, blueCal.whiteValue, 0, 255);
            
            Serial.print("R: "); Serial.print(r);
            Serial.print(" G: "); Serial.print(g);
            Serial.print(" B: "); Serial.print(b);
            Serial.print(" C: "); Serial.println(c);

            Paint_Clear(rgbToColor(redValue,greenValue,blueValue));

            char buffer[20];
            Paint_DrawString_EN(50, 50, "Color Scan", &Font20, BLACK, WHITE);


            Paint_DrawString_EN(40, 80, rgbToHex(redValue, greenValue, blueValue), &Font16, BLACK, WHITE);
            
            sprintf(buffer, "R: %d", redValue);
            Paint_DrawString_EN(40, 100, buffer, &Font16, RED, WHITE);
            
            sprintf(buffer, "G: %d", greenValue);
            Paint_DrawString_EN(40, 120, buffer, &Font16, GREEN, WHITE);
            
            sprintf(buffer, "B: %d", blueValue);
            Paint_DrawString_EN(40, 140, buffer, &Font16, BLUE, WHITE);
            
            sprintf(buffer, "C: %d", c);
            Paint_DrawString_EN(40, 160, buffer, &Font16, BLACK, WHITE);
            
            if (historyCount < MAX_HISTORY) {
                history[historyCount].r = redValue;
                history[historyCount].g = greenValue;
                history[historyCount].b = blueValue;
                strcpy(history[historyCount].hex, rgbToHex(redValue, greenValue, blueValue));
                historyCount++;
            } else {
                for (int i = 0; i < MAX_HISTORY - 1; i++) {
                    history[i] = history[i + 1];
                }
                history[MAX_HISTORY - 1].r = redValue;
                history[MAX_HISTORY - 1].g = greenValue;
                history[MAX_HISTORY - 1].b = blueValue;
                strcpy(history[MAX_HISTORY - 1].hex, rgbToHex(redValue, greenValue, blueValue));
            }
            break;
        }
        case 1: {
            Paint_DrawString_EN(50, 50, "History", &Font20, BLACK, WHITE);
            if (historyCount == 0) {
                Paint_DrawString_EN(40, 100, "No scans yet", &Font16, RED, WHITE);
            } else {
                for (int i = 0; i < historyCount && i < 4; i++) {
                    int y = 80 + (i * 25);
                    char buffer[30];
                    sprintf(buffer, "%s R:%d G:%d B:%d", history[i].hex, history[i].r, history[i].g, history[i].b);
                    Paint_DrawString_EN(20, y, buffer, &Font12, BLACK, WHITE);
                }
            }
            break;
        }
        case 2: {
            Paint_DrawString_EN(30, 50, "Calibration", &Font20, BLACK, WHITE);
            Paint_DrawString_EN(20, 80, "Place WHITE sample", &Font12, BLACK, WHITE);
            Paint_DrawString_EN(20, 95, "and touch screen", &Font12, BLACK, WHITE);
            LCD_1IN28_Display(BlackImage);
            
            while (!touch.available()) delay(50);
            delay(200);
            
            uint16_t wr,wg,wb,wc;
            tcs.getRawData(&wr, &wg, &wb, &wc);
            
            Paint_Clear(WHITE);
            Paint_DrawString_EN(20, 80, "Place BLACK sample", &Font12, BLACK, WHITE);
            Paint_DrawString_EN(20, 95, "and touch screen", &Font12, BLACK, WHITE);
            LCD_1IN28_Display(BlackImage);
            
            while (touch.available()) delay(50);
            while (!touch.available()) delay(50);
            delay(200);
            
            
            uint16_t br,bg,bb,bc;
            tcs.getRawData(&br, &bg, &bb, &bc);
            
            redCal.whiteValue = wr;
            redCal.blackValue = br;
            greenCal.whiteValue = wg;
            greenCal.blackValue = bg;
            blueCal.whiteValue = wb;
            blueCal.blackValue = bb;
            delay(400);
            Paint_Clear(WHITE);
            Paint_DrawString_EN(30, 100, "Calibrated!", &Font20, GREEN, WHITE);
            break;
        }
    }
    
    Paint_DrawString_EN(40, 180, "Touch to return", &Font12, GRAY, WHITE);
    LCD_1IN28_Display(BlackImage);
    
    while (!touch.available()) {
        delay(50);
    }
    delay(200); 
    drawMenu();
}

int RGBmap(unsigned int x, unsigned int inlow, unsigned int inhigh, int outlow, int outhigh){
  float flx = float(x);
  float fla = float(outlow);
  float flb = float(outhigh);
  float flc = float(inlow);
  float fld = float(inhigh);

  float res = ((flx-flc)/(fld-flc))*(flb-fla) + fla;
  
  int result = int(res);
  return constrain(result, outlow, outhigh);
}

const char* rgbToHex(uint8_t r, uint8_t g, uint8_t b) {
  static char hexCol[8];
  sprintf(hexCol, "#%02X%02X%02X", r, g, b);
  return hexCol;
}

UWORD rgbToColor(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

