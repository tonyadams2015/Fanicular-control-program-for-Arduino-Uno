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
#define PIN_FAN_UP 11
#define PIN_FAN_DOWN 12

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
  ESTOPPED,
  OFF,
  TRAIN,
  IDLE,
  UP,
  DOWN,
  MANUAL_UP,
  MANUAL_DOWN,
  STOPPING,
  STATE_MAX
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
  Serial.println(F ("Lift starting..."));

  sm_init ();
  tests_run ();

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
    case ESTOPPED:
      return F("Emergency stop state");
    case OFF:
      return F("Off state");
    case TRAIN:
      return F("Train state");
    case IDLE:
      return F("Idle state");
    case UP:
      return F("Up state");
    case DOWN:
      return F("Down state");
    case MANUAL_UP:
      return F("Manual up state");
    case MANUAL_DOWN:
      return F("Manual down state");
    case STOPPING:
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
    case I_AM_LOST:
      return F("I am lost");
    case ROAD:
      return F("Road");
    case BASEMENT:
      return F("Basement");
    case HOUSE:
      return F("House");
    default:
      Serial.print (F("(No such location "));
      Serial.println (location);
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
