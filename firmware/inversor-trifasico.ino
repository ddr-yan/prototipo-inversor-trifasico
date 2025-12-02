#include <Arduino.h>
#include "driver/mcpwm.h"

// =================== CONFIGURACIÓN ===================
static const int PIN_AH = 33; // MCPWM0A
static const int PIN_AL = 13; // GPIO
static const int PIN_BH = 27; // MCPWM1A
static const int PIN_BL = 14; // GPIO
static const int PIN_CH = 25; // MCPWM2A
static const int PIN_CL = 26; // GPIO

static const int PIN_POT = 32; // ADC

// PWM / tiempos
static const uint32_t PWM_FREQ_HZ  = 25000;   // 10–16 kHz (IGBT) / 20–30 kHz (MOSFET HV)
static const uint16_t DEADTIME_US  = 2;       // 1–3 µs
static const uint32_t ALIGN_MS     = 300;     // alineación inicial
static const uint32_t RAMP_MS      = 600;     // rampa abierta
static const float    F_START_HZ   = 5.0f;    // inicio
static const float    F_RAMP_HZ    = 20.0f;   // final rampa

// Límites de duty en %
static const float DUTY_MIN = 6.0f;   // mantener bootstrap
static const float DUTY_MAX = 94.0f;  // dejar margen superior


// =================== PROTOTIPOS ===================
static inline void setDutyAH(float duty_percent);
static inline void setDutyBH(float duty_percent);
static inline void setDutyCH(float duty_percent);
static inline void offAH();
static inline void offBH();
static inline void offCH();
static inline void allOff();
static inline void applyStep(uint8_t s, float duty);
static inline void commutate_to(uint8_t nextStep, float duty);
static inline void stepNext(float duty);
static void mcpwm_init_three_channels();
static void alignAndRampOpenLoop();

// =================== ESTADO ===================
enum { S0=0, S1, S2, S3, S4, S5 };
volatile uint8_t curStep = S0;
volatile float   duty_cmd = 10.0f;

// =================== UTILIDADES GPIO ===================
inline void setHigh(int pin){ digitalWrite(pin, HIGH); }
inline void setLow (int pin){ digitalWrite(pin, LOW ); }

// =================== MCPWM (AH/BH/CH) ===================
static void mcpwm_init_three_channels() {
  // AH en MCPWM0A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PIN_AH);
  // BH en MCPWM1A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, PIN_BH);
  // CH en MCPWM2A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM2A, PIN_CH);

  mcpwm_config_t cfg;
  cfg.frequency = PWM_FREQ_HZ;
  cfg.cmpr_a = 0.0f;
  cfg.cmpr_b = 0.0f;
  cfg.counter_mode = MCPWM_UP_COUNTER;
  cfg.duty_mode = MCPWM_DUTY_MODE_0;

  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &cfg); // AH
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &cfg); // BH
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_2, &cfg); // CH
}

inline void setDutyAH(float d){
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, d);
  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}
inline void setDutyBH(float d){
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, d);
  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}
inline void setDutyCH(float d){
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A, d);
  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}
inline void offAH(){ setDutyAH(0.0f); }
inline void offBH(){ setDutyBH(0.0f); }
inline void offCH(){ setDutyCH(0.0f); }

// =================== SEIS PASOS ===================
inline void allOff() {
  offAH(); offBH(); offCH();
  setLow(PIN_AL); setLow(PIN_BL); setLow(PIN_CL);
}

// En cada estado: XH = PWM; YL = ON; Z = flotante
static inline void applyStep(uint8_t s, float duty) {
  switch (s % 6) {
    case S0: // A+ PWM, B- ON, C flotante
      offBH(); offCH();
      setLow(PIN_AL); setLow(PIN_CL);   // A low OFF, C low OFF (float)
      setHigh(PIN_BL);                  // B low ON
      setDutyAH(duty);                  // A high PWM
      break;

    case S1: // A+ PWM, C- ON, B flotante
      offBH(); offCH();
      setLow(PIN_AL); setLow(PIN_BL);   // A low OFF, B low OFF (float)
      setHigh(PIN_CL);                  // C low ON
      setDutyAH(duty);                  // A high PWM
      break;

    case S2: // B+ PWM, C- ON, A flotante
      offAH(); offCH();
      setLow(PIN_AL); setLow(PIN_BL);   // A low OFF (float), B low OFF
      setHigh(PIN_CL);                  // C low ON
      setDutyBH(duty);                  // B high PWM
      break;

    case S3: // B+ PWM, A- ON, C flotante
      offAH(); offCH();
      setLow(PIN_BL); setLow(PIN_CL);   // B low OFF, C low OFF (float)
      setHigh(PIN_AL);                  // A low ON
      setDutyBH(duty);                  // B high PWM
      break;

    case S4: // C+ PWM, A- ON, B flotante
      offAH(); offBH();
      setLow(PIN_BL); setLow(PIN_CL);   // B low OFF (float), C low OFF
      setHigh(PIN_AL);                  // A low ON
      setDutyCH(duty);                  // C high PWM
      break;

    case S5: // C+ PWM, B- ON, A flotante
      offAH(); offBH();
      setLow(PIN_AL); setLow(PIN_CL);   // A low OFF (float), C low OFF
      setHigh(PIN_BL);                  // B low ON
      setDutyCH(duty);                  // C high PWM
      break;
  }
}


static inline void commutate_to(uint8_t nextStep, float duty) {
  allOff();
  delayMicroseconds(DEADTIME_US);   // DEAD-TIME duro entre ramas
  curStep = nextStep % 6;
  applyStep(curStep, duty);
}

static inline void stepNext(float duty){
  commutate_to(curStep + 1, duty);
}



void setup() {
  // Low-sides
  pinMode(PIN_AL, OUTPUT);
  pinMode(PIN_BL, OUTPUT);
  pinMode(PIN_CL, OUTPUT);
  allOff();

  // High-sides MCPWM
  mcpwm_init_three_channels();

  Serial.begin(115200);
  Serial.println("BLDC MCPWM listo (six-step, dead-time SW).");

  while (!Serial) { delay(10); }
}
void loop() {
  // Duty fijo de prueba
  duty_cmd = 20.0f;

  // Frecuencia eléctrica fija (por ejemplo 12 Hz)
  const float f_elec = 50.0f;
  const uint32_t t_sector_us = (uint32_t)(1e6f / (6.0f * f_elec));

  // Avanza un paso (S0..S5) con dead-time entre sectores
  stepNext(duty_cmd);
  delayMicroseconds(t_sector_us);
}