void startup() {
  delay(1000);
  portal.attachBuild(build);  // подключаем интерфейс
  portal.attach(checkPortal);
  EEPROM.begin(sizeof(data) + 1); // +1 на ключ
  memory.begin(0, 'a');           // запускаем менеджер памяти

  IPAddress ip = IPAddress(data.ip[0], data.ip[1], data.ip[2], data.ip[3]);

  // запускаем ленту
  FastLED.addLeds<WS2812B, LED1_PIN, GRB>(leds1, LED_AMOUNT).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(50);
  FastLED.show();
  gPal = HeatColors_p;

  // настраиваем уровень кнопки
  btn.setBtnLevel(BTN_LEVEL);

  // таймер на 2 секунды перед подключением,
  // чтобы юзер успел кликнуть если надо
  Timer tmr(4000);
  while (!tmr.period()) {
    loadAnimation(CRGB::Green);    // анимация подключения
    btn.tick();
    if (btn.click()) data.AP_mode = !data.AP_mode; // клик - переключаем режим работы
  }

  // если работаем не в автономном режиме
  if (!data.AP_mode) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(data.ssid, data.pass);
    tmr.setPeriod(15000);
    tmr.restart();

    while (WiFi.status() != WL_CONNECTED) {
      loadAnimation(CRGB::Blue);      // анимация подключения
      btn.tick();
      // если клик по кнопке или вышел таймаут
      if (btn.click() || tmr.period()) {
        WiFi.disconnect();  // отключаемся
        data.AP_mode = 1;
        memory.tick();
        memory.updateNow();
      }
    }
  }
  if (!data.AP_mode) {
    FastLED.clear();
    FastLED.show();
    // переписываем удачный IP себе в память
    if (ip != WiFi.localIP()) {
      ip = WiFi.localIP();
      for (int i = 0; i < 4; i++) data.ip[i] = ip[i];
      memory.update();
    }
    // стартуем вебсокет
    if (data.mqtt_on) {
      mqtt.setServer(data.host, data.port);
      mqtt.setCallback(callback);
      randomSeed(micros());
    }
  }
  else if (data.AP_mode) {
    WiFi.mode(WIFI_AP);
    String s(F("Dashkkk"));
    s += ip.toString();
    WiFi.softAP(s);
  }
  portal.start();   // стартуем портал
  portal.enableOTA();
  portal.downloadAuto(true);
  frameTimer.setPeriod(1000000/data.FPS);
  FastLED.setBrightness(((long)data.brightness * data.brightness * data.brightness + 130305) >> 16);
}
