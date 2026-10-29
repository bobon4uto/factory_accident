#ifdef    MONO_BUILD
#define SPAWNER_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _SPAWNER_H_
#define    _SPAWNER_H_
// spawner interface
#include "rayinc.h"

void spawner_update();
void spawner_draw();
#define HOME_REC (Rectangle){ -200, -50, 400, 200 }

#ifdef      SPAWNER_IMPLEMENTATION
// spawner implementation

#include "timer.h"
#include "worker.h"
#include "random.h"
#include "input.h"
#include "bank.h"
#include "common_text.h"

typedef struct sSpawner {
  float spawn_timer;
  float spawn_timer_reset;
  u price;
  Rectangle area;
} Spawner;
Spawner spawner = {
  .spawn_timer = 30.0f,
  .spawn_timer_reset = 30.0f,
  .area = (Rectangle){ -150, -200, 300, 100 },
  .price = 1,
};
void spawner_update() {
  timer_countdown_frame(&spawner.spawn_timer);
  if (spawner.spawn_timer < 0.0f) {
    Worker w = worker_base();
    w.transform.position = (Vector2){
      random_float_in_range(HOME_REC.x, HOME_REC.x + HOME_REC.width),
      random_float_in_range(HOME_REC.y, HOME_REC.y + HOME_REC.height),
    };
    spawner.spawn_timer = spawner.spawn_timer_reset;
    worker_spawn(w);
  }
  Input i = input_get();
  if (i.click) {
    if ( check_collision_point_rec( i.pos, spawner.area ) ) {
      if (spawner.spawn_timer_reset > 1.5f) {
        if ( bank_pay(spawner.price) ) {
          spawner.spawn_timer_reset -= 1.0f;
          spawner.price *= 2;
        }
      }
    }
  }

}
void spawner_draw() {
  if (spawner.spawn_timer_reset > 1.5f) {
    draw_rectangle_rec( spawner.area, fade(GRAY, 0.5f) );
    draw_rectangle_rec( HOME_REC, fade(GRAY, 0.1f) );
    text_draw_bounded(
        text_format(  "new worker %.0f/%.0f\npay %zu to speed up", spawner.spawn_timer, spawner.spawn_timer_reset, spawner.price  ) ,
        spawner.area, BLACK);
  }
}

#endif   // SPAWNER_IMPLEMENTATION
#endif   //_SPAWNER_H_

