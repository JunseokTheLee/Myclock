// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
// TM1637 CLK --> 6
// TM1637 DIO --> 7
// Potentiometer middle pin --> A0

#include <RtcDS1302.h>
#include <TM1637Display.h>

ThreeWire myWire(4, 5, 2); 
RtcDS1302<ThreeWire> Rtc(myWire);
TM1637Display display(6, 7); 

const int potPin = A0; 
const int offThreshold = 50; 
void setup() 
{
    Serial.begin(1200);
    display.setBrightness(0x0f); 

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void loop() 
{
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();

  
    int potValue = analogRead(potPin);
    
    
    if (potValue < offThreshold) {
        display.setBrightness(0x00); 
    } else {
        
        int brightnessLevel = map(potValue, offThreshold, 1023, 0x01, 0x0f);
        display.setBrightness(brightnessLevel); 
    }

    int displayTime = (now.Hour() * 100) + now.Minute();
    display.showNumberDecEx(displayTime, 0b01000000, true); 

    if (!now.IsValid())
    {
        Serial.println("RTC lost confidence in the DateTime!");
    }

    delay(1000); 
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[26];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
