#include <PinChangeInterrupt.h>

#define PIN_LS_ROAD 0
#define PIN_LS_BASEMENT 1
#define PIN_LS_HOUSE 2
#define PIN_CALL_ROAD 4 
#define PIN_CALL_BASEMENT 5
#define PIN_CALL_HOUSE 6
#define PIN_ESTOP 3
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 9

void pins_init (void)
{
  pinMode (PIN_LS_ROAD, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(PIN_LS_ROAD), pin_ls_road_isr, LOW);

  pinMode (PIN_LS_BASEMENT, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(PIN_LS_ROAD), pin_ls_basement_isr, LOW);

  pinMode (PIN_LS_HOUSE, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(PIN_LS_HOUSE), pin_ls_house_isr, LOW);

  pinMode (PIN_CALL_ROAD, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(PIN_CALL_ROAD), pin_call_road_isr, LOW);

  pinMode (PIN_CALL_BASEMENT, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(PIN_CALL_BASEMENT), pin_call_basement_isr, LOW);

  pinMode (PIN_CALL_HOUSE, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(PIN_CALL_HOUSE), pin_call_house_isr, LOW);

  pinMode (PIN_ESTOP, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(PIN_ESTOP), pin_estop_isr, LOW);

  pinMode (PIN_FAN_UP, OUTPUT);
  pinMode (PIN_FAN_DOWN, OUTPUT);
}

void pin_ls_road_isr ()
{
  sm_event_send (EVT_LS_ROAD, 0);
}

void pin_ls_basement_isr ()
{
  sm_event_send (EVT_LS_BASEMENT, 0);
}

void pin_ls_house_isr ()
{
  sm_event_send (EVT_LS_HOUSE, 0);
}

void pin_call_road_isr ()
{
  sm_event_send (EVT_CALL_ROAD, 0);
}

void pin_call_basement_isr ()
{
  sm_event_send (EVT_CALL_BASEMENT, 0);
}

void pin_call_house_isr ()
{
  sm_event_send (EVT_CALL_HOUSE, 0);
}

void pin_estop_isr ()
{
  sm_event_send (EVT_ESTOP, 0);
}
