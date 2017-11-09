#ifndef DEFINES_H
#define DEFINES_H

#define  VERSION        "0.2.5"

/*
   EEPROM MAP
    PID MENU
*/
#define  EM_PIDBase           0
//                            0   // (UseGas, Obsolete, placeholder, do not remove!).
//                            1   // kP, obsolete
//                            2   // kI, obsolete
//                            3   // kD, obsolete
#define  EM_SampleTime        4   // SampleTime
//                            5   // WindowSize, obsolete
#define  EM_BoilHeat          6   // Boil Heat %
#define  EM_TempOffset        7   // Temperature Offset
//                            8   // Hysteresis (obsolete, for Gas).
#if USE_HLT == true
#define  EM_TempHLT           9   // Temperature HLT
#endif
/*
    UNIT MENU
*/
#define  EM_UnitBase         10
//                           10   // Scale Temp (Obsolete, placeholder, do not remove!).
#define  EM_SensorType       11   // Intern or Extern
#define  EM_BoilTemperature  12   // Boil temperature °C
//                           13   // Boil temperature °F
#if USE_PumpPWM == true
#define  EM_PumpSlow         13   // Pump Slow PWM value
#endif
#define  EM_PumpCycle        14   // Pump Cycle minutes
#define  EM_PumpRest         15   // Pump Rest minutes
#define  EM_PumpPreMash      16   // Pump Premash
#define  EM_PumpOnMash       17   // Pump during Mash
#define  EM_PumpMashout      18   // Pump on Mashout
#define  EM_PumpOnBoil       19   // Pump during boil
#define  EM_PumpMaxTemp      20   // Pump max temp °C
//                           21   // Pump max temp °F (Obsolete)
#define  EM_PIDPipe          22   // PID Pipe
#define  EM_WaitAdd          23   // Skip Add Malt
#define  EM_WaitRemove       24   // Skip Remove Malt
#define  EM_WaitIodine       25   // Skip Iodine test
#define  EM_IodoneTime       26   // Iodine timeout
//                           27   // Whirlpool (Obsolete)
//                           28
//                           29
//                           30
#define  EM_ActiveRecipe     31   // Recipe loaded 1..10

/*
  RUN AUTOMATION
  In each step there used to be Farenheit settings on
  bytes 2 and 3. They are free and some are used for
  other recipe settings.
*/
#define  EM_RunBase          32
#define  EM_StageTemp(i)     ((EM_RunBase)+(i)*5)
#define  EM_StageTime(i)     ((EM_RunBase)+(i)*5+4)

// 0:   32 -  36 MashIn
#define  EM_CoolingTemp      34   // Cooling destination temp Hi-byte.
//                           35   // Cooling destination temp Lo-byte.
// 1:   37 -  41 Phytase
#define  EM_Whirlpool_9      39   // Zero or the Hot Whirlpool time 88..100 °C
#define  EM_Whirlpool_7      40   // Zero or the Sub Isomerization Whirlpool time 71..77 °C
// 2:   42 -  46 Glucanase
#define  EM_Whirlpool_6      44   // Zero or the "Tepid" Whirlpool time 60..66 °C
#define  EM_Whirlpool_2      45   // Zero or the Cold Whirlpool time < 30 °C
// 3:   47 -  51 Protease
// 4:   52 -  55 B-amylase
// 5:   57 -  61 A-Amylase 1
// 6:   62 -  66 A-Amylase 2
#define  EM_Signature        64   // EEPROM data signature 'M'
//                           65   // EEPROM data signature 'B'
// 7:   67 -  71 Mash Out
#define  EM_Version          69   // EEPROM data version 0
#define  EM_Revision         70   // EEPROM data revision 1


#define  EM_NumberOfHops     72   // Number of hop additions
#define  EM_BoilTime         73   // Boil time
#define  EM_HopTimeBase      74   // 74 - 83 Hop timers
#define  EM_TimeOfHop(i)     ((EM_HopTimeBase)+(i))
/*
    74       Time Hop  1
    75       Time Hop  2
    76       Time Hop  3
    77       Time Hop  4
    78       Time Hop  5
    79       Time Hop  6
    80       Time Hop  7
    81       Time Hop  8
    82       Time Hop  9
    83       Time Hop 10
*/

#define  EM_AutoModeStarted  84   // FLAG Automode Started
#define  EM_StageResume      85   // Resume Stage
#define  EM_StageTimeLeft    86   // Resume time left
#define  EM_HopAddition      87   // Hop Addition counter

