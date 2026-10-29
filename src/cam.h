#ifdef    MONO_BUILD
#define CAM_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _CAM_H_
#define    _CAM_H_
// cam interface

#include "rayinc.h"

void cam_update();
void cam_begin();
void cam_end();
Camera2D cam_get();

#ifdef      CAM_IMPLEMENTATION
// cam implementation

#include "config.h"
#include "input.h"
#include "fmath.h"

Camera2D cam = {
    (Vector2){SCREEN_X_/2, SCREEN_Y_/2},
    (Vector2){0,0},
    0.0f,
    1.0f,
};

static bool coming_back = false;

void cam_update() {
  Input i = input_get();

  if (cam.zoom > 0.2f) {
    float zoom_speed = 0.1f;
    cam.zoom += i.zoom * zoom_speed;
  }
  if (cam.zoom < 0.2f) {
    cam.zoom = 0.21f;
  }
  if (i.reset_zoom) {
    cam.zoom = 1.0f;
  }
  float zoom = cam.zoom;

  float dist = vector2_distance(cam.target, i.pos) * zoom;
  float spring_len = 200.0f;
  float spring_power = 0.1f;
  float spring_overshoot = 100.0f;
  if (dist > spring_len) {
    coming_back = true;
    float move_by = fmaxf(1.0f, (dist-float_middle(spring_overshoot, spring_len)) * spring_power);
    cam.target = vector2_move_towards(cam.target, i.pos, move_by);
    return;
  }
  if (dist < spring_overshoot) {
    coming_back = false;
  }
  if (coming_back) {
    float move_by = fmaxf(1.0f, (dist-spring_overshoot) * spring_power);
    cam.target = vector2_move_towards(cam.target, i.pos, move_by);
  }
}
void cam_begin() {
  begin_mode2_d(cam);
}

void cam_end() {
  end_mode2_d();
}

Camera2D cam_get() {
  return cam;
}

#endif   // CAM_IMPLEMENTATION
#endif   //_CAM_H_

