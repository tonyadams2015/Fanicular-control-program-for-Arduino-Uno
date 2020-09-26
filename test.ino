#define NUM_TESTS 19

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
    test_event,
    test_event,
    test_event,
  };

static const test_args_t test_args [NUM_TESTS] PROGMEM =
  {
    {EVT_LS_ROAD, LOW, LOC_ROAD},
    {EVT_CALL_BASEMENT, LOW, ST_UP},
    {EVT_LS_BASEMENT, LOW, ST_STOPPING},
    {EVT_LIFT_STOPPED, LOW, ST_IDLE},
    {EVT_CALL_HOUSE, LOW, ST_UP},
    {EVT_LS_HOUSE, LOW, ST_STOPPING},
    {EVT_LIFT_STOPPED, LOW, ST_IDLE},
    {EVT_CALL_ROAD, LOW, ST_DOWN},
    {EVT_LS_BASEMENT, LOW, ST_DOWN},
    {EVT_LS_ROAD, LOW, ST_STOPPING},
    {EVT_LIFT_STOPPED, LOW, ST_IDLE},
    {EVT_LIFT_MAN_UP, LOW, ST_MANUAL_UP},
    {EVT_LIFT_MAN_UP, HIGH, ST_STOPPING},
    {EVT_LIFT_STOPPED, LOW, ST_IDLE},
    {EVT_LIFT_MAN_DOWN, LOW, ST_MANUAL_DOWN},
    {EVT_LIFT_MAN_DOWN, HIGH, ST_STOPPING},
    {EVT_LIFT_STOPPED, LOW, ST_IDLE},
    {EVT_ESTOP, HIGH, ST_ESTOPPED},
    {EVT_ESTOP, LOW, ST_IDLE},
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
  Serial.print (i);
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
  return check (loc_get () == args.out);
}

bool test_event (byte n)
{
  test_args_t args;
  PROGMEM_readAnything (&test_args [n], args );
  sm_event_send (args.event, args.value);
  return check (sm_get_curr_state () == args.out);
}
