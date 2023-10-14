#define LED1_PIN D7     // пин ленты 1
#define BTN_PIN D5      // пин кнопки
#define PIR_PIN D0      // пин PIR (ИК датчика)
#define LED_AMOUNT 72    // кол-вл светодиодов
#define LED_NUM 36
#define BTN_LEVEL 1     // 1 - кнопка подключает VCC, 0 - подключает GND
#define USE_PIR 1       // 1 - использовать PIR (ИК датчик) на этой лампе
#define IGNORE_PIR 0    // 1 - игнорировать сигнал PIR (ИК датчика) с удалённой лампы
#define MAX_BUGS_AMOUNT 20
#define SECOND_IN_MILLIS 1000000
#define EB_STEP 100   // период step шага кнопки
/*
  Запуск:
  Клик или >15 секунд при анимации подклчения: сменить режим работы автономный <-> Wi-Fi
  Кнопка сохранить или клик: перезагрузить систему

  Анимация:
  - Мигает зелёным: подключение к роутеру
  - Мигает синим: запущена точка доступа WLamp <IP>

  Работа:
  1 клик: вкл/выкл
  2 клика: сменить режим
  3 клика: сменить цвет
  Удержание: сменить яркость

  Вика

  Лента D6 и D7 по 8 светодиодов начало вверху
  Кнопка D5
  Датчика движения нет


  Даша (НГ)

  Лента по 13 светодиодов начало вверху D6, D7, D8
  Кнопка D5
  E - +5V
  F - GND
*/

// ============= ВСЯКОЕ =============
#define MQTT_HEADER "GWL:"  // заголовок пакета данных
#define MODES_AMOUNT 6 // количество режимов работы
// ============= БИБЛЫ =============
#include <GyverPortal.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEManager.h>
#include <FastLED.h>

#include <EncButton.h>
#include "Timer.h"

struct LampData {
  char ssid[32] = "KEKW";                           //SSID точки доступа
  char pass[32] = "melharucos";                           //пароль точки доступа
  char local[20] = "Dashkk";                          //имя лампы
  char remote[20] = "HOTPEPE";                         //имя удаленной лампы
  char host[52] = "broker.mqttdashboard.com";   //MQTT сервер
  uint16_t port = 1883;                         //порт MQTT сервера
  uint8_t ip[4] = {0, 0, 0, 0};                 //IP лампы

  bool dark_mode = 1;         // 1 - dark           0 - light
  bool mqtt_on = 1;           // 1 - mqtt включен   0 - mqtt выключен
  bool AP_mode = 0;           // 1 - автономный     0 - с Wi-Fi
  bool power = 1;
  int16_t brightness = 100;       //яркость
  uint8_t cur_mode = 1;       //текущий режим

  //параметры режима 1
  uint32_t FPS_1 = 60;
  uint8_t color = 0;          //цвет
  uint8_t blend_speed = 17;   //скорость смешивания

  //параметры режима 2
  uint32_t FPS_2 = 60;
  uint8_t bug_max_speed = 30; //максимальная скорость
  uint8_t bugs_amount = 3;    //количество "жучар"

  //параметры режима 3
  uint32_t FPS_3 = 60;
  uint8_t hue_speed = 1;

  //параметры режима 4
  uint32_t FPS_4 = 60;
  uint8_t rainbow_speed = 1;

  //параметры режима 5
  uint32_t FPS_5 = 60;
  uint8_t sparkles_cooling_speed = 50;

  //параметры режима 6
  uint32_t FPS_6 = 60;
  uint8_t fire_cooling_speed = 80;
  uint8_t fire_sparking = 100;

  uint32_t FPS = 60;   //время кадра (мс)
  uint8_t brightness_delay = 5;   //время шага изменения яркости при смене режима
  uint8_t brightness_step = 5;    //шаг изменения яркости при смене режима
};

LampData data;                    //объявление структуры данных лампы
Button btn(BTN_PIN);              //подключаем кнопку
CRGB leds1[LED_AMOUNT];           //подключаем ленту 1
WiFiClient espClient;
PubSubClient mqtt(espClient);
GyverPortal portal;
EEManager memory(data);
CRGBPalette16 gPal;

bool fill_flag = 0;
bool pirFlag = 0;                           //Флаг датчика движения
bool winkFlag = 0;                          //Флаг подмигивания
bool startFlag = 0;                         //Флаг включения лампы
bool loadingFlag = 1;                       //Флаг загрузки режима
bool powerOffFlag = 0;                      //Флаг выключения лампы (кнопкой или удаленно)
uint8_t next_mode = data.cur_mode;          //следующий режим
uint8_t wink_counter = 1;
uint8_t min_brightness = 10;
uint8_t max_brightness = 255;
uint32_t max_FPS = 1000;
uint32_t min_FPS = 1;
uint16_t FPS_step = 1;
int16_t target_brightness = data.brightness;
int16_t local_brightness = 0; //Текущая яркость для режима
const uint8_t hLen = strlen(MQTT_HEADER);   //Длина заголовка MQTT сообщения

Timer onlineTmr(18000, false, 0);  // 18000 миллисекунд таймаут онлайна
Timer pirTmr(2000, false, 0);      // 2000 миллисекундунды таймаут пира
Timer hbTmr(10000, false, 0);              // 10000 миллисекунд период отправки пакета
Timer delayTimer(data.brightness_delay, true, 0);   // Таймер для изменения яркости при переключении режима или выключении
Timer frameTimer(1000000/data.FPS, 1, 1);
void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(D4, OUTPUT);
  startup();      // запускаем всё
  if (!data.power) {
    target_brightness = 0;
  }
}

void loop() {
  if (USE_PIR && digitalRead(PIR_PIN)) {
    pirFlag = 1;  // опрос ИК датчика
  }
  //digitalWrite(D4, !digitalRead(PIR_PIN));
  if (data.mqtt_on)  {
    heartbeat();    // отправляем пакет что мы онлайн
    mqttTick();     // проверяем входящие
  }
  memory.tick();  // проверяем обновление настроек
  buttonTick();   // действия кнопки
  if (frameTimer.period() && data.power) { //настало время кадра
    animation();    // рисуем кадр
  }
  if (winkFlag && data.power) wink();
  change_state();
  if (delayTimer.period()) {
    if (local_brightness != target_brightness) {
      int8_t bright_steps = (local_brightness - target_brightness) / data.brightness_step;
      if (bright_steps > 0) local_brightness -= data.brightness_step;
      else if (bright_steps < 0) local_brightness += data.brightness_step;
      else if (bright_steps == 0) local_brightness = target_brightness;
      if (local_brightness < 10) FastLED.setBrightness(0);
      else FastLED.setBrightness(((long)local_brightness * local_brightness * local_brightness + 130305) >> 16);
    }
  }
  FastLED.show();
  portal.tick();  // пинаем портал
}
