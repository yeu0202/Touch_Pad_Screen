// the regular Adafruit "TouchScreen.h" library only works on AVRs

// different mcufriend shields have Touchscreen on different pins
// and rotation.
// Run the TouchScreen_Calibr_native sketch for calibration of your shield

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>

char *name = "Filter Controller";  //edit name of shield
const int XP=8,XM=A2,YP=A3,YM=9; //240x320 ID=0x9341
const int TS_LEFT=111,TS_RT=923,TS_TOP=73,TS_BOT=890;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
uint8_t Orientation = 0;    //PORTRAIT

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF



void show_Serial(void)
{
    Serial.println(F("Most Touch Screens use pins 6, 7, A1, A2"));
    Serial.println(F("But they can be in ANY order"));
    Serial.println(F("e.g. right to left or bottom to top"));
    Serial.println(F("or wrong direction"));
    Serial.println(F("Edit name and calibration statements\n"));
    Serial.println(name);
    Serial.print(F("ID=0x"));
    Serial.println(ID, HEX);
    Serial.println("Screen is " + String(tft.width()) + "x" + String(tft.height()));
    Serial.println("Calibration is: ");
    Serial.println("LEFT = " + String(TS_LEFT) + " RT  = " + String(TS_RT));
    Serial.println("TOP  = " + String(TS_TOP)  + " BOT = " + String(TS_BOT));
    Serial.println("Wiring is always PORTRAIT");
    Serial.println("YP=" + String(YP)  + " XM=" + String(XM));
    Serial.println("YM=" + String(YM)  + " XP=" + String(XP));
}

void show_tft(void)
{
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.println(name);
    tft.setTextSize(1);
    tft.println("The colours are all different filters");
    tft.println("Red: Contrast filter");
    tft.println("Blue: Saturation filter");
    tft.println("Green: Kaleidoscope filter");
    tft.println("Cyan: Hue filter");
    tft.println("Yellow: Canny filter");
   
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.setCursor((tft.width() - 48) / 2, (tft.height() * 2) / 4);
    tft.print("START");
//    tft.setTextColor(YELLOW, BLACK);
//    tft.setCursor(0, (tft.height() * 6) / 8);
//    tft.print("Touch screen for loc");
    while (1) {
        tp = ts.getPoint();
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        if (tp.z < MINPRESSURE || tp.z > MAXPRESSURE) continue;
        if (tp.x > 450 && tp.x < 570  && tp.y > 450 && tp.y < 570) break;
//        tft.setCursor(0, (tft.height() * 3) / 4);
//        tft.print("tp.x=" + String(tp.x) + " tp.y=" + String(tp.y) + "   ");
    }
}


void setup(void)
{
    uint16_t tmp;

    tft.reset();
    ID = tft.readID();
    tft.begin(ID);
    Serial.begin(9600);
    show_Serial();
    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);
    show_tft();

    BOXSIZE = tft.width() / 5;
