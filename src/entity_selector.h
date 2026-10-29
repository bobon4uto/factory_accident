#ifdef    MONO_BUILD
#define ENTITY_SELECTOR_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _ENTITY_SELECTOR_H_
#define    _ENTITY_SELECTOR_H_
// entity_selector interface
#include "rayinc.h"

void        selector_start();
void        selector_update();
void        selector_draw();

void         start_selection();
void           end_selection();
void         close_selection();
void   force_close_selection();
bool            is_selecting();
bool just_finished_selecting();

Rectangle get_selection();

#ifdef      ENTITY_SELECTOR_IMPLEMENTATION
// entity_selector implementation

#include "input.h"

static Vector2 start_point = {0};
static Vector2   end_point = {0};
static bool selecting = false;
static bool finished_selecting = false;


void        selector_start() {
  Input i = input_get();

  if (i.click) {
    start_selection();
  } else if (i.release) {
    end_selection();
  }
  end_point = i.pos;
}
void        selector_update() {
  close_selection();
}
void        selector_draw() {
  if (is_selecting()) {
    draw_rectangle_rec( get_selection() , fade(BLUE, 0.2f));
  }
}

void         start_selection() {
  Input i = input_get();

  start_point = i.pos;
  end_point = start_point;
  selecting = true;
  finished_selecting = false;
}
void           end_selection() {
  Input i = input_get();

  end_point = i.pos;
  selecting = false;
  finished_selecting = true;
}
void         close_selection() {
  finished_selecting = false;
}
void   force_close_selection() {
  end_selection();
  close_selection();
}
bool            is_selecting() {
  return selecting;
}
bool just_finished_selecting() {
  return finished_selecting;
}

Rectangle rectangle_from_corners(Vector2 c1, Vector2 c2) {
  Rectangle rect = {0};

  float x_min = fminf( c1.x, c2.x );
  float y_min = fminf( c1.y, c2.y );

  float x_max = fmaxf( c1.x, c2.x );
  float y_max = fmaxf( c1.y, c2.y );

  rect.x = x_min;
  rect.y = y_min;
  rect.width = x_max - x_min;
  rect.height = y_max - y_min;

  return rect;
}

Rectangle get_selection() {
  return rectangle_from_corners(start_point, end_point);
}



#endif   // ENTITY_SELECTOR_IMPLEMENTATION
#endif   //_ENTITY_SELECTOR_H_

