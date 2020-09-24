
#define LOC_ADDR 0x0
#define CMD_ADDR 0x01
static byte lift_location = I_AM_LOST;
static byte location_target = I_AM_LOST;

void location_cmd_set (byte cmd)
{
  if (test_mode_get () == false)
  {
    EEPROM.write(CMD_ADDR, cmd);
  }
  Serial.print (F("Writing command "));
  Serial.print (event_desc (cmd));
  Serial.println (F(" to EEPROM"));
}

void location_set (byte event)
{
  lift_location = event;
  if (test_mode_get () == false)
  {
    EEPROM.write(LOC_ADDR, event);
  }
  Serial.print (F("Writing new location "));
  Serial.print (location_desc (event));
  Serial.println (F(" to EEPROM"));
}

int location_load (void)
{
  byte last_cmd;

  /* Get location */
  lift_location = EEPROM.read(LOC_ADDR);
  if (lift_location >= LOCATION_MAX)
  {
    Serial.println (F("Could not load location - EEPROM Failure\n"));
    return FAILURE;
  }
  
  last_cmd = EEPROM.read(CMD_ADDR);

  if (last_cmd > EVT_MAX)
  {
    Serial.println (F("Could not load last command - EEPROM Failure\n"));
    return FAILURE;
  }

  /* Are command and location consistent */
  switch (last_cmd)
  {
    case EVT_CALL_ROAD:
    case EVT_CALL_HOUSE_LONG:
      if (lift_location != ROAD)
      {
        lift_location = I_AM_LOST;
      }
      break;
    case EVT_CALL_BASEMENT:
      if (lift_location != BASEMENT)
      {
        lift_location = I_AM_LOST;
      }
      break;
    case EVT_CALL_HOUSE:
    case EVT_CALL_ROAD_LONG:
      if (lift_location != HOUSE)
      {
        lift_location = I_AM_LOST;
      }    
      break;
    default:
      Serial.print (F("No such command "));
      Serial.println (last_cmd);
      return FAILURE;
  }

  Serial.print (F("We are at the "));
  Serial.println (location_desc (lift_location));

 return SUCCESS;
}

int location_get ()
{
  if (lift_location == I_AM_LOST)
  {
    Serial.println (F("I am lost.\n"));
  }
  return lift_location;
}

void location_target_set (byte location)
{
  location_target = location;
}

byte location_directory (void)
{
  switch (location_target)
  {
    case ROAD:
      switch (lift_location)
      {
        case ROAD:
          return STOPPING;
        case BASEMENT:
        case HOUSE:
          return DOWN; 
      }
    case BASEMENT:
      switch (lift_location)
      {
        case ROAD:
          return UP;
        case BASEMENT:
          return STOPPING;
        case HOUSE:
          return DOWN; 
      }
    case HOUSE:
      switch (lift_location)
      {
        case ROAD:
          return UP;
        case BASEMENT:
          return UP;
        case HOUSE:
          return STOPPING; 
      }
    default:
        Serial.print (F("No such command "));
        Serial.println (location_target);
        return IDLE;
  }
}
