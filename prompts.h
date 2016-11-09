#ifndef  PROMPTS_H
#define  PROMPTS_H



#define  P0_clear        100
#define  P0_banner       101
#define  P0_manual       102
#define  P0_auto         103
#define  P0_setup        104
#define  P0_stage        105
#define  P0_iodine       106
#define  P0_prepare      107

#define  P1_clear        200
#define  P1_pBKwater     201
#define  P1_pHLTwater    202
#define  P1_ready        203
#define  P1_aborted      204
#define  P1_cool         205
#define  P1_whirl        206
#define  P1_twhirl       207
#define  P1_prime        208
#define  P1_resume       209

#define  P2_clear        300
#define  P2_malt_add     301
#define  P2_malt_rem     302
#define  P2_norecipe     303

#define  P3_clear        400
#define  P3_xMAS         401
#define  P3_xBPQ         402
#if USE_HLT == true
#define  P3_HBPQ         403
#endif
#define  P3_UD0Q         404
#define  P3_UD1Q         405
#define  P3_xx0Q         406
#define  P3_xx1Q         407
#define  P3_proceed      408
#define  P3_xxxO         409
#define  P3_xxSO         410
#define  P3_QQxO         411
#define  P3_SGQO         412
#define  P3_xGQO         413
#define  P3_SxQO         414
#define  P3_UDPQ         415
#define  P3_UDRx         416
#define  P3_xxRx         417
#define  P3_QQBO         418
#define  P3_OOxO         419
#define  P3_UDxQ         420
#if DebugErrors == true
#define  P3_erase        421
#endif

#define  X6Y1_temp       501
#define  X1Y1_temp       502
#define  X11Y1_setpoint  503
#define  X1Y2_pwm        504
#define  X1Y2_blank      505
#define  X11Y2_timer     506
#if USE_HLT == true
#define  X6Y2_temp       507
#define  X1Y2_temp       508
#define  X11Y2_setpoint  509
#endif


#if langNL == true
const char *stageName[] = { "Maisch In ",
                            "Maisch #1 ",
                            "Maisch #2 ",
                            "Maisch #3 ",
                            "Maisch #4 ",
                            "Maisch #5 ",
                            "Maisch #6 ",
                            "Maisch Uit",
                            "Koken     ",
                            "Koelen    ",
                            "Whirlpool ",
                            "Whirlpool ",
                            "Whirlpool ",
                            "Whirlpool "
                          };
#else
const char *stageName[] = { "Mash In   ",
                            "Mash #1   ",
                            "Mash #2   ",
                            "Mash #3   ",
                            "Mash #4   ",
                            "Mash #5   ",
                            "Mash #6   ",
                            "Mash Out  ",
                            "Boil      ",
                            "Cooling   ",
                            "Whirlpool ",
                            "Whirlpool ",
                            "Whirlpool ",
                            "Whirlpool "
                          };
#endif


void LCDSpace (byte Num) {
  for (byte i = 0; i < Num; i++)
    lcd.write(32);
}


void FormatNumber(float Numero, int Offset) {
  byte Space = 0;
  if (Numero <= -100.0)
    Space = 0;
  if (Numero <=  -10.0 && Numero > -100.0)
    Space = 1;
  if (Numero <     0.0 && Numero >  -10.0)
    Space = 2;
  if (Numero <    10.0 && Numero >=   0.0)
    Space = 3;
  if (Numero <   100.0 && Numero >=  10.0)
    Space = 2;
  if (Numero <  1000.0 && Numero >= 100.0)
    Space = 1;
  if (Numero >= 1000.0)
    Space = 0;

  LCDSpace(Space + Offset);
}


