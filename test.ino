#define NUM_TESTS 16

typedef struct
{
  byte event;
  byte value;
  byte out;
} test_args_t;

typedef bool (*test_fn_t)(byte);

const test_fn_t test_fns [NUM_TESTS] PROGMEM =
  {
    test_event_new_location,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
  };

static const test_args_t test_args [NUM_TESTS] PROGMEM =
  {
    {EVT_LS_ROAD, LOW, ROAD},
    {EVT_CALL_BASEMENT, LOW, UP},
    {EVT_LS_BASEMENT, LOW, STOPPING},
    {EVT_LIFT_STOPPED, LOW, IDLE},
    {EVT_CALL_HOUSE, LOW, UP},
    {EVT_LS_HOUSE, LOW, STOPPING},
    {EVT_LIFT_STOPPED, LOW, IDLE},
    {EVT_CALL_HOUSE_LONG, LOW, MANUAL_UP},
    {EVT_CALL_HOUSE, HIGH, MANUAL_IDLE},
    {EVT_CALL_ROAD_LONG, LOW, MANUAL_DOWN},
    {EVT_CALL_HOUSE, HIGH, MANUAL_IDLE},
    {EVT_CALL_ROAD_LONG, LOW, MANUAL_DOWN},
    {EVT_LS_HOUSE, LOW, STOPPING},
    {EVT_LIFT_STOPPED, LOW, IDLE},
    {EVT_ESTOP, HIGH, ESTOPPED},
    {EVT_ESTOP, LOW, IDLE},
  };

static bool test_mode = false; 

bool test_mode_get (void)
{
  return  test_mode; 
}

byte check (bool test)
{
  if (test)
  {
    return true;
  }

  return false;
}

void tests_run ()
{
  int i;
  test_fn_t test_fn;

  test_mode = true;

  for (i = 0; i < NUM_TESTS; i++)
  {
    PROGMEM_readAnything (&test_fns [i], test_fn);
    if (test_fn (i) == true)
    {
      Serial.print (F ("Test "));
      Serial.print (i);
      Serial.println (F (" passed"));
    }
    else
    {
      Serial.print (F ("Test "));
      Serial.print (i);
      Serial.println (F (" failed"));
      break;
    }
  }

  Serial.print (F ("Tests: "));
  Serial.print (i - 1);
  Serial.print (F (" passed "));
  Serial.print (NUM_TESTS - i);
  Serial.println (F (" failed"));

  test_mode = false; 
}

bool test_event_new_location (byte n)
{
  test_args_t args;
  PROGMEM_readAnything (&test_args [n], args );
  sm_event_send (args.event, args.value);
  return check (location_get () == args.out);
}

bool test_event (byte n)
{
  test_args_t args;
  PROGMEM_readAnything (&test_args [n], args );
  sm_event_send (args.event, args.value);
  return check (sm_get_curr_state () == args.out);
}
