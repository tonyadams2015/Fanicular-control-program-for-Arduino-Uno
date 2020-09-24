static int sm_curr_state = OFF;

void (*sm_enter_cb [STATE_MAX])(void) = {sm_enter_estop,
                                          sm_enter_off,
                                          sm_enter_train,
                                          sm_enter_idle,
                                          sm_enter_up,
                                          sm_enter_down,
                                          sm_enter_manual_up,
                                          sm_enter_manual_down,
                                          sm_enter_stopping};
                                          
void (*sm_cb [STATE_MAX])(int, long) =   {sm_estop,
                                           sm_off,
                                           sm_train,
                                           sm_idle,
                                           sm_up,
                                           sm_down,
                                           sm_manual_up,
                                           sm_manual_down,
                                           sm_stopping};

void (*sm_exit_cb [STATE_MAX])(void) = {};
                                                           
void sm_init (void)                              
{
  sm_next_state (OFF);
}

void sm_event_send (int event, long value)
{
  Serial.print (F ("Processing "));
  Serial.print (event_desc (event));
  Serial.print (F (" = "));
  Serial.println (switch_state_desc (value));
  
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
    case EVT_LIFT_STOPPED:
      break;
    case EVT_LIFT_MAN_UP:
    case EVT_LIFT_MAN_DOWN:
      break;
    default:
      Serial.print (F ("No such event "));
      Serial.println (event);
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

static void sm_enter (void)
{
  if (sm_enter_cb [sm_curr_state] != NULL)
  {
    Serial.print (F ("Entering "));
    Serial.println (state_desc (sm_get_curr_state ()));
    
    sm_enter_cb [sm_curr_state] ();
  }
}

static void sm_update (int event, long value)
{
  if (sm_cb [sm_curr_state] != NULL)
  {
    sm_cb [sm_curr_state] (event, value);
  }
}

static void sm_exit (void)
{
  if (sm_exit_cb && sm_exit_cb [sm_curr_state] != NULL)
  {
    sm_exit_cb [sm_curr_state] ();
  }
}

static void sm_enter_estop (void)
{
  lift_stop ();
}


static void sm_enter_off (void)
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

static void sm_enter_train (void)
{
  lift_stop ();
}

static void sm_enter_idle (void)
{
  lift_stop ();
}

static void sm_enter_up (void)
{
  lift_up ();
}

static void sm_enter_down (void)
{
  lift_down ();
}

static void sm_enter_manual_up (void)
{
  lift_up ();
}

static void sm_enter_manual_down (void)
{
  lift_down ();
}

static void sm_enter_stopping (void)
{
}

static void sm_estop (int event, long value)
{
  if (event == EVT_ESTOP && value == LOW)
  {
    sm_next_state (OFF);
  }
}

static void sm_off (int event, long value)
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

static void sm_train (int event, long value)
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

static void sm_idle (int event, long value)
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
      location_target_set (ROAD);
      sm_next_state (location_directory ());
      break;
    case EVT_CALL_BASEMENT:
      location_target_set (BASEMENT);
      sm_next_state (location_directory ());
      break;
    case EVT_CALL_HOUSE:
      location_target_set (HOUSE);
      sm_next_state (location_directory ());
      break;
    case EVT_LIFT_MAN_UP:
      sm_next_state (MANUAL_UP);
      break;
    case EVT_LIFT_MAN_DOWN:
      sm_next_state (MANUAL_DOWN);
      break;     
  }
}

static void sm_up (int event, long value)
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
      sm_next_state (location_directory ());
      break;
  }
}
 
static void sm_down (int event, long value)
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
      sm_next_state (location_directory ());
      break;
  }
}

/* Only available above HOUSE
 */
static void sm_manual_up (int event, long value)
{
  switch (event)
  {
    case EVT_LS_ROAD:
      sm_next_state (STOPPING);
      break;
    case EVT_LIFT_MAN_UP:
      /* Button released */
      if (value == HIGH)
      {
         sm_next_state (STOPPING);
      }
      break;
  }
}

static void sm_manual_down (int event, long value)
{
  switch (event)
  {
    case EVT_LS_ROAD:
      sm_next_state (STOPPING);
      break;
    case EVT_LIFT_MAN_DOWN:
      /* Button released */
      if (value == HIGH)
      {
        sm_next_state (STOPPING);
      }
      break;
  }
}

static void sm_stopping (int event, long value)
{
  if (event == EVT_LIFT_STOPPED)
  {
    sm_next_state (IDLE);
  }
}
