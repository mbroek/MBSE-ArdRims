// ==============================================
// ATTENTION!!!!!
// YOU MUST SET ONLY THIS SECTION
// ==============================================

#include <EEPROM.h>
#include <OneWire.h>
#include <PID_v1.h>

//SET PCB
// 1 Brauduino Original (Matho's PCB)
// 2 Brauduino by DanielXan
// 3 ArdBir by DanielXan
// 4 Protoduino NANO by J. Klinge
// 5 ArdRims NANO by C. Broek
// 6 ArduinoBrewboard by J. Klinge ?? 2 soorten ??
#define PCBType 6

// should be false
#define FakeHeating     false       // For development only.
#define USE_HLT         false       // A HLT shared with the MLT. (Not yet).
#define Silent          false       // No beeps (during development).
#define USE_ESP8266     false       // Add serial interface to ESP8266-12E DEVKIT   DO NOT USE !!! 

// Serial debugging
#define DebugPID        true
#define DebugProcess    true
#define DebugButton     true
#define DebugReadWrite  true
#define DebugErrors     true


// Default language is English, others must be set.
// Nederlands.
#define langNL          true

// Don not change this next block
#if FakeHeating == true
#define USE_DS18020     false
#else
#define USE_DS18020     true
#endif

#if USE_DS18020 == true
// Normal brew sensor
#if PCBType == 1
const byte SensorMLTPin = 11;
#elif PCBType == 2
const byte SensorMLTPin =  8;
#elif (PCBType == 3 || PCBType == 4 || PCBType == 6)
const byte SensorMLTPin =  7;
#elif PCBType == 5
const byte SensorMLTPin =  7;
#if USE_HLT == true
// Sensor for sparge water.
const byte SensorHLTPin = 11;
#endif // USE_HLT
#endif // PCBType
#endif // USE_DS18020

// Output Pump, Buzzer, Heater
#if PCBType == 1
#define PumpControlPin   8
#define BuzzControlPin   10
#define HeatControlPin   9
#elif PCBType == 2
#define PumpControlPin   9
#define BuzzControlPin   10
#define HeatControlPin   11
#elif PCBType == 3
#define PumpControlPin   6
#define BuzzControlPin   8
#define HeatControlPin   9
#elif PCBType == 4
#define PumpControlPin   6
#define BuzzControlPin   A7
#define HeatControlPin   9
#elif PCBType == 5
#define PumpControlPin   6
#define BuzzControlPin   8
#define HeatControlPin   9
#if USE_HLT == true
// Heater for sparge water
#define HLTControlPin   10
#endif
#elif PCBType == 6
#define PumpControlPin   6
#define BuzzControlPin   11
#define HeatControlPin   9
#endif

// Keyboard buttons
#if (PCBType == 1 || PCBType == 5)
#define ButtonUpPin     A3
#define ButtonDownPin   A2
#define ButtonStartPin  A1
#define ButtonEnterPin  A0
#elif PCBType == 2
#define ButtonUpPin     A3
#define ButtonDownPin   A2
#define ButtonStartPin  A0
#define ButtonEnterPin  A1
#elif (PCBType == 3 || PCBType == 4 || PCBType == 6)
#define ButtonUpPin     A2
#define ButtonDownPin   A3
#define ButtonStartPin  A0
#define ButtonEnterPin  A1
#endif

#if USE_DS18020 == true
OneWire dsm(SensorMLTPin);
#if USE_HLT == true
OneWire dsh(SensorHLTPin);
#endif
#endif

// LCD connections
#include <LiquidCrystal.h>
#if (PCBType == 1 || PCBType == 2)
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
#elif (PCBType == 3 || PCBType == 4 || PCBType == 5)
LiquidCrystal lcd(A4, A5, 2, 3, 4, 5);
#elif (PCBType == 6)
LiquidCrystal lcd(A5, A4, 2, 3, 4, 5);
#endif

/*
   Timer using the interrupt driven secTimer library.
*/
#include <secTimer.h>

secTimer myTimer;


// ==============================================
// END OF SETTING SECTION
// ==============================================