/*
	 88 -  89 [ SPACE ]

    RECIPE
*/
#define  EM_RecipeIndexBase  90   // Index 1-10
#define  EM_RecipeIndex(i)   ((EM_RecipeIndexBase)+(i))
#define  EM_RecipeDataBase  100   // Recipe data
#define  EM_RecipeData(i)    ((EM_RecipeDataBase)+(i)*52)
#define  EM_RecipeNameBase  620   // Recipe name
#define  EM_RecipeName(i)    ((EM_RecipeNameBase)+(i)*10)
/*
	 90 -  99 Index 1-10
	100 - 151 Recipe Data  1
	152 - 203 Recipe Data  2
	204 - 255 Recipe Data  3
	256 - 307 Recipe Data  4
	308 - 359 Recipe Data  5
	360 - 411 Recipe Data  6
	412 - 463 Recipe Data  7
	464 - 515 Recipe Data  8
	516 - 567 Recipe Data  9
	568 - 619 Recipe Data 10
	620 - 629 Recipe Name  1
	630 - 639 Recipe Name  2
	640 - 649 Recipe Name  3
	650 - 659 Recipe Name  4
	660 - 669 Recipe Name  5
	670 - 679 Recipe Name  6
	680 - 689 Recipe Name  7
	690 - 699 Recipe Name  8
	700 - 709 Recipe Name  9
	710 - 719 Recipe Name 10
*/



#if DebugErrors == true
#define  EM_ErrorBase         900
#define  EM_ErrorNo(i)        ((EM_ErrorBase)+(i))
#endif

/*
   New locations
*/
#define  EM_NewBase           920
#define  EM_Marker1           EM_NewBase          // AA
#define  EM_Marker2           EM_NewBase+1        // 55
#define  EM_NewVersion        EM_NewBase+2        // New EEPROM data version
#define  EM_NewRevision       EM_NewBase+3        // New EEPROM data revision
#define  EM_ManualMLT         EM_NewBase+4        // Last MLT manual temperature (uint16_t)
#define  EM_ManualHLT         EM_NewBase+6        // Last HLT manual temperature
#define  EM_PID_Kp            EM_NewBase+8        // PID P (uint16_t)
#define  EM_PID_Ki            EM_NewBase+10       // PID I (uint16_t)
#define  EM_PID_Kd            EM_NewBase+12       // PID D (uint16_t)

#define  PID_Kp_max           2048                // Maximum setup value
#define  PID_Kp_step          PID_Kp_max / 65536.0
#define  PID_Kp_div           65536 / PID_Kp_max
#define  PID_Ki_max           128                 // Maximum setup value
#define  PID_Ki_step          PID_Ki_max / 65536.0
#define  PID_Ki_div           65536 / PID_Ki_max
#define  PID_Kd_max           32768               // Maximum setup value
#define  PID_Kd_step          PID_Kd_max / 65536.0
#define  PID_Kd_div           65536 / PID_Kd_max


//Stage names
#define StageMashIn           0     // Mash-in
#define StageMash1            1     // Opt. Phytase
#define StageMash2            2     // Opt. Glucanase
#define StageMash3            3     // Opt. Protease
#define StageMash4            4     // Opt. B-Amylase
#define StageMash5            5     // Opt. A-Amylase1
#define StageMash6            6     // A-Amylase2 + Iodine
#define StageMashOut          7     // Mash-out
#define StageBoil             8     // Boil
#define StageCooling          9     // Cooling
#define StageWhirlpool2      10     // Whirlpool cold
#define StageWhirlpool9      11     // Whirlpool 88..100 °C
#define StageWhirlpool7      12     // Whirlpool 71..77 °C
#define StageWhirlpool6      13     // Whirlpool 60..66 °C
#define StageInit            50     // Initial questions.
#define StageDelayStart      51     // Delayed start.
#define StagePrepareHLT      52     // Prepare HLT, preheat etc.
#define StageAborted         90     // Aborted by user.
#define StageFinished        91     // Normal finished brew.
#define StageNothing        255     // There is no Stage.

#define DirectionNone         0
#define DirectionUp           1
#define DirectionDown         2

// Internal Mash states
#define MashNone              0
#define MashWaitTemp          1     // Wait for target temperature
#define MashRest              2     // Rest state

#define WhirlpoolMaxtime    120     // Maximum 120 minutes


byte degC[8]         = {B01000, B10100, B01000, B00111, B01000, B01000, B01000, B00111};  // [0] degree c sybmol
byte SP_Symbol[8]    = {B11100, B10000, B11100, B00111, B11101, B00111, B00100, B00100};  // [2] SP Symbol
byte PumpONOFF[8]    = {B00000, B01110, B01010, B01110, B01000, B01000, B01000, B00000};  // [3] Pump Symbol
byte RevPumpONOFF[8] = {B11111, B10001, B10101, B10001, B10111, B10111, B10111, B11111};  // [4] Reverse PUMP Symbol
byte HeatONOFF[8]    = {B00000, B01010, B01010, B01110, B01110, B01010, B01010, B00000};  // [5] HEAT symbol
byte RevHeatONOFF[8] = {B11111, B10101, B10101, B10001, B10001, B10101, B10101, B11111};  // [6] reverse HEAT symbol
#if langNL == true
byte Language[8]     = {B00001, B00001, B11111, B00000, B11111, B00100, B01000, B11111};  // [7] NL symbol
#else
byte Language[8]     = {B11111, B00100, B01000, B11111, B00000, B10001, B10101, B11111};  // [7] EN symbol
#endif

#endif

