// Program used to test the USB Joystick library when used as 
// a Flight Controller on the Arduino Leonardo or Arduino 
// Micro.
//
// Matthew Heironimus
// 2016-05-29 - Original Version
//------------------------------------------------------------

#include "Joystick.h"


#define joyX A0
#define joyY A1
#define joyZ A2
#define joyThrottle A3


#define joyRangeGap 150

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, 14, 1,
  true, true, false, false, false, true,
  false, true, false, false, false);

// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
//const bool testAutoSendMode = true;
const bool testAutoSendMode = false;

const unsigned long gcCycleDelta = 1000;
const unsigned long gcAnalogDelta = 25;
const unsigned long gcButtonDelta = 500;
unsigned long gNextTime = 0;
unsigned int gCurrentStep = 0;
unsigned int lastHatStatus = 0 ;
int lastHatButton = -1 ;

int lastButtonState[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define firePin 2
#define fireJoyButton 1
#define leftPin 3
#define leftJoyButton 2
#define middlePin 4
#define middleJoyButton 3
#define rightPin 5
#define rightJoyButton 4
#define hatupPin 6
#define hatupJoyButton 5
#define hatleftPin 7
#define hatleftJoyButton 6
#define hatrightPin 8
#define hatrightJoyButton 7
#define hatdownPin 9
#define hatdownJoyButton 8
#define f1Pin 10
#define f1JoyButton 9
#define f2Pin 16
#define f2JoyButton 10
#define f3Pin 1
#define f3JoyButton 11
#define f4Pin 0
#define f4JoyButton 12
#define lshiftPin 15
#define lshiftJoyButton 13
#define rshiftPin 14
#define rshiftJoyButton 14



void testSingleButtonPush(unsigned int button)
{
  if (button > 0)
  {
    Joystick.releaseButton(button - 1);
  }
  if (button < 32)
  {
    Joystick.pressButton(button);
  }
}

void testMultiButtonPush(unsigned int currentStep) 
{
  for (int button = 0; button < 32; button++)
  {
    if ((currentStep == 0) || (currentStep == 2))
    {
      if ((button % 2) == 0)
      {
        Joystick.pressButton(button);
      } else if (currentStep != 2)
      {
        Joystick.releaseButton(button);
      }
    } // if ((currentStep == 0) || (currentStep == 2))
    if ((currentStep == 1) || (currentStep == 2))
    {
      if ((button % 2) != 0)
      {
        Joystick.pressButton(button);
      } else if (currentStep != 2)
      {
        Joystick.releaseButton(button);
      }
    } // if ((currentStep == 1) || (currentStep == 2))
    if (currentStep == 3)
    {
      Joystick.releaseButton(button);
    } // if (currentStep == 3)
  } // for (int button = 0; button < 32; button++)
}

void testXYAxis(unsigned int currentStep)
{
  int xAxis;
  int yAxis;
  
  if (currentStep < 256)
  {
    xAxis = currentStep - 127;
    yAxis = -127;
    Joystick.setXAxis(xAxis);
    Joystick.setYAxis(yAxis);
  } 
  else if (currentStep < 512)
  {
    yAxis = currentStep - 256 - 127;
    Joystick.setYAxis(yAxis);
  }
  else if (currentStep < 768)
  {
    xAxis = 128 - (currentStep - 512);
    Joystick.setXAxis(xAxis);
  }
  else if (currentStep < 1024)
  {
    yAxis = 128 - (currentStep - 768);
    Joystick.setYAxis(yAxis);
  }
  else if (currentStep < 1024 + 128)
  {
    xAxis = currentStep - 1024 - 127;
    Joystick.setXAxis(xAxis);
    Joystick.setYAxis(xAxis);
  }
}

void testThrottleRudder(unsigned int value)
{
  Joystick.setThrottle(value);
  Joystick.setRudder(255 - value);
}

void checkAndSetButton( unsigned int pin,  unsigned int joyButton)
{
  int currentButtonState = !digitalRead(pin);
  int index = joyButton -1 ;
  if  (currentButtonState != lastButtonState[index])   
  {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
  }

}

void checkAndSetHat() 
{
  // first get hat status
  int currentButtonState = 0;
  unsigned int hatStatus = 0;
  currentButtonState = !digitalRead(hatupPin);
  if (currentButtonState == 1 )
  {
    hatStatus = hatStatus + 1;
  }
  currentButtonState = !digitalRead(hatrightPin);
  if (currentButtonState == true )
  {
    hatStatus = hatStatus + 2;
  }
  currentButtonState = !digitalRead(hatdownPin);
  if (currentButtonState == true )
  {
    hatStatus = hatStatus + 4;
  }
  currentButtonState = !digitalRead(hatleftPin);
  if (currentButtonState == true )
  {
    hatStatus = hatStatus + 8;
  }
  if (hatStatus != lastHatStatus)
  {
    switch (hatStatus) {
      case 0:
        if (lastHatButton != -1)
        {
          Joystick.setHatSwitch(0, JOYSTICK_HATSWITCH_RELEASE);
        }
        lastHatButton = -1;
        break;
      case 1:
        Joystick.setHatSwitch(0, 0);
        lastHatButton = 1;
        break;
      case 2:
        Joystick.setHatSwitch(0, 90);
        lastHatButton = 2;
        break;
      case 3:
        Joystick.setHatSwitch(0, 45);
        lastHatButton = 3;
        break;
      case 4:
        Joystick.setHatSwitch(0, 180);
        lastHatButton = 4;
        break;
      case 6:
        Joystick.setHatSwitch(0, 135);
        lastHatButton = 6;
        break;
      case 8:
        Joystick.setHatSwitch(0, 270);
        lastHatButton = 8;
        break;
      case 9:
        Joystick.setHatSwitch(0, 315);
        lastHatButton = 9;
        break;
      case 12:
        Joystick.setHatSwitch(0, 225);
        lastHatButton = 12;
        break;
      default:
        Joystick.setHatSwitch(0, JOYSTICK_HATSWITCH_RELEASE);
        break;
       
    }
    lastHatStatus = hatStatus ;
    
  }
}

void setup() {


    // Initialize Button Pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);  
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);

  pinMode(A0, INPUT);  // X-axis analogue
  pinMode(A1, INPUT);  // Y-axis analogue
  pinMode(A2, INPUT);  // Zangle analogue
  pinMode(A3, INPUT);  // Throttle analogue

  Joystick.setXAxisRange(0, 1023 - joyRangeGap - joyRangeGap);
  Joystick.setYAxisRange(0, 1023 - joyRangeGap - joyRangeGap);
  Joystick.setThrottleRange(0, 1023);
  Joystick.setRzAxisRange(0, 1023);
  
  if (testAutoSendMode)
  {
    Joystick.begin();
  }
  else
  {
    Joystick.begin(false);
  }
      
}