/*
  MBSE-ArdRims is a single (well ..) vessel RIMS controller. It is
  based on ideas of braudino, Open-ArdBir, BrewManiac and maybe others.
  Most of the code is written from scratch.


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


// *************************
//*  global variables
// *************************

#include "defines.h"

unsigned long gSystemStartTime;          // in milliseconds.
unsigned long gCurrentTimeInMS;          // in milliseconds.
unsigned long w_StartTime;
unsigned long Timer;
unsigned long _seconds;                  // timer seconds.
#if FakeHeating == true
unsigned long FakeHeatLastInMS;          // in milliseconds.
#endif

#if USE_DS18020 == true
boolean ConvMLT_start = false;
#if USE_HLT == true
boolean ConvHLT_start = false;
#endif
#endif
#if USE_HLT == true
boolean HLT_is_On = false;
boolean HLT_block = false;
#endif
boolean pumpRest;


byte    mainMenu = 0;
byte    stageTime;
byte    hopTime;
byte    CurrentState = StageNothing;
byte    WindowSize;
byte    Direction;
byte    Boil_output;
byte    nmbrHops;
byte    hopAdd;
byte    MashState;
byte    LogFactor = 0;
byte    pumpTime;


double  Input;
double  Output;
double  Setpoint;

#if FakeHeating == true
float   Temp_MLT = 18.90;
float   Plate_MLT = 18.90;
#else
float   Temp_MLT = 0.0;
#endif
float   boilStageTemp;
float   stageTemp;
#if USE_HLT == true
#if FakeHeating == true
float   Temp_HLT = 18.70;
float   Plate_HLT = 18.70;
#else
float   Temp_HLT = 0.0;
#endif
float   HLT_SetPoint;
#endif


PID myPID(&Input, &Output, &Setpoint, 100, 40, 0, DIRECT);

void Temperature();
void PID_Heat(boolean);
void bk_heat_on();
void bk_heat_off();
void pump_off();
void HLT_on();
void HLT_off();
void Buzzer(byte, int);

#include "functions.h"
#include "timers.h"
#include "buttons.h"
#include "prompts.h"
#include "setup.h"




#if (DebugProcess == true || DebugPID == true)
void DebugTimeSerial() {
  byte Hour, Minute, Second;
  unsigned int Millisecond;

  Hour        = (byte) ((gCurrentTimeInMS / 1000) / 3600);
  Minute      = (byte)(((gCurrentTimeInMS / 1000) % 3600) / 60);
  Second      = (byte) ((gCurrentTimeInMS / 1000) % 60);
  Millisecond = (unsigned int)gCurrentTimeInMS % 1000;
  Serial.print("Time: ");
  if (Hour < 10)
    Serial.print("0");
  Serial.print(Hour);
  Serial.print(":");
  if ( Minute < 10)
    Serial.print("0");
  Serial.print(Minute);
  Serial.print(":");
  if (Second < 10)
    Serial.print("0");
  Serial.print(Second);
  Serial.print(".");
  if (Millisecond <   10)
    Serial.print("0");
  if (Millisecond <  100)
    Serial.print("0");
  Serial.print(Millisecond);
  Serial.print(" ");
}
#endif



#if USE_DS18020 == true
byte OwsInitialize(OneWire ows) {
  if (ows.reset()) {   // return 1 if present, 0 if not.
    ows.skip();
    return 1;
  }
  return 0;
}


void ReadOwSensor(OneWire ows, boolean & Convert_start, float & TempC, boolean Offset) {
  byte data[9];

  // start conversion and return
  if (!(Convert_start)) {
    if (! OwsInitialize(ows))
      return;
    ows.write(0x44, 0);
    Convert_start = true;
    return;
  }
  if (Convert_start) {

    // check for conversion if it isn't complete return if it is then convert to decimal
    if (ows.read_bit() == 0)
      return;

    // Allways a new start after the next steps
    Convert_start = false;
    if (OwsInitialize(ows)) {
      ows.write(0xBE);                           // Read scratchpad
      ows.read_bytes(data, 9);
      if ( OneWire::crc8(data, 8) != data[8]) {
        // if checksum fails start a new conversion.
#if DebugErrors == true
        ew_byte(EM_ErrorNo(0), er_byte(EM_ErrorNo(0)) + 1);        // error counter 0
#endif
        return;
      }
    } else {
      return;
    }

    /*
       After a sensor is connected, or after power-up, the sensor resolution
       can be different from what we desire. If so, configure the sensor and
       start over again.
    */
    if ((data[4] & 0x60) != 0x60) {
      OwsInitialize(ows);
      ows.write(0x4E);           // Write scratchpad
      ows.write(0);              // TL register
      ows.write(0);              // TH register
      ows.write(0x7F);           // Configuration 12 bits, 750 ms
      return;
    }
    int16_t raw = (data[1] << 8) | data[0];

    /*
       Check sign bits, must be all zero's or all one's.
    */
    if ((raw & 0xf800) != 0) {
#if DebugErrors == true
      ew_byte(EM_ErrorNo(1), er_byte(EM_ErrorNo(1)) + 1);        // error counter 1
#endif
      return;
    }
    if ((raw & 0xf800) == 0xf800) {
#if DebugErrors == true
      ew_byte(EM_ErrorNo(2), er_byte(EM_ErrorNo(1)) + 2);        // error counter 2
#endif
      return;
    }

    if (Offset)
      TempC = ((float)raw / 16.0) + ((float)((EEPROM.read(EM_TempOffset) - 50) / 10.0));
    else
      TempC = (float)raw / 16.0;
  }
}
#endif // USE_DS18020



/*
   Read Temperature sensors
*/
void Temperature() {

#if USE_DS18020 == true
  ReadOwSensor(dsm, ConvMLT_start, Temp_MLT, true);

#if USE_HLT == true
  ReadOwSensor(dsh, ConvHLT_start, Temp_HLT, false);
#endif
#endif // USE_DS18020

#if FakeHeating == true
  TimerRun();

  // Try to be as slow as a real sensor
  if ((gCurrentTimeInMS - FakeHeatLastInMS) < 500)
    return;
  /*
     Make this fake heater a bit more real by using a simulated heatplate.
     We heatup that plate and then transfer the heat to the water.
     That way we get a nice overshoot like in real life.
  */
  if (digitalRead(HeatControlPin) == HIGH) {
    if (Plate_MLT < 250.0)
      Plate_MLT += (gCurrentTimeInMS - FakeHeatLastInMS) * 0.001;   // Simulate plate upto 250 degrees
  } else {
    if (Plate_MLT > Temp_MLT)
      Plate_MLT -= (gCurrentTimeInMS - FakeHeatLastInMS) * 0.00002 * (Plate_MLT - Temp_MLT);
  }
  // If plate is hotter then the water with a offset so that cooling later works.
  if (Plate_MLT > (Temp_MLT + 5.0)) {
    if (Temp_MLT < 100.05)
      Temp_MLT += (gCurrentTimeInMS - FakeHeatLastInMS) * 0.000001 * (Plate_MLT - Temp_MLT);
  }
  // Allways loose heat to the air
  if (Temp_MLT > 16.0) {
    Temp_MLT -= (gCurrentTimeInMS - FakeHeatLastInMS) * 0.00000010 * (Temp_MLT - 16.0);
    if (digitalRead(PumpControlPin) == HIGH) // More heat loss when pump is on
      Temp_MLT -= (gCurrentTimeInMS - FakeHeatLastInMS) * 0.00000007 * (Temp_MLT - 16.0);
  }

#if USE_HLT == true
  if (digitalRead(HLTControlPin) == HIGH) {
    if (Temp_HLT < 100.05)
      Temp_HLT += (gCurrentTimeInMS - FakeHeatLastInMS) * 0.000055;
  } else {
    if (Temp_HLT > 16.0)
      Temp_HLT -= (gCurrentTimeInMS - FakeHeatLastInMS) * 0.00000006 * (Temp_HLT - 16.0);
  }
#endif

  FakeHeatLastInMS = gCurrentTimeInMS;
#endif
}



#if USE_HLT == true
void HLT_Heat(void) {
  (Temp_HLT < HLT_SetPoint) ? HLT_on() : HLT_off();
}
#endif


void LCDChar(byte X, byte Y, byte C) {
  lcd.setCursor(X, Y);
  lcd.write(C);
}


