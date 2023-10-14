void build() {
  GP.BUILD_BEGIN();
  GP.PAGE_TITLE("Дашккк");
  if (data.dark_mode) GP.THEME(GP_DARK);
  else GP.THEME(GP_LIGHT);
  GP.ONLINE_CHECK(10000);
  if (!data.AP_mode && data.mqtt_on) GP.UPDATE("ledL,ledR,ledP,switch,mode,color", 2000);
  else GP.UPDATE("switch,bright,color,blend_speed,bug_max_speed,bugs_amount,hue_speed,rainbow_speed,sparkles_cooling_speed,fire_cooling_speed,fire_sparking,bright_step,bright_delay", 2000);
  GP.RELOAD_CLICK("dark_mode");
  GP.NAV_TABS("Главная,Настройки");

  GP.NAV_BLOCK_BEGIN();
  if (!data.AP_mode && data.mqtt_on) {
    GP.LABEL("Статус");
    GP.BLOCK_BEGIN();
    GP.LABEL("Локальная:");
    GP.LED_GREEN("ledL", mqtt.connected());
    GP.BREAK();
    GP.LABEL("Удаленная:");
    GP.LED_GREEN("ledR", !onlineTmr.elapsed());
    GP.BREAK();
    GP.LABEL("Датчик движения:");
    GP.LED_RED("ledP", (!pirTmr.elapsed() && !onlineTmr.elapsed()));
    GP.BLOCK_END();
  }

  GP.BLOCK_BEGIN();
  GP.LABEL("Питание");
  GP.SWITCH("switch", data.power);
  GP.BREAK();
  GP_MAKE_BOX(
    GP.LABEL("Яркость");
    GP.SLIDER("bright", map(data.brightness, 0, 255, 0, 100), 0, 100);
  );
  GP.BOX_BEGIN(GP_CENTER);
  //GP.LABEL("Режим");
  GP.SELECT("mode", "Сплошной цвет,Жучки,Сменяющийся цвет,Радуга,Вспышки,Огонь", next_mode - 1, false, false, true);
  GP.BLOCK_END();
  switch (next_mode) {
    case 1: {
        GP_MAKE_BOX(
          GP.LABEL("Цвет");
          GP.SLIDER("color", data.color, 0, 255);
        );
//        GP_MAKE_BOX(
//          GP.LABEL("Скорость");
//          GP.SPINNER("FPS_1", data.FPS_1, min_FPS, max_FPS, FPS_step);
//        );
        break;
      }
    case 2: {
        GP_MAKE_BOX(
          GP.LABEL("Жуков");
          GP.SPINNER("bugs_amount", data.bugs_amount, 1, MAX_BUGS_AMOUNT);
        );
        GP_MAKE_BOX(
          GP.LABEL("Скорость");
          GP.SPINNER("FPS_2", data.FPS_2, min_FPS, max_FPS, FPS_step);
        );
        break;
      }
    case 3: {
        GP_MAKE_BOX(
          GP.LABEL("Скорость");
          GP.SPINNER("FPS_3", data.FPS_3, min_FPS, max_FPS, FPS_step);
        );
        break;
      }
    case 4: {
        GP_MAKE_BOX(
          GP.LABEL("Скорость");
          GP.SPINNER("FPS_4", data.FPS_4, min_FPS, max_FPS, FPS_step);
        );
        break;
      }
    case 5: {
        GP_MAKE_BOX(
          GP.LABEL("Затухание");
          GP.SPINNER("sparkles_cooling_speed", data.sparkles_cooling_speed, 1, 255);
        );
        GP_MAKE_BOX(
          GP.LABEL("Скорость");
          GP.SPINNER("FPS_5", data.FPS_5, min_FPS, max_FPS, FPS_step);
        );
        break;
      }
    case 6: {
        GP_MAKE_BOX(
          GP.LABEL("Охлаждения");
          GP.SPINNER("fire_cooling_speed", data.fire_cooling_speed, 1, 255);
        );
        GP_MAKE_BOX(
          GP.LABEL("Искры");
          GP.SPINNER("fire_sparking", data.fire_sparking, 1, 255);
        );
        GP_MAKE_BOX(
          GP.LABEL("Скорость");
          GP.SPINNER("FPS_6", data.FPS_6, min_FPS, max_FPS, FPS_step);
        );
        break;
      }
  }
  if (data.mqtt_on) {
    GP.BUTTON("wink", "Подмигнуть");
  }
  GP.BLOCK_END();
  GP.NAV_BLOCK_END();

  GP.NAV_BLOCK_BEGIN();
  GP.BLOCK_BEGIN();
  GP_MAKE_BOX(
    GP.LABEL("Темная тема");
    GP.SWITCH("dark_mode", data.dark_mode);
  );
  GP_MAKE_BOX(
    GP.LABEL("Точка доступа");
    GP.SWITCH("AP_mode", data.AP_mode);
  );
  GP_MAKE_BOX(
    GP.LABEL("MQTT");
    GP.SWITCH("mqtt_on", data.mqtt_on);
  );

  GP_MAKE_BOX(
    GP.LABEL("Bright delay");
    GP.SLIDER("bright_delay", data.brightness_delay, 0, 10);
  );
  GP_MAKE_BOX(
    GP.LABEL("Bright step");
    GP.SLIDER("bright_step", data.brightness_step, 1, 63);
  );
  GP_MAKE_BOX(
    GP.LABEL("Colors step");
    GP.SLIDER("hue_speed", data.hue_speed, 1, 255);
  );
  GP_MAKE_BOX(
    GP.LABEL("Blend speed");
    GP.SLIDER("blend_speed", data.blend_speed, 1, 255);
  );
  GP_MAKE_BOX(
    GP.LABEL("Max speed");
    GP.SLIDER("bug_max_speed", data.bug_max_speed, 1, 255);
  );
  GP_MAKE_BOX(
    GP.LABEL("Rainbow speed");
    GP.SPINNER("rainbow_speed", data.rainbow_speed, 1, 255);
  );
  GP.BLOCK_END();
  GP.FORM_BEGIN("/save");

  GP.LABEL("WIFI");
  GP.BLOCK_BEGIN();
  GP.TEXT("ssid", "SSID", data.ssid);
  GP.BREAK();
  GP.PASS("pass", "Пароль", data.pass);
  GP.BLOCK_END();
  if (data.mqtt_on) {
    GP.LABEL("MQTT");
    GP.BLOCK_BEGIN();
    GP.TEXT("local", "Имя лампы", data.local);
    GP.BREAK();
    GP.TEXT("remote", "Имя парной лампы", data.remote);
    GP.BREAK();
    GP.TEXT("host", "Host", data.host);
    GP.BREAK();
    GP.NUMBER("port", "Port", data.port);
    GP.BLOCK_END();
  }
  GP.SUBMIT("Сохранить");
  GP.BUTTON("reload", "Перезагрузка");
  GP.BUTTON("Reset", "Сброс настроек");
  GP.BUTTON_LINK("/ota_update", "Обновление ПО");
  GP.FORM_END();
  GP.NAV_BLOCK_END();
  GP.BUILD_END();
}
