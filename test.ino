#define NUM_TESTS 16

typedef struct
{
  byte event;
  byte value;
  byte out;
} test_args_t;

bool (*test_fn [NUM_TESTS])(byte) = 
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

static test_args_t args [NUM_TESTS] =
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

  test_mode = true;

  for (i = 0; i < NUM_TESTS; i++)
  {
    if (test_fn [i] (i) == true)
    {
      Serial.print ("Test ");
      Serial.print (i);
      Serial.println (" passed");
    }
    else
    {
      Serial.print ("Test ");
      Serial.print (i);
      Serial.println (" failed");
      break;
    }
  }

  Serial.print ("Tests: ");
  Serial.print (i - 1);
  Serial.print (" passed ");
  Serial.print (NUM_TESTS - i);
  Serial.println (" failed");

   test_mode = false; 
}

bool test_event_new_location (byte n)
{
  sm_event_send (args [n].event, args [n].value);
  return check (location_get () == args [n].out);
}

bool test_event (byte n)
{
  sm_event_send (args [n].event, args [n].value);
  return check (sm_get_curr_state () == args [n].out);
}