void LoadPIDsettings() {
  // send the PID settings to the PID
  myPID.SetTunings(er_byte(EM_PID_kP) - 100, (double)(er_byte(EM_PID_kI) / 1000.00), er_byte(EM_PID_kD) - 100);
  WindowSize = er_byte(EM_WindowSize);
  myPID.SetSampleTime(er_byte(EM_SampleTime) * 250);
  LogFactor = er_byte(EM_LogFactor);
  /*
     Initialize the PID
  */
  Output = 0.0;   // Reset internal Iterm.
  myPID.SetMode(MANUAL);
  myPID.SetMode(AUTOMATIC);
}



/*
   PID control.
    autoMode = true  - PID is active.
    autoMode = false - Output value is send as slow PWM
*/
void PID_Heat(boolean autoMode) {
  double RealPower;

  TimerRun();
  if (autoMode)
    myPID.Compute();

  /*
     Apply logarithmic factor to the output.
  */
  RealPower = int(Output);
  if (RealPower && LogFactor) {
    /*
       Make sure that even 1% Output results in enough power to actually heat the water.
    */
    RealPower += ((255 - int(Output)) / (21 - LogFactor));
  }

  if (gCurrentTimeInMS - w_StartTime > (unsigned int) WindowSize * 250) {
    w_StartTime += (unsigned int)WindowSize * 250; //time to shift the Relay Window
  }
  ((RealPower / 255) * ((unsigned int)WindowSize * 250) > gCurrentTimeInMS - w_StartTime) ? bk_heat_on() : bk_heat_off();

#if DebugPID == true
  static unsigned long LastTimeSpent;
  if (TimeSpent != LastTimeSpent) {
    LastTimeSpent = TimeSpent;
    DebugTimeSerial();
    (autoMode) ? Serial.print(F("AUTOMATIC ")) : Serial.print(F("MANUAL    "));
    Serial.print(F("Mash Temp: "));
    if (Temp_MLT <  10 && Temp_MLT >= 0) Serial.print(F("  "));
    if (Temp_MLT < 100 && Temp_MLT >= 10) Serial.print(F(" "));
    Serial.print(Temp_MLT);
    Serial.print(F(" Setpoint: "));
    if (Setpoint <  10 && Setpoint >= 0) Serial.print(F("  "));
    if (Setpoint < 100 && Setpoint >= 10) Serial.print(F(" "));
    Serial.print(Setpoint);
    Serial.print(F(" Output: "));
    if (Output <  10 && Output >= 0) Serial.print(F("  "));
    if (Output < 100 && Output >= 10) Serial.print(F(" "));
    Serial.print(Output);
    Serial.print(F(" RealPower="));
    if (RealPower <  10 && RealPower >= 0) Serial.print(F("  "));
    if (RealPower < 100 && RealPower >= 10) Serial.print(F(" "));
    Serial.print(RealPower);
    Serial.print(F(" LogFactor="));
    Serial.print(LogFactor);
    Serial.print(F("  Now: "));
    Serial.print(gCurrentTimeInMS);
    Serial.print(F(" w_StartTime: "));
    Serial.println(w_StartTime);
  }
#endif
}



/*
   Boil/Mash kettle heat control
*/
void bk_heat_on() {
#if USE_HLT == true
  HLT_block = true;
  if (digitalRead(HLTControlPin) == HIGH) {
    digitalWrite(HLTControlPin, LOW);
    LCDChar(0, 1, 5);
  }
#endif
  digitalWrite(HeatControlPin, HIGH);
  LCDChar(19, 1, 6);
}
void bk_heat_off() {
  digitalWrite(HeatControlPin, LOW);
  LCDChar(19, 1, 5);
#if USE_HLT == true
  HLT_block = false;
  if (HLT_is_On) {
    digitalWrite(HLTControlPin, HIGH);
    LCDChar(0, 1, 6);
  }
#endif
}
void bk_heat_hide() {
  digitalWrite(HeatControlPin, LOW);
  LCDChar(19, 1, 32);
#if USE_HLT == true
  HLT_block = false;
  if (HLT_is_On) {
    digitalWrite(HLTControlPin, HIGH);
    LCDChar(0, 1, 6);
  }
#endif
}


/*
   Pump control.
*/
void pump_on() {
  digitalWrite(PumpControlPin, HIGH);
  LCDChar(19, 2, 4);
}
void pump_off() {
  digitalWrite(PumpControlPin, LOW);
  LCDChar(19, 2, 3);
}
void pump_hide() {
  digitalWrite(PumpControlPin, LOW);
  LCDChar(19, 2, 32);
}



/*
   HLT heating control
*/
#if USE_HLT == true
void HLT_on() {
  if (HLT_block == false) {
    digitalWrite(HLTControlPin, HIGH);
    LCDChar(0, 1, 6);
  } else {
    digitalWrite(HLTControlPin, LOW);
    LCDChar(0, 1, 5);
  }
  HLT_is_On = true;
}
void HLT_off() {
  digitalWrite(HLTControlPin, LOW);
  LCDChar(0, 1, 5);
  HLT_is_On = false;
}
void HLT_hide() {
  digitalWrite(HLTControlPin, LOW);
  LCDChar(0, 1, 32);
  HLT_is_On = false;
}
#endif


/*
   Center display values
*/
void DisplayValues(boolean PWM, boolean Timer, boolean HLTtemp, boolean HLTset) {

  TimerRun();
  Prompt(X1Y1_temp);
  Prompt(X11Y1_setpoint);

#if USE_HLT == true
  if (PWM && HLTtemp)
    // Dual show Mash PWM and HLT temperature.
    ((TimeSpent % 5) && ! pumpRest) ? Prompt(X1Y2_pwm) : Prompt(X1Y2_temp);
  else if (PWM) {
    if (pumpRest)
      Prompt(X1Y2_blank);
    else
      Prompt(X1Y2_pwm);
  } else if (HLTset)
    Prompt(X1Y2_temp);
  if (Timer && HLTset)
    (TimeSpent % 5) ? Prompt(X11Y2_timer) : Prompt(X11Y2_setpoint);
  else if (Timer)
    Prompt(X11Y2_timer);
  else if (HLTset)
    Prompt(X11Y2_setpoint);
#else
  if (PWM) {
    if (pumpRest)
      Prompt(X1Y2_blank);
    else
      Prompt(X1Y2_pwm);
  }
  if (Timer)
    Prompt(X11Y2_timer);
#endif
}



/*
   Toggle pump
*/
void PumpControl() {
  //turns the pump on or off
  if (btn_Press(ButtonStartPin, 50))
    (digitalRead(PumpControlPin) == HIGH) ? pump_hide() : pump_on();
}



