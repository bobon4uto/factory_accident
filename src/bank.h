#ifdef    MONO_BUILD
#define BANK_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _BANK_H_
#define    _BANK_H_
// bank interface
#include "noutil.h"


void bank_start_income_calculation();
void bank_add_income(u income);
void bank_update();
void bank_draw();
bool bank_pay(u price);

#ifdef      BANK_IMPLEMENTATION
// bank implementation

#include "timer.h"
#include "common_text.h"
#include "rayinc.h"

typedef struct sBank {
  u money;
  u income;
  u last_income;
  float second_timer;
  float second_timer_reset;
} Bank;

Bank bank = (Bank){
  .money = 0,
  .income = 0,
  .last_income = 0,
  .second_timer = 0.0f,
  .second_timer_reset = 1.0f,
};
void bank_start_income_calculation() {
  bank.income = 0.0f;
}
void bank_add_income(u income) {
  bank.income += income;
}
void bank_update() {
  timer_countdown_frame(&bank.second_timer);
  bank.second_timer -= 1.0f/60.0f;

  if (bank.last_income > bank.income) {
    // prevent mid-second income drop (eg worker stopped working mid second)
    bank.income = bank.last_income;
  } else {
    bank.last_income = bank.income;
  }

  if (bank.second_timer < 0.0f) {
    bank.second_timer = bank.second_timer_reset;
    bank.money += bank.income;
    // at the end of the second reset last_income since income can change next second
    bank.last_income = 0.0f;
  }
}
void bank_draw() {
  draw_rectangle_gradient_h(0, 0, 720, 32, BLACK, fade(BLACK, 0.1));
  text_draw(text_format("%zu +%zu", bank.money, bank.income),(Vector2){0,0},YELLOW);
}
bool bank_pay(u price) {
  // TODO: add visuals
  if (bank.money >= price) {
    bank.money -= price;
    return true;
  }
  return false;
}


#endif   // BANK_IMPLEMENTATION
#endif   //_BANK_H_

