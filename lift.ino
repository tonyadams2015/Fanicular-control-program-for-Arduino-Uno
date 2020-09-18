#include <MsTimer2.h>
#include <PinChangeInterrupt.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>
#include <SimpleTimer.h>

#define NUM_STATES 8
#define NUM_EVENTS 11
#define SUCCESS 0
#define FAILURE -1

/* Pin GPIO numbers */
#define PIN_LS_ROAD 2
#define PIN_LS_BASEMENT 3
#define PIN_LS_HOUSE 4
#define PIN_CALL_ROAD 5 
#define PIN_CALL_BASEMENT 6
#define PIN_CALL_HOUSE 7
#define PIN_ESTOP 8
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 10
#define MOTOR_STOP_TIME 5000

enum events 
{
  EVT_LS_ROAD = 1,
  EVT_LS_BASEMENT,
  EVT_LS_HOUSE,
  EVT_CALL_ROAD,
  EVT_CALL_BASEMENT,
  EVT_CALL_HOUSE,
  EVT_ESTOP,
  EVT_CALL_ROAD_LONG,
  EVT_CALL_BASEMENT_LONG,
  EVT_CALL_HOUSE_LONG,
  EVT_LIFT_STOPPED,
  EVT_MAX};

enum states
{
  ESTOPPED,
  OFF,
  TRAIN,
  IDLE,
  UP,
  DOWN,
  MANUAL,
  STOPPING
};

enum locations 
{
  I_AM_LOST,
  ROAD, 
  BASEMENT, 
  HOUSE, 
  LOCATION_MAX
};

SimpleTimer timer;

void setup() {
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor\n");

  if (location_load () != SUCCESS)
  {
    return;
  }
  
  pins_init ();
  sm_init ();
}

void loop()
{
  timer.run ();
}

const char *event_desc (byte event)
{
  switch (event)
  {
    case 1: 
      return "Road limit switch";
   case 2: 
      return "Basement limit switch";
   case 3: 
      return "House limit switch";
   case 4: 
      return "Call road button";
   case 5: 
      return "Call basement button";
   case 6: 
      return "Call house button";
   case 7: 
      return "Estop";
   case 8: 
      return "Call road button long";
   case 9: 
      return "Call basement button long";
   case 10: 
      return "Call house button long";
   case 11: 
      return "Stopped";
  }
}

const char *switch_state_desc (byte state) 
{
  switch (state)
  {
    case 0:
      return "activated";
    case 1:
      return "deactivated";
  }
};

const char *state_desc (byte state) 
{
  switch (state)
  {
    case 0:
      return "Emergency stop state";
    case 1:
      return "Off state";
    case 2:
      return "Train state";
    case 3:
      return "Idle state";
    case 4:
      return "Up state";
    case 5:
      return "Down state";
    case 6:
      return "Manual state";
    case 7:
      return "Stopping state";
  }
}

const char *location_desc (byte location)
{ 
  switch (location)
  {
    case 0:
      return "I am lost";
    case 1:
      return "Road";
    case 2:
      return "Basement";
    case 3:
      return "House";
      
  }
}

void lift_stop (void)
{
  digitalWrite(PIN_FAN_UP, HIGH);
  digitalWrite(PIN_FAN_DOWN, HIGH);
}

void lift_up (void)
{
  digitalWrite(PIN_FAN_UP, LOW);
}

void lift_down (void)
{
  digitalWrite(PIN_FAN_DOWN, LOW);
}

void lift_stopping (void)
{
  timer.setTimeout (MOTOR_STOP_TIME, lift_stopped);
}

void lift_stopped (void)
{
  sm_event_send (EVT_LIFT_STOPPED, 0);
}