/*
   Iodine test, continue after user presses Enter
   or after the iodine timeout.
*/
void IodineTest(void) {
  byte IodineTime = er_byte(EM_IodoneTime);

  TimerSet(IodineTime * 60);
  while (true) {
    Temperature();
    Input = Temp_MLT;
    Prompt(P0_iodine);
#if USE_HLT == true
    DisplayValues(true, true, Temp_HLT != 0.0, true);
#else
    DisplayValues(true, true, false, true);
#endif
    PID_Heat(true);
#if USE_HLT == true
    if (HLT_SetPoint)
      HLT_Heat();
#endif

    if (TimeSpent % 45 == 0)
      Buzzer(1, 65);

    Prompt(P3_xxxO);
    if (btn_Press(ButtonEnterPin, 50) || (TimeLeft == 0)) {
      return;
    }
  }
}



/*
   Manual control
*/
void manual_mode() {
  byte    manualMenu   = 0;
  float   mset_temp    = 35.0;
  boolean mheat        = false;
  boolean mtempReached = false;
  boolean mreachedBeep = false;
#if USE_HLT == true
  float   hset_temp    = 40.0;
  boolean hheat        = false;
  boolean htempReached = false;
  boolean hreachedBeep = false;
#endif

  lcd.clear();
  Prompt(P0_manual);

  if (PromptForMashWater(true) == false) {
    lcd.clear();
    return;
  }
#if USE_HLT == true
  if (PromptForMashWater(false) == false) {
    lcd.clear();
    return;
  }
#endif
  Prompt(P1_clear);
  LoadPIDsettings();

  while (true) {

    Temperature();
    Setpoint = mset_temp;
    Input = Temp_MLT;

    if (mtempReached == false) {
      if (Input >= Setpoint) {
        mtempReached = true;
      }
    }

    if (mtempReached && (mreachedBeep == false)) {
      Buzzer(3, 250);
      mreachedBeep = true;
    }

    (mheat) ? PID_Heat(true) : bk_heat_hide();

#if USE_HLT == true
    HLT_SetPoint = hset_temp;

    if (htempReached == false) {
      if (Temp_HLT >= HLT_SetPoint) {
        htempReached = true;
      }
    }

    if (htempReached && (hreachedBeep == false)) {
      Buzzer(3, 250);
      hreachedBeep = true;
    }

    (hheat) ? HLT_Heat() : HLT_hide();
    DisplayValues(mheat, false, true, true);
#else
    DisplayValues(mheat, false, false, false);
#endif

    switch (manualMenu) {

      case 0:          // manual Main menu
#if USE_HLT == true
        Prompt(P3_HBPQ);
        if (btn_Press(ButtonUpPin, 50))
          manualMenu = 1;
#else
        Prompt(P3_xBPQ);
#endif
        if (btn_Press(ButtonDownPin, 50))
          manualMenu = 2;
        if (btn_Press(ButtonStartPin, 50))
          manualMenu = 3;
        if (btn_Press(ButtonEnterPin, 50)) {
          lcd.clear();
          bk_heat_hide();
          pump_hide();
#if USE_HLT == true
          HLT_hide();
#endif
          return;
        }
        break;

#if USE_HLT == true
      case 1:         // manual Hot Liquer Tank
        (hheat) ? Prompt(P3_UD0Q) : Prompt(P3_UD1Q);
        ReadButton(Direction, Timer);
        Set(hset_temp, 110, 20, 0.25, Timer, Direction);
        if ((hset_temp - HLT_SetPoint) > 2) {
          // Increased setting at least 2 degrees
          htempReached = hreachedBeep = false;
        }
        if (btn_Press(ButtonStartPin, 50)) {
          (hheat) ? hheat = false : hheat = true;
        }
        if (btn_Press(ButtonEnterPin, 50))
          manualMenu = 0;
        break;
#endif

      case 2:          // manual Boil Kettle heater
        (mheat) ? Prompt(P3_UD0Q) : Prompt(P3_UD1Q);
        ReadButton(Direction, Timer);
        Set(mset_temp, 110, 20, 0.25, Timer, Direction);
        if ((mset_temp - Setpoint) > 2) {
          // Increased setting at least 2 degrees
          mtempReached = mreachedBeep = false;
        }
        if (btn_Press(ButtonStartPin, 50)) {
          (mheat) ? mheat = false : mheat = true;
        }
        if (btn_Press(ButtonEnterPin, 50))
          manualMenu = 0;
        break;

      case 3:          // manual Pump control.
        (digitalRead(PumpControlPin) == HIGH) ? Prompt(P3_xx0Q) : Prompt(P3_xx1Q);
        PumpControl();
        if (btn_Press(ButtonEnterPin, 50))
          manualMenu = 0;
        break;
    }
  }
}



