#include <MsTimer2.h>
#include <PinChangeInterrupt.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>

#define NUM_STATES 5
#define NUM_EVENTS 7
#define PIN_LS_ROAD 2
#define PIN_LS_BASEMENT 3
#define PIN_LS_HOUSE 4
#define PIN_CALL_ROAD 5 
#define PIN_CALL_BASEMENT 6
#define PIN_CALL_HOUSE 7
#define PIN_ESTOP 8
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 10
#define I_AM_LOST 0

enum events {EVT_LS_ROAD = 1, EVT_LS_BASEMENT, EVT_LS_HOUSE, EVT_CALL_ROAD,
             EVT_CALL_BASEMENT, EVT_CALL_HOUSE, EVT_ESTOP};
enum states {OFF, MANUAL, IDLE, UP, DOWN};

String location_desc [4] = {"I am lost", "Road", "Basement", "House"};

static byte lift_location = I_AM_LOST;
static int address = 0x0;

void setup() {
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor\n");

  /* Get location */
  lift_location = EEPROM.read(address);

  Serial.println ("We are at the " + location_desc [lift_location] + "\n");
  
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
  EEPROM.write(0, event);
  Serial.println ("Writing new location " + location_desc [event] + " to EEPROM\n");
}

int lift_location_get ()
{
  if (lift_location == I_AM_LOST)
  {
    Serial.println ("I am lost.\n");
  }
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
