void checkPortal() {
  // клики
  if (portal.click()) {
    if (portal.click("bright")) {
      data.brightness = map(portal.getInt("bright"), 0, 100, 10, 255);
      target_brightness = data.brightness;
    }
    if (portal.click("switch")) {
      data.power = portal.getCheck("switch");
      if (!data.power) powerOffFlag = 1;
      sendPacket();
    }
    if (portal.click("mode")) {
      next_mode = portal.getInt("mode") + 1;
      if (data.mqtt_on) sendPacket();
      switch (next_mode) {
        case 1:
          data.FPS = data.FPS_1;
          break;
        case 2:
          data.FPS = data.FPS_2;
          break;
        case 3:
          data.FPS = data.FPS_3;
          break;
        case 4:
          data.FPS = data.FPS_4;
          break;
        case 5:
          data.FPS = data.FPS_5;
          break;
        case 6:
          data.FPS = data.FPS_6;
          break;
      }
      frameTimer.setPeriod(1000000 / data.FPS);
    }
    if (portal.click("color")) {
      data.color = portal.getInt("color");
      sendPacket();
    }
    if (portal.click("blend_speed")) data.blend_speed = portal.getInt("blend_speed");
    if (portal.click("bug_max_speed")) data.bug_max_speed = portal.getInt("bug_max_speed");
    if (portal.click("bugs_amount")) data.bugs_amount = portal.getInt("bugs_amount");
    if (portal.click("hue_speed")) data.hue_speed = portal.getInt("hue_speed");
    if (portal.click("rainbow_speed")) data.rainbow_speed = portal.getInt("rainbow_speed");
    if (portal.click("sparkles_cooling_speed")) data.sparkles_cooling_speed = portal.getInt("sparkles_cooling_speed");
    if (portal.click("fire_cooling_speed")) data.fire_cooling_speed = portal.getInt("fire_cooling_speed");
    if (portal.click("fire_sparking")) data.fire_sparking = portal.getInt("fire_sparking");
    if (portal.click("bright_delay")) {
      data.brightness_delay = portal.getInt("bright_delay");
      delayTimer.setPeriod(data.brightness_delay);
    }
    if (portal.click("bright_step")) data.brightness_step = portal.getInt("bright_step");
//    if (portal.click("FPS_1")) {
//      data.FPS = portal.getInt("FPS_1");
//      frameTimer.setPeriod(1000000 / data.FPS);
//    }
    if (portal.click("FPS_2")) {
      data.FPS = portal.getInt("FPS_2");
      frameTimer.setPeriod(1000000 / data.FPS);
    }
    if (portal.click("FPS_3")) {
      data.FPS = portal.getInt("FPS_3");
      frameTimer.setPeriod(1000000 / data.FPS);
    }
    if (portal.click("FPS_4")) {
      data.FPS = portal.getInt("FPS_4");
      frameTimer.setPeriod(1000000 / data.FPS);
    }
    if (portal.click("FPS_5")) {
      data.FPS = portal.getInt("FPS_5");
      frameTimer.setPeriod(1000000 / data.FPS);
    }
    if (portal.click("FPS_6")) {
      data.FPS = portal.getInt("FPS_6");
      frameTimer.setPeriod(1000000 / data.FPS);
    }
    if (portal.click("wink"))  {
      winkFlag = 1;
      sendPacket();
    }
    if (portal.click("AP_mode")) data.AP_mode = portal.getInt("AP_mode");
    if (portal.click("mqtt_on")) data.mqtt_on = portal.getInt("mqtt_on");
    if (portal.click("dark_mode")) data.dark_mode = portal.getInt("dark_mode");
    if (portal.click("Reset")) {
      data.dark_mode = 1;
      data.mqtt_on = 1;
      data.AP_mode = 0;
      data.power = 1;
      data.brightness = 100;
      data.cur_mode = 1;
      next_mode = 1;
      data.FPS_1 = 60;
      data.color = 200;
      data.blend_speed = 17;
      data.FPS_2 = 60;
      data.bug_max_speed = 30;
      data.bugs_amount = 3;
      data.FPS_3 = 60;
      data.hue_speed = 1;
      data.FPS_4 = 60;
      data.rainbow_speed = 1;
      data.FPS_5 = 60;
      data.sparkles_cooling_speed = 50;
      data.FPS_6 = 60;
      data.fire_cooling_speed = 80;
      data.fire_sparking = 100;
      data.FPS = 60;
      data.brightness_delay = 5;
      data.brightness_step = 5;
      memory.updateNow();
    }
    if (portal.click("reload")) {
      memory.updateNow();
      ESP.reset();
    }
    memory.updateNow();
  }

  // обновления
  if (portal.update()) {
    if (!data.AP_mode && data.mqtt_on) {
      if (portal.update("ledL")) portal.answer(mqtt.connected());
      if (portal.update("ledR")) portal.answer(!onlineTmr.elapsed());
      if (portal.update("ledP")) portal.answer((!pirTmr.elapsed() && !onlineTmr.elapsed()));
    }
    if (portal.update("switch")) portal.answer(data.power);
    if (portal.update("mode")) portal.answer(data.cur_mode - 1);
    if (portal.update("color")) portal.answer(data.color);
  }

  // формы
  if (portal.form()) {
    if (portal.form("/save")) {
      portal.copyStr("ssid", data.ssid);
      portal.copyStr("pass", data.pass);
      portal.copyStr("local", data.local);
      portal.copyStr("remote", data.remote);
      portal.copyStr("host", data.host);
      data.port = portal.getInt("port");

      memory.updateNow();
      if ( data.mqtt_on) {
        mqtt.disconnect();
        mqtt.setServer(data.host, data.port);
        connectMQTT();
      }
    }
  }
}