/*
   Automatic brew control
*/
void auto_mode() {
  byte    NewState             = StageInit;
  byte    tmpMinute            = 0;
  byte    TimeWhirlPool;
  byte    _EM_StageTime;
  byte    _EM_PumpPreMash      = er_byte(EM_PumpPreMash);
  byte    _EM_PumpOnMash       = er_byte(EM_PumpOnMash);
  byte    _EM_PumpMashout      = er_byte(EM_PumpMashout);
  byte    _EM_PumpOnBoil       = er_byte(EM_PumpOnBoil);
  byte    _EM_PumpMaxTemp      = er_byte(EM_PumpMaxTemp);
  byte    _EM_Whirlpool_9      = er_byte(EM_Whirlpool_9);
  byte    _EM_Whirlpool_7      = er_byte(EM_Whirlpool_7);
  byte    _EM_Whirlpool_6      = er_byte(EM_Whirlpool_6);
  byte    _EM_Whirlpool_2      = er_byte(EM_Whirlpool_2);
  byte    _EM_PumpCycle        = er_byte(EM_PumpCycle);
  byte    _EM_PumpRest         = er_byte(EM_PumpRest);
  byte    LastMashStep         = 0;
  byte    ResumeTime;
  boolean Resume               = false;
  boolean tempBoilReached      = false;
  boolean newMinute            = false;
  boolean Pwhirl;
  boolean WP9Done              = false;
  boolean WP7Done              = false;
  boolean WP6Done              = false;
  boolean CoolBeep             = false;
#if DebugProcess == true
  boolean Debugger             = false;
#endif
  float   _EM_StageTemp;
  float   DeltaTemp;

  CurrentState = StageNothing;
  LoadPIDsettings();

  /*
     See what our last Mash Step is
  */
  for (byte i = 1; i < 7; i++) {
    if (er_byte(EM_StageTime(i)))
      LastMashStep = i;
  }
#if DebugProcess == true
  DebugTimeSerial();
  Serial.print(F("Last Mash Step: "));
  Serial.println(LastMashStep);
#endif

  /*
     Check for a crashed/unfinished brew
  */
  if (er_byte(EM_AutoModeStarted)) {
    lcd.clear();
    if (WaitForConfirm(2, false, 0, P1_resume, 0, P3_proceed)) {
      NewState = CurrentState = er_byte(EM_StageResume);
      TimeLeft = ResumeTime = er_byte(EM_StageTimeLeft);
      Resume = true;
      MashState = MashNone;
      pumpTime = 0;
      pumpRest = false;

      Temp_MLT = 65.2;

    } else {
      ew_byte(EM_AutoModeStarted, 0);
    }
    lcd.clear();
  }

  do {
startover:

    Temperature();
    Input = Temp_MLT;
    TimerRun();
    if ((byte)((TimeLeft % 3600) / 60) != tmpMinute) {
      tmpMinute = (byte)((TimeLeft % 3600) / 60);
      stageTime = (byte)(TimeLeft / 60);
      ew_byte(EM_StageTimeLeft, stageTime);
      newMinute = true;
    }

    /*
       New state change
    */
    if (NewState != CurrentState) {

#if DebugProcess == true
      DebugTimeSerial();
      Serial.print(F("Current State: "));
      Serial.print(CurrentState);
      Serial.print(F(" -> New State: "));
      Serial.println(NewState);
#endif

      lcd.clear();
      bk_heat_hide();
      pump_hide();
#if USE_HLT
      HLT_hide();
#endif

      /*
         Do once the state we enter
      */
      switch (NewState) {

        case StageMashIn:
        case StageMash1:
        case StageMash2:
        case StageMash3:
        case StageMash4:
        case StageMash5:
        case StageMash6:
        case StageMashOut:
          MashState = MashNone;
          pumpTime = 0;
          pumpRest = false;
          break;

        case StageBoil:
          TimerSet(240 * 60);      // Just make sure the timer runs.
          Output = 255;
          stageTemp = Setpoint = er_byte(EM_BoilTemperature);
          stageTime = er_byte(EM_BoilTime);
          Boil_output = er_byte(EM_BoilHeat);
          pumpRest = false;

          hopAdd = 0;
          ew_byte(EM_HopAddition, hopAdd);
          nmbrHops = er_byte(EM_NumberOfHops);
          hopTime = er_byte(EM_TimeOfHop(hopAdd));
          break;

        case StageCooling:
          if (! WaitForConfirm(2, false, 0, P1_cool, 0, P3_proceed)) {
            NewState = StageFinished;
            goto startover;
          }
          lcd.clear();
          CoolBeep = false;
          if (_EM_Whirlpool_7 && ! WP7Done) {
            stageTemp = 77.0;
          } else if (_EM_Whirlpool_6 && ! WP6Done) {
            stageTemp = 66.0;
          } else {
            stageTemp = word(er_byte(EM_CoolingTemp), er_byte(EM_CoolingTemp + 1)) / 16.0;
          }
#if DebugProcess == true
          DebugTimeSerial();
          Serial.print(F("Start Cooling Temp="));
          Serial.print(Temp_MLT);
          Serial.print(F(" Target="));
          Serial.println(stageTemp);
#endif
          break;

        case StageWhirlpool2:
        case StageWhirlpool9:
        case StageWhirlpool7:
        case StageWhirlpool6:
          if (! WaitForConfirm(2, false, 0, P1_whirl, 0, P3_proceed)) {
            if (NewState == StageWhirlpool2)
              NewState = StageFinished;
            else
              NewState = StageCooling;
            goto startover;
          }
          lcd.clear();
          Prompt(P1_twhirl);

          if (_EM_Whirlpool_9 && ! WP9Done) {
            TimeWhirlPool = _EM_Whirlpool_9;
            Setpoint = 93.0;
          } else if (_EM_Whirlpool_7 && ! WP7Done) {
            TimeWhirlPool = _EM_Whirlpool_7;
            Setpoint = 74.0;
          } else if (_EM_Whirlpool_6 && ! WP6Done) {
            TimeWhirlPool = _EM_Whirlpool_6;
            Setpoint = 63.0;
          } else {
            TimeWhirlPool = _EM_Whirlpool_2;
          }

          Pwhirl = true;
          while (Pwhirl) {
            TimerShow(TimeWhirlPool * 60, 6, 2);
            Prompt(P3_SGQO);
            ReadButton(Direction, Timer);
            Set((TimeWhirlPool), WhirlpoolMaxtime, 1, 1, Timer, Direction);
            if (btn_Press(ButtonStartPin, 1500))
              Pwhirl = false;
            if (btn_Press(ButtonEnterPin, 50)) {
              Pwhirl = false;
              TimerSet(TimeWhirlPool * 60);
            }
          }
          lcd.clear();
#if DebugProcess == true
          DebugTimeSerial();
          Serial.print(F("Whirlpool "));
          Serial.print(TimeWhirlPool);
          Serial.print(F(" mins. Temp="));
          Serial.print(Temp_MLT);
          Serial.print(F(" Sp="));
          Serial.println(Setpoint);
#endif
          break;

        case StagePrepare:
          /*
             Pump Prime
          */
          Prompt(P1_prime);
          for (byte i = 1; i < 6; i++) {
            pump_on();
            delay(750 + i * 250);
            pump_off();
            delay(350);
          }
          pump_hide();
          ew_byte(EM_AutoModeStarted, 1);
          Setpoint = (word(er_byte(EM_StageTemp(0)), er_byte(EM_StageTemp(0) + 1)) / 16.0) - 10.0; // 10 below Mash-in
          lcd.clear();
          break;
      }

      CurrentState = NewState;
      ew_byte(EM_StageResume, CurrentState);
      ew_byte(EM_StageTimeLeft, 0);
      Resume = false;
    }

    /*
       Current unchanged state
    */
    switch (CurrentState) {

      case StageMashIn:
      case StageMash1:
      case StageMash2:
      case StageMash3:
      case StageMash4:
      case StageMash5:
      case StageMash6:
      case StageMashOut:
        if (CurrentState == StageMashIn) {
          (_EM_PumpPreMash && ! pumpRest) ? pump_on() : pump_off();
        } else if (CurrentState == StageMashOut) {
          (_EM_PumpMashout && ! pumpRest) ? pump_on() : pump_off();
        } else {
          (_EM_PumpOnMash && ! pumpRest) ? pump_on() : pump_off();
        }
        if (MashState == MashNone) {
          _EM_StageTemp = word(er_byte(EM_StageTemp(CurrentState)), er_byte(EM_StageTemp(CurrentState) + 1)) / 16.0;
          _EM_StageTime = er_byte(EM_StageTime(CurrentState));
#if DebugProcess == true
          Debugger = true;
#endif
          if (_EM_StageTime == 0) {
            NewState = CurrentState + 1;
            break;      // skip this step
          }
          TimerSet(240 * 60);      // Just make sure the timer runs.
          stageTemp = Setpoint = _EM_StageTemp;
          PID_Heat(true);
          MashState = MashWaitTemp;

        } else if (MashState == MashWaitTemp) {
          /*
             Final wait for the Mash step temperature
          */
          PID_Heat(true);
          if (Temp_MLT < stageTemp) {
            // Reset Steady counter if temp not reached
            Steady = 0;
#if DebugProcess == true
          } else if (newMinute) {
            Debugger = true;
#endif
          }
          if ((Temp_MLT >= stageTemp) && (Steady > 20)) {
            Buzzer (3, 250);
            MashState = MashRest;
            if (CurrentState == StageMashIn)
              TimerSet(0);
            else {
              if (Resume && (ResumeTime < _EM_StageTime))
                TimerSet((ResumeTime + 1) * 60);
              else
                TimerSet(_EM_StageTime * 60);
            }
            pumpTime = 0;
#if DebugProcess == true
            Debugger = true;
          } else if (newMinute) {
            Debugger = true;
#endif
          }
        } else if (MashState == MashRest) {
          /*
             Mash step rest time
             Pump rest control
          */
          if (((CurrentState == StageMashOut) && _EM_PumpMashout) || ((CurrentState != StageMashOut) && _EM_PumpOnMash)) {
            DeltaTemp = _EM_PumpRest * stageTemp / 120; // Maximum temperature drop before heating again.
            if (pumpTime >= (_EM_PumpCycle + _EM_PumpRest) || ((stageTemp - Temp_MLT) > DeltaTemp)) {
              pumpTime = 0;
            }
            pumpRest = (pumpTime >= _EM_PumpCycle);
          }
          (pumpRest) ? bk_heat_off() : PID_Heat(true);

          /*
             End of mash step time
          */
          if (TimeLeft == 0) {
            NewState = CurrentState + 1;
            if ((CurrentState == StageMashIn) && (! er_byte(EM_SkipAdd))) {
              pump_off();
#if USE_HLT == true
              HLT_off();
#endif
              if (! WaitForConfirm(2, true, P0_stage, X1Y1_temp, P2_malt_add, P3_proceed)) {
                NewState = StageAborted;
              }
            }
            if ((CurrentState == LastMashStep) && (! er_byte(EM_SkipIodine))) {
              (_EM_PumpOnMash) ? pump_on() : pump_off();
              IodineTest();
            }
            if ((CurrentState == StageMashOut) && (! er_byte(EM_SkipRemove))) {
              pump_off();
#if USE_HLT == true
              HLT_off();
#endif
              if (! WaitForConfirm(2, (er_byte(EM_PIDPipe)), P0_stage, X1Y1_temp, P2_malt_rem, P3_proceed)) {
                NewState = StageAborted;
              }
            }
          }
#if DebugProcess == true
          if (newMinute)
            Debugger = true;
#endif
        }
#if DebugProcess == true
        if (Debugger == true) {
          DebugTimeSerial();
          Serial.print(F("Mash: "));
          Serial.print(stageName[CurrentState]);
          Serial.print(F(" State: "));
          Serial.print(MashState);
          Serial.print(F(" Temp: "));
          Serial.print(Temp_MLT);
          Serial.print(F(" Sp: "));
          Serial.print(Setpoint);
          Serial.print(F("/"));
          Serial.print(_EM_StageTemp);
          Serial.print(F(" Timer: "));
          Serial.print(TimeLeft);
          Serial.print(F(" Steady: "));
          Serial.print(Steady);
          Serial.print(F(" DeltaTemp: "));
          Serial.print(DeltaTemp);
          Serial.print(F(" pumpTime: "));
          Serial.print(pumpTime);
          Serial.print(F(" pumpRest: "));
          (pumpRest) ? Serial.println(F("true")) : Serial.println(F("false"));
          Debugger = false;
        }
#endif
        Prompt(P0_stage);
#if USE_HLT == true
        if (HLT_SetPoint)
          HLT_Heat();
        DisplayValues(true, (MashState == MashRest), Temp_HLT != 0.0, HLT_SetPoint != 0);
#else
        DisplayValues(true, (MashState == MashRest), false, false);
#endif
        Prompt(P3_xxRx);
        if ((btn_Press(ButtonStartPin, 50)) && ! Pause()) {
          NewState = StageAborted;
        }
        break;

      case StageBoil:
        if (Resume) {
          Output = 255;
          stageTemp = Setpoint = er_byte(EM_BoilTemperature);
          Prompt(P1_clear);
          Boil_output = er_byte(EM_BoilHeat);

          hopAdd = er_byte(EM_HopAddition);
          nmbrHops = er_byte(EM_NumberOfHops);
          hopTime = er_byte(EM_TimeOfHop(hopAdd));
          Resume = 0;
          if (ResumeTime < er_byte(EM_BoilTime))
            stageTime = ResumeTime + 1;
          else
            stageTime = er_byte(EM_BoilTime);
        }

#if USE_HLT == true
        DisplayValues(true, tempBoilReached, Temp_HLT != 0.0, false);
#else
        DisplayValues(true, tempBoilReached, false, false);
#endif
        (_EM_PumpOnBoil && (Temp_MLT < _EM_PumpMaxTemp)) ? pump_on() : pump_off();
        PID_Heat(false);         // No PID control during boil

        if (! tempBoilReached) {
          if (Temp_MLT < stageTemp)
            Steady = 0;
          Prompt(P0_stage);
          Prompt(P3_UDRx);
          ReadButton(Direction, Timer);
          Set(stageTemp, 105, 90, 1, Timer, Direction);
          Setpoint = stageTemp;
          if ((Temp_MLT >= stageTemp) && (Steady > 20)) {
            tempBoilReached = true;
            Buzzer(3, 250);
            stageTime = er_byte(EM_BoilTime);
            TimerSet((stageTime * 60) + 60);   // +1 minute for flameout
#if DebugProcess == true
            Debugger = true;
#endif
          }
        } else {
          // tempBoilReached
          if (newMinute)
            Prompt(P0_stage);   // Allow Hop add one minute to display

          if (TimeLeft < 60) {  // Flameout minute
            Prompt(P3_xxRx);
            Output = 0;
          } else if (Temp_MLT >= stageTemp) {
            Prompt(P3_UDRx);
            ReadButton(Direction, Timer);
            Set(Boil_output, 100, 0, 1, Timer, Direction);
            Output = Boil_output * 255 / 100;
          } else {
            Prompt(P3_xxRx);
            Output = 255;
          }

          /*
             Check for Hop (or something else) addition
          */
          if (newMinute && (hopAdd < nmbrHops) && (stageTime == hopTime)) {
            Buzzer(4, 250);
            // Put it on the display, it will be visible during one minute
            lcd.setCursor(10, 0);
            lcd.print(F(" Hop: "));
            hopAdd++;
            lcd.print(hopAdd);
            delay(500);
            Buzzer(1, 750);
            ew_byte(EM_HopAddition, hopAdd);
            hopTime = er_byte(EM_TimeOfHop(hopAdd));
          }
          if (TimeLeft == 0) {
            (_EM_Whirlpool_9) ? NewState = StageWhirlpool9 : NewState = StageCooling;
          }
        }
#if DebugProcess == true
        if (newMinute)
          Debugger = true;

        if (Debugger == true) {
          DebugTimeSerial();
          Serial.print(F("Boiling Hop: "));
          Serial.print(hopAdd);
          Serial.print(F(" Temp: "));
          Serial.print(Temp_MLT);
          Serial.print(F(" Sp: "));
          Serial.print(stageTemp);
          Serial.print(F(" Timer: "));
          Serial.print(TimeLeft);
          Serial.print(F(" Steady: "));
          Serial.print(Steady);
          Serial.print(F(" Output: "));
          Serial.print(Output);
          Serial.print(F(" Boiltemp: "));
          (tempBoilReached) ? Serial.println(F("true")) : Serial.println(F("false"));
          Debugger = false;
        }
#endif
        if (btn_Press(ButtonStartPin, 50) && ! Pause()) {
          NewState = StageAborted;
        }
        break;

      case StageCooling:
#if FakeHeating == true
        if (Temp_MLT > 16.0)
          Temp_MLT -= (gCurrentTimeInMS - FakeHeatLastInMS) * 0.000001 * (Temp_MLT - 16.0);
#endif
        Prompt(P0_stage);
        Setpoint = stageTemp;
        DisplayValues(false, false, false, false);
        Prompt(P3_UDPQ);
        ReadButton(Direction, Timer);
        if (_EM_Whirlpool_7 && ! WP7Done)
          Set(stageTemp, 77, 71, 0.25, Timer, Direction);
        else if (_EM_Whirlpool_6 && ! WP6Done)
          Set(stageTemp, 66, 60, 0.25, Timer, Direction);
        else
          Set(stageTemp, 30, 10, 0.25, Timer, Direction);
        PumpControl();
        /*
           Make some noise when aproaching the final cooling temperature.
        */
        if (! CoolBeep && (Temp_MLT < (stageTemp + 2.0))) {
          CoolBeep = true;
          Buzzer(4, 100);
        }
        if (Temp_MLT <= stageTemp)
          Buzzer(3, 250);
        if ((Temp_MLT <= stageTemp) || (btn_Press(ButtonEnterPin, 2500))) {
          if (_EM_Whirlpool_7 && ! WP7Done) {
            NewState = StageWhirlpool7;
          } else if (_EM_Whirlpool_6 && ! WP6Done) {
            NewState = StageWhirlpool6;
          } else if (_EM_Whirlpool_2) {
            NewState = StageWhirlpool2;
          } else {
            NewState = StageFinished;
          }
        }
        break;

      case StageWhirlpool2:
      case StageWhirlpool9:
      case StageWhirlpool7:
      case StageWhirlpool6:
        (Temp_MLT < _EM_PumpMaxTemp) ? pump_on() : pump_off();
        Prompt(P0_stage);
        if (TimeLeft == 120) {
          /*
             Drop the temperature when whirlpool is almost ready. If we
             are lucky the heater element will cool down so the next
             cooling stage will not wast too much energy.
          */
          if (CurrentState == StageWhirlpool9)
            Setpoint = 88.0;
          else if (CurrentState == StageWhirlpool7)
            Setpoint = 71.0;
          else if (CurrentState == StageWhirlpool6)
            Setpoint = 60.0;
          stageTemp = Setpoint;
        }
        if (CurrentState != StageWhirlpool2)
          PID_Heat(true);     // Setpoint is already set
        DisplayValues((CurrentState != StageWhirlpool2), true, false, false);
        Prompt(P3_xxRx);
        if (((btn_Press(ButtonStartPin, 50)) && (! Pause())) || (TimeLeft == 0)) {
          NewState = StageCooling;
          if (CurrentState == StageWhirlpool9)
            WP9Done = true;
          else if (CurrentState == StageWhirlpool7)
            WP7Done = true;
          else if (CurrentState == StageWhirlpool6)
            WP6Done = true;
          else if (CurrentState == StageWhirlpool2)
            NewState = StageFinished;
        }
        break;

      case StageInit:
        if (PromptForMashWater(true) == false) {
          NewState = StageAborted;
          break;
        }
#if USE_HLT == true
        HLT_SetPoint = er_byte(EM_TempHLT);
        if (HLT_SetPoint && ! PromptForMashWater(false)) {
          // No Sparge water, turn it off and continue
          HLT_SetPoint = 0;
        }
#endif
        // Initial questions, delay start etc.
        NewState = StagePrepare;
#if FakeHeating == true
        Temp_MLT = (word(er_byte(EM_StageTemp(0)), er_byte(EM_StageTemp(0) + 1)) / 16.0) - 15.0;
#if USE_HLT == true
        if (HLT_SetPoint)
          Temp_HLT = HLT_SetPoint - 4.2;
#endif
#endif
        break;

      case StagePrepare:
        /*
           Heat Mash water to 10 degrees below Mash-in
        */
        Prompt(P0_prepare);
#if USE_HLT == true
        DisplayValues((Temp_MLT < Setpoint), false, Temp_HLT != 0.0, HLT_SetPoint);
#else
        DisplayValues((Temp_MLT < Setpoint), false, false, false);
#endif
        if (Temp_MLT < Setpoint) {
          Output = 255;
          PID_Heat(false);
          break;
        }
        bk_heat_off();

#if USE_HLT == true
        /*
           Heat Sparge water if set
        */
        if (! HLT_SetPoint) {      // If HLT is off, skip heatup.
          NewState = StageDelayStart;
          break;
        }
        // Change setpoint here?
        HLT_Heat();
        if (Temp_HLT >= HLT_SetPoint) {
          Buzzer(3, 250);
          NewState = StageDelayStart;
        }
#else
        NewState = StageDelayStart;
#endif
        break;

      case StageDelayStart:
        // Wait for start, keep system hot.
        // Or, first delay and then preheat?
        NewState = StageMashIn;
        break;

      case StageAborted:
      case StageFinished:
        bk_heat_hide();
        pump_hide();
#if USE_HLT
        HLT_hide();
#endif
        lcd.clear();
        if (CurrentState == StageFinished) {
          Prompt(P1_ready);
          Buzzer(1, 1000);
        } else {
          Prompt(P1_aborted);
        }
        ew_byte(EM_AutoModeStarted, 0);
        Prompt(P3_xxxO);
        while ((btn_Press(ButtonEnterPin, 50)) == false);
        break;
    }
    newMinute = false;

  } while (CurrentState != StageAborted && CurrentState != StageFinished);

  CurrentState = StageNothing;
  lcd.clear();
}



