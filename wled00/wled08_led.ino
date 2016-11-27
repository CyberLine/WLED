void setAllLeds() {
  double d = bri_t;
  double val = d /256;
  int r = col_t[0]*val;
  int g = col_t[1]*val;
  int b = col_t[2]*val;
  for (int i=0; i < led_amount; i++) {
    strip.SetPixelColor(i, RgbColor(r, g, b));
  }
  strip.Show();
}

void setLedsStandard()
{
  col_old[0] = col[0];
  col_old[1] = col[1];
  col_old[2] = col[2];
  bri_old = bri;
  col_t[0] = col[0];
  col_t[1] = col[1];
  col_t[2] = col[2];
  bri_t = bri;
  setAllLeds();
}

void colorUpdated(int callMode)
{
  //call for notifier -> 0: init 1: direct change 2: button 3: notification 4: nightlight
  if (col[0] == col_it[0] && col[1] == col_it[1] && col[2] == col_it[2] && bri == bri_it)
  {
    return; //no change
  }
  col_it[0] = col[0];
  col_it[1] = col[1];
  col_it[2] = col[2];
  bri_it = bri;
  if (bri > 0) bri_last = bri;
  notify(callMode);
  if (fadeTransition || seqTransition)
  {
    if (transitionActive)
    {
      col_old[0] = col_t[0];
      col_old[1] = col_t[1];
      col_old[2] = col_t[2];
      bri_old = bri_t;
    }
    transitionActive = true;
    transitionStartTime = millis();
    transitionDelay = transitionDelay_old;
  } else
  {
    setLedsStandard();
  }
}

void handleTransitions()
{
  if (transitionActive && transitionDelay > 0)
  {
    float tper = (millis() - transitionStartTime)/(float)transitionDelay;
    if (tper >= 1.0)
    {
      transitionActive = false;
      tper_last = 0;
      setLedsStandard();
      if (nightlightActive && nightlightFade)
      {
        initNightlightFade();
      }
      return;
    }
    if (tper - tper_last < transitionResolution)
    {
      return;
    }
    tper_last = tper;
    if (fadeTransition)
    {
      col_t[0] = col_old[0]+((col[0] - col_old[0])*tper);
      col_t[1] = col_old[1]+((col[1] - col_old[1])*tper);
      col_t[2] = col_old[2]+((col[2] - col_old[2])*tper);
      bri_t = bri_old+((bri - bri_old)*tper);
    }
    if (seqTransition)
    {
      
    } else setAllLeds();
  }
}

void initNightlightFade()
{
  float nper = (millis() - nightlightStartTime)/((float)nightlightDelayMs);
  nightlightDelayMs = nightlightDelayMs*(1-nper);
  if (nper >= 1)
  {
    return;
  }
  bri = 0;
  bri_it = 0;
  transitionDelay = (int)(nightlightDelayMins*60000);
  transitionStartTime = nightlightStartTime;
  transitionActive = true;
  nightlightStartTime = millis();
}

void handleNightlight()
{
  if (nightlightActive)
  {
    if (!nightlightActive_old) //init
    {
      notify(4);
      nightlightDelayMs = (int)(nightlightDelayMins*60000);
      nightlightActive_old = true;
      if (nightlightFade)
      {
        initNightlightFade();
      }
    }
    float nper = (millis() - nightlightStartTime)/((float)nightlightDelayMs);
    if (nper >= 1)
    {
      nightlightActive = false;
    }
  } else if (nightlightActive_old) //de-init
  {
    nightlightPassedTime = 0;
    nightlightActive_old = false;
    if (nightlightFade)
    {
      transitionDelay = transitionDelay_old;
      transitionActive = false;
    } else
    {
      bri = 0;
      colorUpdated(4);
    }
  }
}
