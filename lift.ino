#include <MsTimer2.h>
#include <PinChangeInterrupt.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>
#include <SimpleTimer.h>

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
  MANUAL_UP,
  MANUAL_DOWN,
  MANUAL_IDLE,
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
  Serial.println("Lift starting...");

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

const char *event_desc (byte event)
{
  switch (event)
  {
    case EVT_LS_ROAD: 
      return "Road limit switch";
   case EVT_LS_BASEMENT: 
      return "Basement limit switch";
   case EVT_LS_HOUSE: 
      return "House limit switch";
   case EVT_CALL_ROAD: 
      return "Call road button";
   case EVT_CALL_BASEMENT: 
      return "Call basement button";
   case EVT_CALL_HOUSE: 
      return "Call house button";
   case EVT_ESTOP: 
      return "Estop";
   case EVT_CALL_ROAD_LONG: 
      return "Call road button long";
   case EVT_CALL_BASEMENT_LONG: 
      return "Call basement button long";
   case EVT_CALL_HOUSE_LONG: 
      return "Call house button long";
   case EVT_LIFT_STOPPED: 
      return "Stopped";
   default:
     Serial.print ("No such event ");
     Serial.println (event);
  }
}

const char *switch_state_desc (byte state) 
{
  switch (state)
  {
    case LOW:
      return "activated";
    case HIGH:
      return "deactivated";
    default:
     Serial.print ("No such switch state ");
     Serial.println (state);
  }
};

const char *state_desc (byte state) 
{
  switch (state)
  {
    case ESTOPPED:
      return "Emergency stop state";
    case OFF:
      return "Off state";
    case TRAIN:
      return "Train state";
    case IDLE:
      return "Idle state";
    case UP:
      return "Up state";
    case DOWN:
      return "Down state";
    case MANUAL_UP:
      return "Manual up state";
    case MANUAL_DOWN:
      return "Manual down state";
    case MANUAL_IDLE:
      return "Manual idle";    
    case STOPPING:
      return "Stopping state";
    default:
      Serial.print ("No such state ");
      Serial.println (state);
  }
}

const char *location_desc (byte location)
{ 
  switch (location)
  {
    case I_AM_LOST:
      return "I am lost";
    case ROAD:
      return "Road";
    case BASEMENT:
      return "Basement";
    case HOUSE:
      return "House";
    default:
      Serial.print ("No such location ");
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
