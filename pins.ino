#define PIN_NULL 0
#define PIN_LS_ROAD 2
#define PIN_LS_BASEMENT 3
#define PIN_LS_HOUSE 4
#define PIN_CALL_ROAD 5 
#define PIN_CALL_BASEMENT 6
#define PIN_CALL_HOUSE 7
#define PIN_ESTOP 8
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 10
#define NUM_INPUT_PINS 10

typedef struct input_config 
{
  int pin;
  bool valid;
  byte val; 
  bool debounce_active;
  byte trigger;
  void (*isr) (void);
  byte event;
} input_config_t;

typedef struct input_state 
{
  bool valid;
  byte val; 
  bool debounce_active;
} input_state_t;

static input_config_t icfg [NUM_INPUT_PINS] = 
  {{PIN_NULL, false, HIGH, false, CHANGE, NULL, 0},
   {PIN_NULL, false, HIGH, false, CHANGE, NULL, 0},
   {PIN_LS_ROAD, true, HIGH, false, CHANGE, pin_ls_road_isr, EVT_LS_ROAD},
   {PIN_LS_BASEMENT, true, HIGH, false, CHANGE, pin_ls_basement_isr, EVT_LS_BASEMENT},
   {PIN_LS_HOUSE, true, HIGH, false, CHANGE, pin_ls_house_isr, EVT_LS_HOUSE},
   {PIN_CALL_ROAD, true, HIGH, false, CHANGE, pin_call_road_isr, EVT_CALL_ROAD},
   {PIN_CALL_BASEMENT, true, HIGH, false, CHANGE, pin_call_basement_isr, EVT_CALL_BASEMENT},
   {PIN_CALL_HOUSE, true, HIGH, false, CHANGE, pin_call_basement_isr, EVT_CALL_HOUSE},
   {PIN_ESTOP, true, HIGH, false, CHANGE, pin_estop_isr, EVT_ESTOP},
   {PIN_NULL, false, HIGH, false, CHANGE, NULL, 0}};

ArduinoQueue<int> event_queue(10);

static input_config_t istate [NUM_INPUT_PINS] =
  {{false, HIGH, false},
   {false, HIGH, false},
   {true, HIGH, false},
   {true, HIGH, false},
   {true, HIGH, false},
   {true, HIGH, false},
   {true, HIGH, false},
   {true, HIGH, false},
   {true, HIGH, false},
   {false, HIGH, false}};

void pins_init (void)
{ 
  MsTimer2::set(100, debounce);
  MsTimer2::start();
  int pin;

  for (pin = 0; pin < NUM_INPUT_PINS; pin++)
  {
    if (icfg[pin].valid)
    {
      pinMode (icfg[pin].pin, INPUT);
      attachPCINT(digitalPinToPCINT(icfg[pin].pin), icfg[pin].isr ,icfg[pin].trigger);
    }
  }

  pinMode (PIN_FAN_UP, OUTPUT);
  pinMode (PIN_FAN_DOWN, OUTPUT);
}

void process_interrupt (int pin)
{
  event_queue.enqueue (pin);
}

void debounce (void)
{
  int pin = 0;
  bool val;

  for (pin = 0; pin < NUM_INPUT_PINS; pin++)
  {
    if (istate[pin].valid == false)
    {
      continue;  
    }
    
    if (istate[pin].debounce_active == true)
    {
      val = digitalRead (pin);
      
      if (istate[pin].val == val)
      {
        istate[pin].debounce_active = false;
        if (val == LOW)
        {
          sm_event_send (icfg[pin].event, 0);
        }
      }
      else
      {
        istate[pin].val = val;
      }
    }     
  }

  /* Get get the next key press */
  if (event_queue.isEmpty () == false)
  {
    pin = event_queue.dequeue ();
    Serial.println(pin);
    
    istate[pin].debounce_active = true;
    istate[pin].val = digitalRead (pin);
    Serial.println(istate[pin].val);
  }
}

void pin_ls_road_isr ()
{
  process_interrupt (PIN_LS_ROAD);
}

void pin_ls_basement_isr ()
{
  process_interrupt (PIN_LS_BASEMENT);
}

void pin_ls_house_isr (void)
{
  process_interrupt (PIN_LS_HOUSE);
}

void pin_call_road_isr ()
{
  process_interrupt (PIN_CALL_ROAD);
}

void pin_call_basement_isr ()
{
  process_interrupt (PIN_CALL_BASEMENT);
}

void pin_call_house_isr ()
{
  process_interrupt (PIN_CALL_HOUSE);
}

void pin_estop_isr ()
{
  process_interrupt (PIN_ESTOP);
}
