static int sm_curr_state = OFF;

void (*sm_enter_cb [NUM_STATES])(void) = {sm_enter_estop,
                                          sm_enter_off,
                                          sm_enter_train,
                                          sm_enter_idle,
                                          sm_enter_up,
                                          sm_enter_down,
                                          sm_enter_manual,
                                          sm_enter_stopping};
                                          
void (*sm_cb [NUM_STATES])(int, long) =   {sm_estop,
                                           sm_off,
                                           sm_train,
                                           sm_idle,
                                           sm_up,
                                           sm_down,
                                           sm_manual,
                                           sm_stopping};
                            
void (*sm_exit_cb [NUM_STATES])(void)  = {sm_exit_estop,
                                          sm_exit_off,
                                          sm_exit_train,
                                          sm_exit_idle, 
                                          sm_exit_up,
                                          sm_exit_down,
                                          sm_exit_manual,
                                          sm_exit_stopping};

String state_desc [NUM_STATES] = {"Emergency stop state",
                                  "Off state",
                                  "Train state",
                                  "Idle state",
                                  "Up state",
                                  "Down state",
                                  "Manual state",
                                  "Stopping state"};

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
        location_set (event);
      }
      break;
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
    case EVT_CALL_ROAD_LONG:
    case EVT_CALL_HOUSE_LONG:
      if (value == LOW)
      {
        location_cmd_set (event);
      }
      break;
    case EVT_ESTOP:
      if (value == HIGH)
      {
        sm_next_state (ESTOPPED);
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
    Serial.println ("Entering " + state_desc[sm_get_curr_state ()] + "\n");
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

void sm_enter_estop (void)
{
  lift_stop ();
}


void sm_enter_off (void)
{
  lift_stop ();
  
  if (check_inputs_ready () == true)
  {
    if (location_get () != I_AM_LOST)
    {
    sm_next_state (IDLE);
    }
    else
    {
      sm_next_state (TRAIN);
    }
  }
}

void sm_enter_train (void)
{
  lift_stop ();
}

void sm_enter_idle (void)
{
  lift_stop ();
}

void sm_enter_up (void)
{
  lift_up ();
}

void sm_enter_down (void)
{
  lift_down ();
}

void sm_enter_manual (void)
{
  lift_up ();
}

void sm_enter_stopping (void)
{
}

void sm_estop (int event, long value)
{
  if (event == EVT_ESTOP && value == LOW)
  {
    sm_next_state (OFF);
  }
}

void sm_off (int event, long value)
{  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
      if (check_inputs_ready () == true)
      {
        if (location_get () != I_AM_LOST)
        {
          sm_next_state (IDLE);
        }
        else
        {
          sm_next_state (TRAIN);
        }
      }
      break;
    case EVT_ESTOP:
      if (value == LOW && check_inputs_ready () == true)
      {
        if (location_get () != I_AM_LOST)
        {
          sm_next_state (IDLE);
        }
        else
        {
          sm_next_state (TRAIN);
        }
      }
      break;
  }
}

void sm_train (int event, long value)
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
      if (location_get () != I_AM_LOST)
      {
        sm_next_state (IDLE);
      }
      break;
    case EVT_CALL_ROAD_LONG:
      sm_next_state (DOWN);
      break;
    case EVT_CALL_HOUSE_LONG:
      sm_next_state (UP);
      break;
  }
}

void sm_idle (int event, long value)
{
  byte next;
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
      sm_next_state (how_to_move_to_location (event));
      break;
    case EVT_CALL_HOUSE_LONG:
      if (location_get () == HOUSE)
      {
        sm_next_state (MANUAL);
      }
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
      sm_next_state (STOPPING);
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
      sm_next_state (STOPPING);
      break;
  }
}

/* Only available above HOUSE
 */
void sm_manual (int event, long value)
{
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      sm_next_state (IDLE);
      break;
    case EVT_CALL_ROAD:
    case EVT_CALL_HOUSE:
      /* Button released */
      if (value == HIGH)
      {
        lift_stop ();
      }
      break;
    case EVT_CALL_ROAD_LONG:
      lift_down ();
      break;
    case EVT_CALL_HOUSE_LONG:
      lift_up ();
      break;
  }
}

void sm_stopping (int event, long value)
{
  if (event == EVT_LIFT_STOPPED)
  {
    sm_next_state (IDLE);
  }
}

void sm_exit_estop (void)
{
  
}

void sm_exit_off (void)
{
  
}

void sm_exit_train (void)
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

void sm_exit_manual (void)
{

}

void sm_exit_stopping (void)
{

}
