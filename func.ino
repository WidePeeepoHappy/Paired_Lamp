// подмигнуть надо переписать
void wink() {
  if (wink_counter == 1) {
    delayTimer.setPeriod(1);
    target_brightness = 0;
    wink_counter++;
    sendPacket();
  }
  else if (wink_counter % 2 == 1 && local_brightness == data.brightness) {
    target_brightness = 0;
    wink_counter++;
    sendPacket();
  }
  else if (wink_counter == 6 && (local_brightness == 255 || target_brightness == data.brightness)) {
    target_brightness = data.brightness;
    wink_counter = 1;
    winkFlag = 0;
    delayTimer.setPeriod(data.brightness_delay);
    sendPacket();
  }
  else if (wink_counter != 6 && wink_counter % 2 == 0 && local_brightness == 0) {
    target_brightness = data.brightness;
    wink_counter++;
    sendPacket();
  }
}

// выводим эффект на ленту
void animation() {
  switch (data.cur_mode) {// просто выбираем что отрисовать
    case 1: {
        solid_color(data.color, data.blend_speed);//сплошной цвет
        break;
      }
    case 2: {
        lightBugs(fill_flag, data.bugs_amount, data.bug_max_speed);//жучары
        if (fill_flag) fill_flag = 0;
        break;
      }
    case 3: {
        colors(data.hue_speed);//хуевил
        break;
      }
    case 4: {
        rainbow(data.rainbow_speed);//радуга
        break;
      }
    case 5: {
        sparkles(data.sparkles_cooling_speed);//конфетти
        break;
      }
    case 6: {
        fire(data.fire_cooling_speed, data.fire_sparking);
        break;
      }
  }
}

// анимация работы локал портала
void loadAnimation(CRGB color) {
  static int8_t dir = 1;
  static uint8_t val = 0;
  static Timer tmr(20);
  if (tmr.period()) {
    val += dir;
    if (val >= 100 || val <= 0) dir = -dir;
    fill_solid(leds1, LED_AMOUNT, color);
    FastLED.setBrightness(val);
    FastLED.show();
  }
  yield();
}

// цыганский парсер инта из указанного индекса в строке
int getFromIndex(char* str, int idx, char div = ',') {
  int val = 0;
  uint16_t i = 0;
  int count = 0;
  bool sign = 0;
  while (str[i]) {
    if (idx == count) {
      if (str[i] == div) break;
      if (str[i] == '-') sign = -1;
      else {
        val *= 10L;
        val += str[i] - '0';
      }
    } else if (str[i] == div) count++;
    i++;
  }
  return sign ? -val : val;
}

void change_state() {
  if ((next_mode != data.cur_mode) || powerOffFlag == 1) {
    target_brightness = 0;
    if (local_brightness == 0) powerOffFlag = 0;
    if (local_brightness == 0 && next_mode == 2) fill_flag = 1;
    else fill_flag = 0;
  }
  if (((next_mode != data.cur_mode) || (powerOffFlag == 0 && data.power == 1)) && local_brightness == 0) {
    target_brightness = data.brightness;
    if (local_brightness == 0) {
      data.cur_mode = next_mode;
    }
  }
}
