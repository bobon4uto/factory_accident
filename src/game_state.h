#ifdef    MONO_BUILD
#define GAME_STATE_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _GAME_STATE_H_
#define    _GAME_STATE_H_
// game_state interface

#include "rayinc.h"
#include "noutil.h"

// this time i decided to do modulized approach without huge game state

#include "input.h"
#include "shapes.h"

#include "assets.h"
#include "common_text.h"
#include "cam.h"
#include "entity.h"

#include "bank.h"
#include "animation.h"

#include "play_map.h"

#include "entity_selector.h"
#include "spawner.h"

typedef struct sGameState {
  u screen_x_;
  u screen_y_;
} GameState;

// :fGameState
GameState gs_init();
void      gs_update(GameState* self);
void      gs_reset (GameState* self);
void      gs_draw  (GameState* self);
void      gs_free  (GameState* self);

#ifdef      GAME_STATE_IMPLEMENTATION
// game_state implementation


// :i





// :iGameState
GameState gs_init() {
  GameState self = {0};

  self.screen_x_ = 720;
  self.screen_y_ = 720;


  init_window(self.screen_x_, self.screen_y_, "Factory accident");
  init_audio_device();

  // :init modules
  assets_init();
  animations_init();

  map_spawn();

  return self;
}
void gs_reset(GameState* self) {
  gs_free(self);
  *self = gs_init();
}



void      gs_update(GameState* self) {
  (void)self;
  input_update  ();
  cam_update    ();
  bank_start_income_calculation();

  selector_start();

  entitys_update();

  selector_update();
  bank_update();
  spawner_update();
}
void      gs_draw  (GameState* self) {
  (void)self;
  begin_drawing();
  clear_background(RAYWHITE);

  cam_begin();
  draw_rectangle_gradient_ex((Rectangle){-4000,-4000,8000,8000}, RED, GREEN, BLUE, WHITE);
  text_draw("Windmill is low risk,\nincidents happen rarely.\nIncome: 1/s per worker", (Vector2){400, 400}, BLACK);
  text_draw("Cave of the deathbringer has 0% survival rate.\nNo one has escaped it alive, or dead for that matter.\nIncome 100/s but workers last only 4 seconds in it\n"
      "(For some reason workers with stable career love going there as a hobby)", (Vector2){-300, 600}, BLACK);
  text_draw("Press is medium risk,\nincidents happen often.\nIncome: 5/s per worker", (Vector2){-700, 250}, BLACK);
  text_draw("Black Hole generator is high risk,\nincidents happen non-stop.\nIncome: 10/s per worker", (Vector2){-250, -1200}, BLACK);
  entitys_draw();

  spawner_draw();
  // pointer
  selector_draw();
  shapes_draw_plus(input_get().pos, 10, RED);

  cam_end();
  bank_draw();

    end_drawing();
}

void      gs_free  (GameState* self) {
  (void)self;
  // :free modules
  //close_audio_device();
  //close_window();
  //assets_free();
}



#endif   // GAME_STATE_IMPLEMENTATION
#endif   //_GAME_STATE_H_