/*
   LCD messages
*/
void Prompt(int Pmpt) {
  float Temp;

  if (Pmpt == 0)
    return;

  if ((Pmpt >= 100) && (Pmpt < 200)) {
    lcd.setCursor(0, 0);
  } else if ((Pmpt >= 200) && (Pmpt < 300)) {
    lcd.setCursor(1, 1);
  } else if ((Pmpt >= 300) && (Pmpt < 400)) {
    lcd.setCursor(0, 2);
  } else if ((Pmpt >= 400) && (Pmpt < 500)) {
    lcd.setCursor(0, 3);
  }

  switch (Pmpt) {
    case P0_clear:
      LCDSpace(20);
      return;
    case P0_banner:
      lcd.print(F("MBSE ArdRims " VERSION " \x07"));
      return;
    case P0_manual:
#if langNL == true
      lcd.print(F("   HANDBEDIENING    "));
#else
      lcd.print(F("    MANUAL  MODE    "));
#endif
      return;
    case P0_auto:
#if langNL == true
      lcd.print(F("    AUTOMATISCH     "));
#else
      lcd.print(F("   AUTOMATIC MODE   "));
#endif
      return;
    case P0_setup:
#if langNL == true
      lcd.print(F("    INSTELLINGEN    "));
#else
      lcd.print(F("     SETUP MODE     "));
#endif
      return;
    case P0_stage:
      lcd.print(F(" AUTO --> "));
      lcd.print(stageName[CurrentState]);
      return;
    case P0_iodine:
#if langNL == true
      lcd.print(F(" ** JODIUM PROEF ** "));
#else
      lcd.print(F(" ** IODINE  TEST ** "));
#endif
      return;
    case P0_prepare:
#if langNL == true
      lcd.print(F(" **   OPWARMEN   ** "));
#else
      lcd.print(F(" **   PRE-HEAT   ** "));
#endif
      return;


    case P1_clear:
      LCDSpace(18);
      return;
    case P1_pBKwater:
#if langNL == true
      lcd.print(F("Maischwater Aanw? "));
#else
      lcd.print(F(" Have Mash water? "));
#endif
      return;
#if USE_HLT == true
    case P1_pHLTwater:
#if langNL == true
      lcd.print(F(" Spoelwater Aanw? "));
#else
      lcd.print(F("Have Sparge water?"));
#endif
      return;
#endif
    case P1_ready:
#if langNL == true
      lcd.print(F(" Brouwen is Klaar "));
#else
      lcd.print(F(" Brew is finished "));
#endif
      return;
    case P1_aborted:
#if langNL == true
      lcd.print(F("Brouwen Afgebroken"));
#else
      lcd.print(F("  Brew  aborted   "));
#endif
      return;
    case P1_cool:
#if langNL == true
      lcd.print(F("   START KOELEN   "));
#else
      lcd.print(F("  START  COOLING  "));
#endif
      return;
    case P1_whirl:
#if langNL == true
      lcd.print(F(" START  WHIRLPOOL "));
#else
      lcd.print(F(" START  WHIRLPOOL "));
#endif
      return;
    case P1_twhirl:
#if langNL == true
      lcd.print(F("  Whirlpool Tijd  "));
#else
      lcd.print(F("  Whirlpool Time  "));
#endif
      return;
    case P1_prime:
#if langNL == true
      lcd.print(F(" Pomp Ontluchten  "));
#else
      lcd.print(F("    Pump Prime    "));
#endif
      return;
    case P1_resume:
#if langNL == true
      lcd.print(F("  Hervat Proces?  "));
#else
      lcd.print(F(" Resume  Process? "));
#endif
      return;

    case P2_clear:
      LCDSpace(20);
      return;
    case P2_malt_add:
#if langNL == true
      lcd.print(F("    Mout Storten   "));
#else
      lcd.print(F("      Add Malt     "));
#endif
      return;
    case P2_malt_rem:
#if langNL == true
      lcd.print(F("  Mout Verwijderen "));
#else
      lcd.print(F("    Remove Malt    "));
#endif
      return;
    case P2_norecipe:
#if langNL == true
      lcd.print(F("   GEEN RECEPTEN!  "));
#else
      lcd.print(F("    NO  RECIPES!   "));
#endif
      return;

    case P3_clear:
      LCDSpace(20);
      return;
    case P3_xMAS:
#if langNL == true
      lcd.print(F(" --  HAND AUTO SETUP"));
#else
      lcd.print(F(" --  MAN  AUTO SETUP"));
#endif
      return;
    case P3_xBPQ:
#if langNL == true
      lcd.print(F(" --  KOOK POMP STOP "));
#else
      lcd.print(F(" --   BK  PUMP QUIT "));
#endif
      return;
#if USE_HLT == true
    case P3_HBPQ:
#if langNL == true
      lcd.print(F("HWAT KOOK POMP STOP "));
#else
      lcd.print(F("HLT   BK  PUMP QUIT "));
#endif
      return;
#endif
    case P3_UD1Q:
#if langNL == true
      lcd.print(F("Op   Neer Aan  Terug"));
#else
      lcd.print(F("Up   Down  On  Quit "));
#endif
      return;
    case P3_UD0Q:
#if langNL == true
      lcd.print(F("Op   Neer Uit  Terug"));
#else
      lcd.print(F("Up   Down Off  Quit "));
#endif
      return;
    case P3_xx1Q:
#if langNL == true
      lcd.print(F(" --   --  Aan  Terug"));
#else
      lcd.print(F(" --   --   On  Quit "));
#endif
      return;
    case P3_xx0Q:
#if langNL == true
      lcd.print(F(" --   --  Uit  Terug"));
#else
      lcd.print(F(" --   --  Off  Quit "));
#endif
      return;
    case P3_proceed:
#if langNL == true
      lcd.print(F("Doorgaan:   Ja  Nee "));
#else
      lcd.print(F("Continue:   Yes  No "));
#endif
      return;
    case P3_xxxO:
#if langNL == true
      lcd.print(F("                 Ok "));
#else
      lcd.print(F("                 Ok "));
#endif
      return;
    case P3_xxSO:
#if langNL == true
      lcd.print(F("           Niet  Ok "));
#else
      lcd.print(F("           Skip  Ok "));
#endif
      return;
    case P3_QQxO:
#if langNL == true
      lcd.print(F(" Op  Neer        Ok "));
#else
      lcd.print(F(" Up  Down        Ok "));
#endif
      return;
    case P3_SGQO:
#if langNL == true
      lcd.print(F(" Op  Neer  Terug  Ok"));
#else
      lcd.print(F(" Up  Down  Quit  Ok "));
#endif
      return;
    case P3_xGQO:
#if langNL == true
      lcd.print(F(" --  Neer  Terug  Ok"));
#else
      lcd.print(F(" --  Down  Quit  Ok "));
#endif
      return;
    case P3_SxQO:
#if langNL == true
      lcd.print(F(" Op   --   Terug  Ok"));
#else
      lcd.print(F(" Up   --   Quit  Ok "));
#endif
      return;
    case P3_UDPQ:
#if langNL == true
      lcd.print(F(" Op  Neer Pomp Terug"));
#else
      lcd.print(F(" Up  Down  Pump Quit"));
#endif
      return;
    case P3_UDRx:
#if langNL == true
      lcd.print(F(" Op  Neer Pauze     "));
#else
      lcd.print(F(" Up  Down Pause     "));
#endif
      return;
    case P3_xxRx:
#if langNL == true
      lcd.print(F("          Pauze     "));
#else
      lcd.print(F("          Pause     "));
#endif
      return;
    case P3_QQBO:
#if langNL == true
      lcd.print(F("OP* *NEER  Terug Ok "));
#else
      lcd.print(F("UP*  *DWN   Back Ok "));
#endif
      return;
    case P3_OOxO:
#if langNL == true
      lcd.print(F(" On   Off        Ok "));
#else
      lcd.print(F("Aan   Uit        Ok "));
#endif
      return;
    case P3_UDxQ:
#if langNL == true
      lcd.print(F(" Op  Neer      Terug"));
#else
      lcd.print(F(" Up  Down       Quit"));
#endif
      return;

#if DebugErrors == true
    case P3_erase:
      lcd.print(F("Clear all:  Yes  No "));
      return;
#endif

    case X6Y1_temp:
    case X1Y1_temp:
      if (Pmpt == X6Y1_temp)
        lcd.setCursor(6, 1);
      else
        lcd.setCursor(1, 1);
      Temp = Temp_MLT;
      FormatNumber(Temp, -1);
      lcd.print(Temp, 2);
      lcd.write((byte)0);
      lcd.write(32);
      return;

    case X11Y1_setpoint:
      lcd.setCursor(11, 1);
      Temp = Setpoint;
      FormatNumber(Temp, -1);
      lcd.print(Temp, 2);
      lcd.write(2);
      return;

    case X1Y2_pwm:
      lcd.setCursor(1, 2);
      lcd.print(F("PWM="));
      Temp = (Output * 100 ) / 255;
      FormatNumber(Temp, -1);
      lcd.print(Temp, 0);      //Display output%
      lcd.print(F("% "));
      return;

    case X1Y2_blank:
      lcd.setCursor(1, 2);
      LCDSpace(9);
      return;

    case X11Y2_timer:
      TimerShow(TimeLeft, 11, 2);
      return;

#if USE_HLT == true
    case X6Y2_temp:
    case X1Y2_temp:
      if (Pmpt == X6Y2_temp)
        lcd.setCursor(6, 2);
      else
        lcd.setCursor(1, 2);
      Temp = Temp_HLT;
      FormatNumber(Temp, -1);
      lcd.print(Temp, 2);
      lcd.write((byte)0);
      lcd.write(32);
      return;

    case X11Y2_setpoint:
      lcd.setCursor(11, 2);
      Temp = HLT_SetPoint;
      FormatNumber(Temp, -1);
      lcd.print(Temp, 2);
      lcd.write(2);
      lcd.write(32);
      return;
#endif
  }
}


