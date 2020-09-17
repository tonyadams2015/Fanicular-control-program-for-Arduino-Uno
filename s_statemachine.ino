static int sm_curr_state = OFF;

void (*sm_enter_cb [NUM_STATES])(void) = {sm_enter_off,
                                          sm_enter_manual,
                                          sm_enter_idle,
                                          sm_enter_up,
                                          sm_enter_down};
                                          
void (*sm_cb [NUM_STATES])(int, long) =   {sm_off,
                                           sm_manual,
                                           sm_idle,
                                           sm_up,
                                           sm_down};
                            
void (*sm_exit_cb [NUM_STATES])(void)  = {sm_exit_off,
                                          sm_exit_manual,
                                          sm_exit_idle, 
                                          sm_exit_up,
                                          sm_exit_down};

String event_desc [NUM_EVENTS] = {"Road limit switch",
                                  "Basement limit switch",
                                  "House limit switch",
                                  "Call road button",
                                  "Call basement button",
                                  "Call house button",
                                  "Estop"};

String state_desc [NUM_STATES] = {"Off state",
                                  "Manual state",
                                  "Idle state",
                                  "Up state",
                                  "Down state"};

String switch_state_desc [2] = {"activated", "deactivated"};
                                          
void sm_init (void)                              
{
  sm_next_state (OFF);
}

void sm_event_send (int event, long value)
{
  Serial.println ("Processing " +  event_desc [event] + " = " + switch_state_desc [value] + "\n");

  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      if (value == LOW)
      {
        lift_location_set (event);
      }
      break;  
  }
  
  sm_update (event, value);
}

int sm_get_curr_state (void)
{
  return sm_curr_state;
}

void sm_next_state (int new_state)
{
  sm_exit ();
  sm_curr_state = new_state;
  sm_enter ();
}

void sm_enter (void)
{
  if (sm_enter_cb [sm_curr_state] != NULL)
  {
    sm_enter_cb [sm_curr_state] ();
  }
}

void sm_update (int event, long value)
{
  if (sm_cb [sm_curr_state] != NULL)
  {
    sm_cb [sm_curr_state] (event, value);
  }
}

void sm_exit (void)
{
  if (sm_exit_cb [sm_curr_state] != NULL)
  {
    sm_exit_cb [sm_curr_state] ();
  }
}

void sm_enter_off (void)
{
  Serial.println ("Entering " + state_desc[sm_get_curr_state ()] + "\n");

  lift_stop ();
  
  if (check_inputs_ready () == true)
  {
    if (lift_location_get () != I_AM_LOST)
    {
    sm_next_state (IDLE);
    }
    else
    {
      sm_next_state (MANUAL);
    }
  }
}

void sm_enter_manual (void)
{
  Serial.println("Entering " + state_desc[sm_get_curr_state ()] + "\n");
  lift_stop ();
}

void sm_enter_idle (void)
{
  Serial.println("Entering " + state_desc[sm_get_curr_state ()] + "\n");
  lift_stop ();
}

void sm_enter_up (void)
{
  Serial.println ("Entering " + state_desc[sm_get_curr_state ()] + "\n");

  lift_up ();
}

void sm_enter_down (void)
{
  Serial.println ("Entering " + state_desc[sm_get_curr_state ()] + "\n");

  lift_down ();
}

void sm_off (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      if (check_inputs_ready () == true)
      {
        if (lift_location_get () != I_AM_LOST)
        {
          sm_next_state (IDLE);
        }
        else
        {
          sm_next_state (MANUAL);
        }
      }
      break;
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
    case EVT_ESTOP:
      break;
  }
}

void sm_manual (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      if (lift_location_get () != I_AM_LOST)
      {
        sm_next_state (IDLE);
      }
      break;
    case EVT_CALL_ROAD:
      sm_next_state (DOWN);
      break;
    case EVT_CALL_BASEMENT:
      break;
    case EVT_CALL_HOUSE:
      sm_next_state (UP);
      break;
    case EVT_ESTOP:
      sm_next_state (OFF);
      break;
  }
}

void sm_idle (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      break;
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
      sm_next_state (lift_how_to_move (event));
      break;
    case EVT_ESTOP:
      sm_next_state (OFF);
      break;
  }
}

void sm_up (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      sm_next_state (IDLE);
      break;
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
      break;
    case EVT_ESTOP:
      sm_next_state (OFF);
      break;
  }
}
 
void sm_down (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      sm_next_state (IDLE);
      break;
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
      break;
    case EVT_ESTOP:
      sm_next_state (OFF);
      break;
  }
}

void sm_exit_off (void)
{
  
}

void sm_exit_manual (void)
{
  
}

void sm_exit_idle (void)
{
  
}

void sm_exit_up (void)
{
  
}

void sm_exit_down (void)
{

}