void setup() {
#if (DebugPID == true || DebugProcess == true || DebugButton == true || DebugReadWrite == true)
  Serial.begin(115200);
#endif

  pinMode (HeatControlPin, OUTPUT);
  pinMode (PumpControlPin, OUTPUT);
  pinMode (BuzzControlPin, OUTPUT);
#if USE_HLT == true
  pinMode (HLTControlPin,  OUTPUT);
#endif
  pinMode (ButtonUpPin,    INPUT_PULLUP);
  pinMode (ButtonDownPin,  INPUT_PULLUP);
  pinMode (ButtonStartPin, INPUT_PULLUP);
  pinMode (ButtonEnterPin, INPUT_PULLUP);
  digitalWrite (HeatControlPin, LOW);
  digitalWrite (PumpControlPin, LOW);
  digitalWrite (BuzzControlPin, LOW);
#if USE_HLT == true
  digitalWrite (HLTControlPin,  LOW);
#endif

#if FakeHeating == true
  FakeHeatLastInMS =
#endif
    gSystemStartTime = gCurrentTimeInMS = millis();

  //tell the PID to range between 0 and the full window size
  myPID.SetMode(AUTOMATIC);

  lcd.begin(20, 4);
  Buzzer(1, 50);

  // write custom symbol to LCD
  lcd.createChar(0, degC);         // Celcius
  lcd.createChar(2, SP_Symbol);    // Set Point
  lcd.createChar(3, PumpONOFF);    // Pump
  lcd.createChar(4, RevPumpONOFF); // Pump
  lcd.createChar(5, HeatONOFF);    // Heat On-OFF
  lcd.createChar(6, RevHeatONOFF); // Heat On-OFF
  lcd.createChar(7, Language);     // Language Symbol

  myTimer.startTimer();            // Start the main timer
  TimerSet(0);                     // Initialize the timers.

  // EEPROM data upgrades
  if ((EEPROM.read(EM_Signature) != 'M') || (EEPROM.read(EM_Signature + 1) != 'B')) {
    // Zero all old Fahrenheit entries
    for (byte i = 0; i < 6; i++) {
      EEPROM.write(EM_StageTemp(i) + 2, 0);
      EEPROM.write(EM_StageTemp(i) + 3, 0);
    }
    EEPROM.write(EM_Signature, 'M');
    EEPROM.write(EM_Signature + 1, 'B');
    EEPROM.write(EM_Version, 0);
    EEPROM.write(EM_Revision, 1);
#if USE_ESP8266 == false
    // Erase all recipes because original ArdBir (and others)
    // are 2 bytes off.
    for (byte i = 0; i < 10; i++)
      EEPROM.write(EM_RecipeIndex(i), 0);
#endif
  }
}


