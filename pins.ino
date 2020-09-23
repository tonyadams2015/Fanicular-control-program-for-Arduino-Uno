
#define NUM_INPUT_PINS 11
#define DEBOUNCE_TIME 50
#define LONG_PRESS_COUNT 200

typedef struct input_config 
{
  byte pin;
  bool valid;
  byte val;
  byte pin_mode; 
  bool debounce_active;
  byte trigger;
  void (*isr) (void);
  byte event;
} input_config_t;

typedef struct input_state 
{
  byte val; 
  bool debounce_active;
  int on_count;
} input_state_t;

static const input_config_t icfg [NUM_INPUT_PINS] PROGMEM = 
  {{},
   {},
   {PIN_LS_ROAD, true, HIGH, INPUT, false, CHANGE , pin_ls_road_isr, EVT_LS_ROAD},
   {PIN_LS_BASEMENT, true, HIGH, INPUT, false, CHANGE, pin_ls_basement_isr, EVT_LS_BASEMENT},
   {PIN_LS_HOUSE, true, HIGH, INPUT, false, CHANGE, pin_ls_house_isr, EVT_LS_HOUSE},
   {PIN_CALL_ROAD, true, HIGH, INPUT, false, CHANGE, pin_call_road_isr, EVT_CALL_ROAD},
   {PIN_CALL_BASEMENT, true, HIGH, INPUT, false, CHANGE, pin_call_basement_isr, EVT_CALL_BASEMENT},
   {PIN_CALL_HOUSE, true, HIGH, INPUT, false, CHANGE, pin_call_house_isr, EVT_CALL_HOUSE},
   {PIN_ESTOP, true, HIGH, INPUT, false, CHANGE, pin_estop_isr ,EVT_ESTOP},
   {PIN_MAN_UP, true, HIGH, INPUT, false, CHANGE, pin_estop_isr ,EVT_ESTOP},
   {PIN_MAN_DOWN, true, HIGH, INPUT, false, CHANGE, pin_estop_isr ,EVT_ESTOP}};

ArduinoQueue<int> event_queue(10);

static input_state_t istate [NUM_INPUT_PINS] =
  {{},
   {},
   {HIGH, false, 0},
   {HIGH, false, 0},
   {HIGH, false, 0},
   {HIGH, false, 0},
   {HIGH, false, 0},
   {HIGH, false, 0},
   {HIGH, false, 0},
   {HIGH, false, 0},
   {HIGH, false, 0}};

void pins_init (void)
{ 
  MsTimer2::set(DEBOUNCE_TIME, process_inputs);
  MsTimer2::start();
  int pin;
  input_config_t cfg;

  for (pin = 0; pin < NUM_INPUT_PINS; pin++)
  {
    PROGMEM_readAnything (&icfg [pin], cfg);
    if (cfg.valid)
    {
      pinMode (cfg.pin, cfg.pin_mode);
      attachPCINT(digitalPinToPCINT(cfg.pin), cfg.isr ,cfg.trigger);
      istate[pin].val = digitalRead (pin);
    }
  }

  pinMode (PIN_LIFT_UP, OUTPUT);
  pinMode (PIN_LIFT_DOWN, OUTPUT);
  digitalWrite(PIN_LIFT_UP, HIGH);
  digitalWrite(PIN_LIFT_DOWN, HIGH);
}

static void process_interrupt (int pin)
{
  event_queue.enqueue (pin);
}

static void process_inputs (void)
{
  int pin = 0;
  bool val;
  input_config_t cfg;
  

  for (pin = 0; pin < NUM_INPUT_PINS; pin++)
  {
    PROGMEM_readAnything (&icfg [pin], cfg);
    if (cfg.valid == false)
    {
      continue;  
    }

    /* Debounce */
    if (istate[pin].debounce_active == true)
    {
      val = digitalRead (pin);
      
      if (istate[pin].val == val)
      {
        istate[pin].debounce_active = false;
        if (val == LOW)
        {
          sm_event_send (cfg.event, 0);
        }
        else
        {
          sm_event_send (cfg.event, 1);
        }
      }
      else
      {
        istate[pin].val = val;
      }
    }

    /* Look for long button presses */
    if ((cfg.pin == PIN_CALL_HOUSE ||
        cfg.pin == PIN_CALL_BASEMENT ||
        cfg.pin == PIN_CALL_ROAD) &&
        istate[pin].val == LOW)
    {
      istate[pin].on_count ++;
      if (istate[pin].on_count > LONG_PRESS_COUNT)
      {
        istate[pin].on_count = 0;

        switch (cfg.pin)
        {
          case PIN_CALL_HOUSE:
            sm_event_send (EVT_CALL_HOUSE_LONG, 0);
            break;
          case PIN_CALL_BASEMENT:
            sm_event_send (EVT_CALL_BASEMENT_LONG, 0);
            break;
          case PIN_CALL_ROAD:
            sm_event_send (EVT_CALL_ROAD_LONG, 0);
            break;
        }
      }
    }
    else
    {
      istate[pin].on_count = 0;
    }
  }

  /* Get get the next key press */
  if (event_queue.isEmpty () == false)
  {
    pin = event_queue.dequeue ();
    
    istate[pin].debounce_active = true;
    istate[pin].val = digitalRead (pin);
  }
}

static byte get_pin_state (byte pin)
{
  return istate[pin].val;
}

/* Check all limit switches and buttons off  */
static bool check_inputs_ready (void)
{
  int pin;
  input_config_t cfg;

  for (pin = 0; pin < NUM_INPUT_PINS; pin ++)
  {
    PROGMEM_readAnything (&icfg [pin], cfg);
    if (cfg.valid == true && istate[pin].val != HIGH)
    {
      Serial.print (F ("Error: switch or button on during boot - "));
      Serial.print (F ("Failed on pin "));
      Serial.println (pin);
      return false;
    }
  }

  return true;
}

static void pin_ls_road_isr ()
{
  process_interrupt (PIN_LS_ROAD);
}

static void pin_ls_basement_isr ()
{
  process_interrupt (PIN_LS_BASEMENT);
}

static void pin_ls_house_isr (void)
{
  process_interrupt (PIN_LS_HOUSE);
}

static void pin_call_road_isr ()
{
  process_interrupt (PIN_CALL_ROAD);
}

static void pin_call_basement_isr ()
{
  process_interrupt (PIN_CALL_BASEMENT);
}

static void pin_call_house_isr ()
{
  process_interrupt (PIN_CALL_HOUSE);
}

static void pin_estop_isr ()
{
  process_interrupt (PIN_ESTOP);
}
