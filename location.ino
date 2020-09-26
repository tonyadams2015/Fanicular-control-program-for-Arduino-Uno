#define LOC_ADDR 0x0
#define CMD_ADDR 0x01
static byte lift_location = LOC_LOST;
static byte location_target = LOC_LOST;

byte loc_convert_event_to_location (byte event)
{
  switch (event)
  {
    case EVT_LS_ROAD:
      return LOC_ROAD; 
    case EVT_LS_BASEMENT:
      return LOC_BASEMENT;
    case EVT_LS_HOUSE:
      return LOC_HOUSE;
    default:
      Serial.print (F("No such location "));
      Serial.println (event_desc (event));
  }
}

void loc_cmd_set (byte cmd)
{
  if (test_mode_get () == false)
  {
    EEPROM.write(CMD_ADDR, cmd);
  }
  Serial.print (F("Writing command "));
  Serial.print (event_desc (cmd));
  Serial.println (F(" to EEPROM"));
}

void loc_set (byte event)
{
  lift_location = loc_convert_event_to_location (event);
  if (test_mode_get () == false)
  {
    EEPROM.write(LOC_ADDR, event);
  }
  Serial.print (F("Writing new location "));
  Serial.print (loc_desc (event));
  Serial.println (F(" to EEPROM"));
}

int loc_load (void)
{
  byte last_cmd;

  /* Get location */
  lift_location = EEPROM.read(LOC_ADDR);
  if (lift_location >= LOC_MAX)
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
      if (lift_location != LOC_ROAD)
      {
        lift_location = LOC_LOST;
      }
      break;
    case EVT_CALL_BASEMENT:
      if (lift_location != LOC_BASEMENT)
      {
        lift_location = LOC_LOST;
      }
      break;
    case EVT_CALL_HOUSE:
    case EVT_CALL_ROAD_LONG:
      if (lift_location != LOC_HOUSE)
      {
        lift_location = LOC_LOST;
      }    
      break;
    default:
      Serial.print (F("No such command "));
      Serial.println (last_cmd);
      return FAILURE;
  }

  if (lift_location == LOC_LOST)
  {
    Serial.print (F("We are "));
  }
  else
  {
    Serial.print (F("We are at the "));
  }
  Serial.println (loc_desc (lift_location));

 return SUCCESS;
}

int loc_get ()
{
  if (lift_location == LOC_LOST)
  {
    Serial.println (F("I am lost.\n"));
  }
  return lift_location;
}

void loc_target_set (byte location)
{
  location_target = location;
}

byte loc_direction_lookup (void)
{
  switch (location_target)
  {
    case LOC_ROAD:
      switch (lift_location)
      {
        case LOC_ROAD:
          return ST_STOPPING;
        case LOC_BASEMENT:
        case LOC_HOUSE:
          return ST_DOWN; 
      }
    case LOC_BASEMENT:
      switch (lift_location)
      {
        case LOC_ROAD:
          return ST_UP;
        case LOC_BASEMENT:
          return ST_STOPPING;
        case LOC_HOUSE:
          return ST_DOWN; 
      }
    case LOC_HOUSE:
      switch (lift_location)
      {
        case LOC_ROAD:
          return ST_UP;
        case LOC_BASEMENT:
          return ST_UP;
        case LOC_HOUSE:
          return ST_STOPPING; 
      }
    default:
        Serial.print (F("No such command "));
        Serial.println (location_target);
        return ST_IDLE;
  }
}

String loc_desc (byte location)
{ 
  switch (location)
  {
    case LOC_LOST:
      return F("Lost");
    case LOC_ROAD:
      return F("Road");
    case LOC_BASEMENT:
      return F("Basement");
    case LOC_HOUSE:
      return F("House");
    default:
      Serial.print (F("(No such location "));
      Serial.println (location);
  }
}