void loop() {


  int xValue = analogRead(joyX);
  int yValue = analogRead(joyY);
  int zValue = analogRead(joyZ);
  int throttleValue = analogRead(joyThrottle);
  int index = 0;
  int currentButtonState = 0;
  
  xValue = xValue - joyRangeGap;
  yValue = yValue - joyRangeGap;
  throttleValue = throttleValue - joyRangeGap - 50;
  Joystick.setXAxis(xValue);
  Joystick.setYAxis(yValue);
  Joystick.setRzAxis(zValue);
  Joystick.setThrottle(throttleValue);

  checkAndSetButton( firePin,  fireJoyButton);
  checkAndSetButton( leftPin,  leftJoyButton);
  checkAndSetButton( middlePin,  middleJoyButton);
  checkAndSetButton( rightPin,  rightJoyButton);
  checkAndSetButton( hatupPin,  hatupJoyButton);
  checkAndSetButton( hatleftPin,  hatleftJoyButton);
  checkAndSetButton( hatrightPin,  hatrightJoyButton);
  checkAndSetButton( hatdownPin,  hatdownJoyButton);
  checkAndSetButton( f1Pin,  f1JoyButton);
  checkAndSetButton( f2Pin,  f2JoyButton);
  checkAndSetButton( f3Pin,  f3JoyButton);
  checkAndSetButton( f4Pin,  f4JoyButton);
  
  checkAndSetButton( lshiftPin,  lshiftJoyButton);
  checkAndSetButton( rshiftPin,  rshiftJoyButton);
  
  
  checkAndSetHat();
  
  
    if (testAutoSendMode == false)
    {
      Joystick.sendState();
    }
    

  delay(50);
  
}