boolean PromptForMashWater(boolean Mash) {
#if USE_HLT == true
  (Mash) ? Prompt(P1_pBKwater) : Prompt(P1_pHLTwater);
#else
  Prompt(P1_pBKwater);
#endif
  Prompt(P3_proceed);
  while (true) {
    AllThreads();
    if (button_Used(buttonStart, 50))
      return true;
    if (button_Used(buttonEnter, 50))
      return false;
  }
}


/*
   Wait for confirm
    Type = 1         choices Yes ---
    Type = 2         choices Yes  No
    Pid = true       run PID
    Pid = false      no PID
    P0, P1, P2, P3   prompts to display or zero
*/
boolean WaitForConfirm(byte Type, boolean Pid, int P0, int P1, int P2, int P3) {

  BuzzerPlay(BUZZ_Prompt);
  while (true) {
    AllThreads();

    Input = Temp_MLT;
    Prompt(P0);
    Prompt(P1);
    Prompt(P2);
    Prompt(P3);

    if (Pid)
      PID_Heat(true);

    if (button_Used(buttonStart, 50))
      return true;
    if (Type == 2 && button_Used(buttonEnter, 50))
      return false;
  }
}


/*
   Returns true  if Yes
           false if No
*/
boolean Pause() {

  bk_heat_off();
  pump_off();
#if USE_HLT == true
  HLT_off();
#endif

  lcd.setCursor(0, 0);
#if langNL == true
  lcd.print(F(" -- PAUZE  PAUZE -- "));
  lcd.setCursor(1, 2);
  lcd.print(F( "-- PAUZE  PAUZE --"));
#else
  lcd.print(F(" -- PAUSE  PAUSE -- "));
  lcd.setCursor(1, 2);
  lcd.print(F( "-- PAUSE  PAUSE --"));
#endif
  Prompt(P3_proceed);

  while (true) {
    AllThreads();

    Prompt(X1Y1_temp);
    Prompt(X11Y1_setpoint);

    if (button_Used(buttonStart, 50))
      break;
    if (button_Used(buttonEnter, 1500))
      return false;
  }
  Prompt(P0_clear);
  Prompt(P2_clear);
  return true;
}


#endif
