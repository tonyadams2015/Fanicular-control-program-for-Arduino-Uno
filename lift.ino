#include <MsTimer2.h>
#include <PinChangeInterrupt.h>
#include <ArduinoQueue.h>

#define NUM_STATES 7
#define PIN_LS_ROAD 2
#define PIN_LS_BASEMENT 3
#define PIN_LS_HOUSE 4
#define PIN_CALL_ROAD 5 
#define PIN_CALL_BASEMENT 6
#define PIN_CALL_HOUSE 7
#define PIN_ESTOP 8
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 10
#define I_AM_LOST -1

enum events {EVT_LS_ROAD, EVT_LS_BASEMENT, EVT_LS_HOUSE, EVT_CALL_ROAD,
             EVT_CALL_BASEMENT, EVT_CALL_HOUSE, EVT_ESTOP};
enum states {OFF, MANUAL, IDLE, UP, DOWN};

static int lift_location = I_AM_LOST;

void setup() {
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor\n");
  pins_init ();
  sm_init ();
}

void loop()
{

}

void lift_stop (void)
{
  digitalWrite(PIN_FAN_UP, HIGH);
  digitalWrite(PIN_FAN_DOWN, HIGH);
}

void lift_location_set (byte event)
{
  lift_location = event;
}

int lift_location_get ()
{
  return lift_location;
}

bool lift_how_to_move (byte location)
{
  switch (location)
  {
    case EVT_CALL_ROAD:
      switch (lift_location)
      {
        case EVT_LS_ROAD:
          return IDLE;
        case EVT_LS_BASEMENT:
        case EVT_LS_HOUSE:
          return DOWN; 
      }
    case EVT_CALL_BASEMENT:
      switch (lift_location)
      {
        case EVT_LS_ROAD:
          return UP;
        case EVT_LS_BASEMENT:
          return IDLE;
        case EVT_LS_HOUSE:
          return DOWN; 
      }
      case EVT_CALL_HOUSE:
      switch (lift_location)
      {
        case EVT_LS_ROAD:
          return UP;
        case EVT_LS_BASEMENT:
          return UP;
        case EVT_LS_HOUSE:
          return IDLE; 
      }
  }
}

void lift_up (void)
{
  digitalWrite(PIN_FAN_UP, LOW);
}

void lift_down (void)
{
  digitalWrite(PIN_FAN_DOWN, LOW);
}