//    Serial.println("BOXSIZE = " + String(BOXSIZE));
    tft.fillScreen(BLACK);

    tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
    tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLUE);
    tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, GREEN);
    tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, CYAN);
    tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, YELLOW);
    tft.drawLine(0,(BOXSIZE + 50), 300, (BOXSIZE + 50), WHITE);

    tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
    currentcolor = RED;
    delay(1000);
    
    tft.setCursor(0,50);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print(" F1");
    // help text
    tft.setCursor(0,70);
    tft.setTextSize(1);
    tft.print("Select a filter to clear the screen");
    Serial.println("XRED");
    Serial.println("XRED");
    
}
void loop()
{
    uint16_t xpos, ypos;  //screen coordinates
    tp = ts.getPoint();   //tp.x, tp.y are ADC values

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!

    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
        // most mcufriend have touch (with icons) that extends below the TFT
        // screens without icons need to reserve a space for "erase"
        // scale the ADC values from ts.getPoint() to screen values e.g. 0-239
        //
        // Calibration is true for PORTRAIT. tp.y is always long dimension 
        // map to your current pixel orientation
        switch (Orientation) {
            case 0:
                xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
                ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
                break;
            case 1:
                xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
                ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
                break;
            case 2:
                xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
                ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
                break;
            case 3:
                xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
                ypos = map(tp.x, TS_LEFT, TS_RT, 0, tft.height());
                break;
        }

        // are we in top color box area ?
        if (ypos < BOXSIZE) {               //draw white border on selected color box
            oldcolor = currentcolor;

            if (xpos < BOXSIZE) {
                currentcolor = RED;
                tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);             
            } else if (xpos < BOXSIZE * 2) {
                currentcolor = BLUE;
                tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 3) {
                currentcolor = GREEN;
                tft.drawRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 4) {
                currentcolor = CYAN;
                tft.drawRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 5) {
                currentcolor = YELLOW;
                tft.drawRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, WHITE);
            }
            

            if(currentcolor == RED)
            {
              tft.fillRect(0, (BOXSIZE), tft.width(), (BOXSIZE), BLACK);
              tft.setCursor(0,50);
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.print(" F1");
              tft.fillRect(0, (BOXSIZE + 51), tft.width(), tft.height() - (BOXSIZE + 51), BLACK);
            
              // send new color to serial port. Send twice because receiving end will erase one
              Serial.println("XRED");
              Serial.println("XRED");
              }
              if(currentcolor == BLUE)
            {
              tft.fillRect(0, (BOXSIZE), tft.width(), (BOXSIZE), BLACK);
              tft.setCursor(BOXSIZE,50);
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.print(" F2");
              tft.fillRect(0, (BOXSIZE + 51), tft.width(), tft.height() - (BOXSIZE + 51), BLACK);
            
              // send new color to serial port. Send twice because receiving end will erase one
              Serial.println("XBLUE");
              Serial.println("XBLUE");
              }
              if(currentcolor == GREEN)
            {
              tft.fillRect(0, (BOXSIZE), tft.width(), (BOXSIZE), BLACK);
              tft.setCursor(BOXSIZE * 2,50);
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.print(" F3");
              tft.fillRect(0, (BOXSIZE + 51), tft.width(), tft.height() - (BOXSIZE + 51), BLACK);
            
              // send new color to serial port. Send twice because receiving end will erase one
              Serial.println("XGREEN");
              Serial.println("XGREEN");
              }
              if(currentcolor == CYAN)
            {
              tft.fillRect(0, (BOXSIZE), tft.width(), (BOXSIZE), BLACK);
              tft.setCursor(BOXSIZE * 3,50);
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.print(" F4");
              tft.fillRect(0, (BOXSIZE + 51), tft.width(), tft.height() - (BOXSIZE + 51), BLACK);
            
              // send new color to serial port. Send twice because receiving end will erase one
              Serial.println("XCYAN");
              Serial.println("XCYAN");
              }
              if(currentcolor == YELLOW)
            {
              tft.fillRect(0, (BOXSIZE), tft.width(), (BOXSIZE), BLACK);
              tft.setCursor(BOXSIZE * 4,50);
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.print(" F5");
              tft.fillRect(0, (BOXSIZE + 51), tft.width(), tft.height() - (BOXSIZE + 51), BLACK);
            
              // send new color to serial port. Send twice because receiving end will erase one
              Serial.println("XYELLOW");
              Serial.println("XYELLOW");
              }
              
              // help text
              tft.setCursor(0,70);
              tft.setTextSize(1);
              tft.print("Select a filter to clear the screen");

            if (oldcolor != currentcolor) { //rub out the previous white border
                if (oldcolor == RED) tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
                if (oldcolor == BLUE) tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLUE);
                if (oldcolor == GREEN) tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, GREEN);
                if (oldcolor == CYAN) tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, CYAN);
                if (oldcolor == YELLOW) tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, YELLOW);
            }
        }
        // are we in drawing area ?
        else if (((ypos - PENRADIUS) > (BOXSIZE + 50)) && ((ypos + PENRADIUS) < tft.height())) {
            tft.fillCircle(xpos, ypos, PENRADIUS, currentcolor);
            int normalisedX = map(xpos, 0, 245, 0, 255);
            normalisedX = constrain(normalisedX, 0, 255);
            int normalisedY = map(ypos, 100, 320, 0, 255);
            normalisedY = constrain(normalisedY, 0, 255);
            Serial.println("X=" + String(normalisedX) + ",Y=" + String(normalisedY) + ","); // serial send the coordinate
        }
        // are we in erase area ?
        // Plain Touch panels use bottom 10 pixels e.g. > h - 10
        // Touch panels with icon area e.g. > h - 0
//        if (ypos > tft.height() - 10) {
//            // press the bottom of the screen to erase
//            tft.fillRect(0, (BOXSIZE + 51), tft.width(), tft.height() - (BOXSIZE + 51), BLACK);
//        }
    }
}
