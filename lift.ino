#include <MsTimer2.h>
#include <PinChangeInterrupt.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>
#include <SimpleTimer.h>
#include <Arduino.h>

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
#define PIN_MAN_UP 9
#define PIN_MAN_DOWN 10
#define PIN_LIFT_UP 11
#define PIN_LIFT_DOWN 12

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
  EVT_LIFT_MAN_UP,
  EVT_LIFT_MAN_DOWN,
  EVT_MAX};

enum states
{
  ST_ESTOPPED,
  ST_OFF,
  ST_TRAIN,
  ST_IDLE,
  ST_UP,
  ST_DOWN,
  ST_MANUAL_UP,
  ST_MANUAL_DOWN,
  ST_STOPPING,
  ST_STATE_MAX
};

enum locations 
{
  LOC_LOST,
  LOC_ROAD, 
  LOC_BASEMENT, 
  LOC_HOUSE, 
  LOC_MAX
};

SimpleTimer timer;

void setup() {
  Serial.begin(9600);  
  Serial.println(F ("Lift control program starting..."));

  sm_init ();
  tests_run ();

  if (loc_load () != SUCCESS)
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

String event_desc (byte event)
{
  switch (event)
  {
    case EVT_LS_ROAD: 
      return F("Road limit switch");
   case EVT_LS_BASEMENT: 
      return F("Basement limit switch");
   case EVT_LS_HOUSE: 
      return F("House limit switch");
   case EVT_CALL_ROAD: 
      return F("Call road button");
   case EVT_CALL_BASEMENT: 
      return F("Call basement button");
   case EVT_CALL_HOUSE: 
      return F("Call house button");
   case EVT_ESTOP: 
      return F("Estop");
   case EVT_CALL_ROAD_LONG: 
      return F("Call road button long");
   case EVT_CALL_BASEMENT_LONG: 
      return F("Call basement button long");
   case EVT_CALL_HOUSE_LONG: 
      return F("Call house button long");
   case EVT_LIFT_STOPPED: 
      return F("Stopped");
   case EVT_LIFT_MAN_UP: 
      return F("MANUAL UP");
   case EVT_LIFT_MAN_DOWN: 
      return F("MANUAL DOWN");
   default:
     Serial.print (F("No such event "));
     Serial.println (event);
  }
}

String switch_state_desc (byte state) 
{
  switch (state)
  {
    case LOW:
      return F("activated");
    case HIGH:
      return F("deactivated");
    default:
     Serial.print (F("No such switch state "));
     Serial.println (state);
  }
};

String state_desc (byte state) 
{
  switch (state)
  {
    case ST_ESTOPPED:
      return F("Emergency stop state");
    case ST_OFF:
      return F("Off state");
    case ST_TRAIN:
      return F("Train state");
    case ST_IDLE:
      return F("Idle state");
    case ST_UP:
      return F("Up state");
    case ST_DOWN:
      return F("Down state");
    case ST_MANUAL_UP:
      return F("Manual up state");
    case ST_MANUAL_DOWN:
      return F("Manual down state");
    case ST_STOPPING:
      return F("Stopping state");
    default:
      Serial.print (F("No such state "));
      Serial.println (state);
  }
}

String location_desc (byte location)
{ 
  switch (location)
  {
    case LOC_LOST:
      return F("Lost");
    case LOC_ROAD:
      return F("Road");
    case LOC_BASEMENT:
      return F("Basement");
    case LOC_HOUSE:
      return F("House");
    default:
      Serial.print (F("(No such location "));
      Serial.println (location);
  }
}

void lift_stop (void)
{
  digitalWrite(PIN_LIFT_UP, HIGH);
  digitalWrite(PIN_LIFT_DOWN, HIGH);
}

void lift_up (void)
{
  digitalWrite(PIN_LIFT_UP, LOW);
}

void lift_down (void)
{
  digitalWrite(PIN_LIFT_DOWN, LOW);
}

void lift_stopping (void)
{
  timer.setTimeout (MOTOR_STOP_TIME, lift_stopped);
}

void lift_stopped (void)
{
  sm_event_send (EVT_LIFT_STOPPED, 0);
}

template <typename T> void PROGMEM_readAnything (const T * sce, T& dest)
  {
  memcpy_P (&dest, sce, sizeof (T));
  }

template <typename T> T PROGMEM_getAnything (const T * sce)
  {
  static T temp;
  memcpy_P (&temp, sce, sizeof (T));
  return temp;
  }
