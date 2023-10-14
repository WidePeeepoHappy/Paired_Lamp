//вообще не ебу что тут происходит
uint8_t TRACK_STEP = 50;
// ********************** Сплошной цвет ********************************
void solid_color(uint8_t color, uint8_t blend_speed) {
  // здесь делаем плавные переходы между цветами
  CRGB ncol = CHSV(color, 255, 255);
  CRGB thisColor1 = leds1[0];
  if (thisColor1 != ncol) thisColor1 = blend(thisColor1, ncol, blend_speed);
  for (int i = 0; i < LED_AMOUNT; i++) {
    leds1[i] = thisColor1;
  }
}

// ****************************** Жучары ******************************
int16_t speed[MAX_BUGS_AMOUNT];
int16_t pos[MAX_BUGS_AMOUNT];
CRGB bugColors[MAX_BUGS_AMOUNT];

void lightBugs(bool loadingFlag, uint8_t bugs_amount, uint8_t max_speed) {
  if (loadingFlag) {
    loadingFlag = false;
    for (int i = 0; i < MAX_BUGS_AMOUNT; i++) {
      bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      pos[i] = random(0, LED_AMOUNT);
      speed[i] += random(-5, 6);
    }
  }
  FastLED.clear();
  for (int i = 0; i < bugs_amount; i++) {
    speed[i] += random(-5, 6);
    if (speed[i] == 0) speed[i] += (-5, 6);

    if (abs(speed[i]) > max_speed) speed[i] = 0;
    pos[i] += speed[i] / 10;
    if (pos[i] < 0) {
      pos[i] = 0;
      speed[i] = -speed[i];
    }
    if (pos[i] > LED_AMOUNT - 1) {
      pos[i] = LED_AMOUNT - 1;
      speed[i] = -speed[i];
    }
    leds1[pos[i]] = bugColors[i];
  }
}

// ****************************** ЦВЕТА ******************************
byte hue;
void colors(uint8_t hue_step) {
  hue += hue_step;
  CRGB thisColor = CHSV(hue, 255, 255);
  for (int i = 0; i < LED_AMOUNT; i++) {
    leds1[i] = thisColor;
  }
}
// ****************************** РАДУГА ******************************
void rainbow(uint8_t hue_speed) {
  hue += hue_speed;
  for (int i = 0; i < LED_AMOUNT; i++) {
    leds1[i] = CHSV((byte)(hue + i * float(255 / LED_AMOUNT)), 255, 255);
  }
}
// ****************************** КОНФЕТТИ ******************************
void sparkles(uint8_t cooling_speed) {
  byte thisNum = random(0, LED_AMOUNT);
  if (getPixColor(thisNum, 1) == 0)
    leds1[thisNum] = CHSV(random(0, 255), 255, 255);
  for (int i = 0; i < LED_AMOUNT; i++) {
    if ((uint32_t)getPixColor(i, 1) == 0) continue;
    leds1[i].fadeToBlackBy(cooling_speed);
  }
}
// ****************************** ОГОНЬ ******************************
void fire(uint8_t fire_cooling_speed, uint8_t sparking) {
  random16_add_entropy( random16());
  // Array of temperature readings at each simulation cell
  static byte heat_1[LED_NUM];
  static byte heat_2[LED_NUM];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < LED_NUM; i++) {
    heat_1[i] = qsub8( heat_1[i],  random8(0, ((fire_cooling_speed * 10) / LED_NUM) + 2));
    yield();
    heat_2[i] = qsub8( heat_2[i],  random8(0, ((fire_cooling_speed * 10) / LED_NUM) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = LED_NUM - 1; k >= 2; k--) {
    heat_1[k] = (heat_1[k - 1] + heat_1[k - 2] + heat_1[k - 2] ) / 3;
    heat_2[k] = (heat_2[k - 1] + heat_2[k - 2] + heat_2[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < sparking ) {
    int y_1 = random8(7);
    heat_1[y_1] = qadd8(heat_1[y_1], random8(160, 255));
    int y_2 = random8(7);
    heat_2[y_2] = qadd8(heat_2[y_2], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < LED_NUM; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex_1 = scale8( heat_1[j], 240);
    byte colorindex_2 = scale8( heat_2[j], 240);
    CRGB color_1 = ColorFromPalette(gPal, colorindex_1);
    CRGB color_2 = ColorFromPalette(gPal, colorindex_2);
    leds1[(LED_AMOUNT - 1) - j] = color_2;
    leds1[j] = color_1;
  }
}

// ****************** СЛУЖЕБНЫЕ ФУНКЦИИ *******************
uint32_t getPixColor(int thisPixel, uint8_t strip_num) {
  if (strip_num == 1)return (((uint32_t)leds1[thisPixel].r << 16) | ((long)leds1[thisPixel].g << 8 ) | (long)leds1[thisPixel].b);
}
