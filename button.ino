void buttonTick() {
  btn.tick();

  // клики
  if (btn.hasClicks()) {
    switch (btn.getClicks()) {
      case 1:
        {                                        // вкл выкл
          data.power = !data.power;              //меняем статус лампы на противоположный
          if (!data.power) powerOffFlag = true;  //если выключаем поднять флаг выключения
          memory.update();                       //записать в память
          break;
        }
      case 2:
        {  // сменить режим
          next_mode++;
          if (next_mode > MODES_AMOUNT) next_mode = 1;
          break;
        }
      case 3:
        {  // сменить цвет
          data.color += 32;
          memory.update();
          break;
        }
      case 4:
        {
          winkFlag = 1;
        }
        if (data.mqtt_on) sendPacket();
    }
  }

  // импульсное удержание
  static int8_t dir = 10;
  if (btn.step() && !winkFlag && target_brightness == local_brightness) {
    data.brightness = constrain(data.brightness + dir, min_brightness, max_brightness);
    target_brightness = data.brightness;
    if (data.brightness == 255) {
      winkFlag = 1;
    }
  }
  if (btn.releaseStep()) {
    dir = -dir;
    memory.update();
  }
}