void loop() {

  switch (mainMenu) {

    case 1:
      manual_mode();
      mainMenu = 0;
      break;

    case 2:
      auto_mode();
      mainMenu = 0;
      break;

    case 3:
      setup_mode();
      mainMenu = 0;
      break;

    default:
      Temperature();
      Prompt(P0_banner);
      Prompt(X6Y1_temp);
#if USE_HLT == true
      Prompt(X6Y2_temp);
#else
      Prompt(P2_clear);
#endif
      Prompt(P3_xMAS);

      if (btn_Press(ButtonDownPin, 500))
        mainMenu = 1;
      if (btn_Press(ButtonStartPin, 500))
        mainMenu = 2;
      if (btn_Press(ButtonEnterPin, 500))
        mainMenu = 3;
#if DebugErrors == true
      // "Secret" counters reset
      if (btn_Press(ButtonUpPin, 1000)) {
        lcd.clear();
        byte x = 1;
        byte y = 0;
        for (byte i = 0; i < 9; i++) {
          lcd.setCursor(x, y);
          lcd.print(er_byte(EM_ErrorNo(i)));
          if (x == 1)
            x = 7;
          else if (x == 7)
            x = 14;
          else if (x == 14) {
            x = 1;
            y++;
          }
        }
        Prompt(P3_erase);
        Buzzer(1, 100);
        while (true) {
          if (btn_Press(ButtonEnterPin, 50))
            break;
          if (btn_Press(ButtonStartPin, 50)) {
            for (byte i = 0; i < 10; i++)
              ew_byte(EM_ErrorNo(i), 0);
            break;
          }
        }
        lcd.clear();
      }
#endif
      break;
  }

}

