// простенький таймер на миллис
#pragma once
struct Timer {
  // указать период, опционально статус (запущен/не запущен)
  Timer (uint32_t nprd, bool nstate = true, bool nuses_micros = 0) {
    state = nstate;
    uses_micros = nuses_micros;
    setPeriod(nprd);
    if (uses_micros)tmr = micros();
    else tmr = millis();
  }
  // смена период
  void setPeriod(uint32_t nprd) {
    prd = nprd;
  }
  // перезапустить
  void restart() {
    if (uses_micros)tmr = micros();
    else tmr = millis();
    state = true;
  }
  // время с рестарта вышло
  bool elapsed() {
    return (!state || check());
  }
  // периодический таймер
  bool period() {
    if (state && check()) {
      restart();
      return 1;
    } return 0;
  }
  bool check() {
    if (uses_micros) return micros() - tmr >= prd;
    else return millis() - tmr >= prd;
  }
  bool uses_micros = 0;
  uint32_t tmr, prd;
  bool state = 1;
};
