#ifdef    MONO_BUILD
#define INPUT_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _INPUT_H_
#define    _INPUT_H_
// input interface

#include "rayinc.h"
#include "cam.h"

typedef struct sInput {
  Vector2 pos;
  float zoom;
  bool click;
  bool release;
  bool undo;
  bool reset_zoom;
  bool keyboard_lock;
} Input;
void input_update();
Input input_get();

#ifdef      INPUT_IMPLEMENTATION
// input implementation

Input input = {0};

void input_update() {
  Camera2D camera = cam_get();
  Vector2 mouse_move = get_mouse_delta();
  bool use_mouse = is_cursor_on_screen();
  if ( is_key_down(KEY_W) || is_key_down(KEY_K) || is_key_down(KEY_UP   )
    || is_key_down(KEY_S) || is_key_down(KEY_J) || is_key_down(KEY_DOWN )
    || is_key_down(KEY_A) || is_key_down(KEY_H) || is_key_down(KEY_LEFT )
    || is_key_down(KEY_D) || is_key_down(KEY_L) || is_key_down(KEY_RIGHT) ) {
    input.keyboard_lock = true;
  }
  if (input.keyboard_lock) {
    use_mouse = !vector2_equals(mouse_move, vector2_zero());
  }
  if (use_mouse) {
    input.keyboard_lock = false;
    Vector2 screen_pos = get_mouse_position();
    Vector2 world_pos = vector2_add(  vector2_scale(  vector2_subtract(screen_pos, camera.offset) , 1.0f/camera.zoom)    ,  camera.target);
    input.pos = world_pos;
    input.click = is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    input.release = is_mouse_button_released(MOUSE_BUTTON_LEFT);
  } else {
    float speed = 5.0f;
    if ( is_key_down(KEY_LEFT_SHIFT) || is_key_pressed(KEY_RIGHT_SHIFT) ) { speed*=4; }

    if ( is_key_down(KEY_W) || is_key_down(KEY_K) || is_key_down(KEY_UP   ) ) { input.pos.y -= speed; }
    if ( is_key_down(KEY_S) || is_key_down(KEY_J) || is_key_down(KEY_DOWN ) ) { input.pos.y += speed; }
    if ( is_key_down(KEY_A) || is_key_down(KEY_H) || is_key_down(KEY_LEFT ) ) { input.pos.x -= speed; }
    if ( is_key_down(KEY_D) || is_key_down(KEY_L) || is_key_down(KEY_RIGHT) ) { input.pos.x += speed; }

    input.reset_zoom = is_key_pressed(KEY_P);

    input.click = is_key_pressed(KEY_SPACE);
    input.release = is_key_released(KEY_SPACE);
    input.undo = is_key_pressed(KEY_Q);
  }


  if      ( is_key_down(KEY_O) ) { input.zoom = -1.0f; }
  else if ( is_key_down(KEY_I) ) { input.zoom = 1.0f;  }
  else { input.zoom = get_mouse_wheel_move();  }



  input.click =
       is_mouse_button_pressed(MOUSE_BUTTON_LEFT)
    ||                  is_key_pressed(KEY_SPACE)
    ||                  is_key_pressed(KEY_E)
    ;
  input.release =
       is_mouse_button_released(MOUSE_BUTTON_LEFT)
    ||                  is_key_released(KEY_SPACE)
    ||                  is_key_released(KEY_E)
    ;
  input.undo =
       is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)
    ||                  is_key_pressed(KEY_Q)
    ;
  float bound = 1200.0f;
  if (input.pos.x < -bound) {
    input.pos.x = -bound;
  }
  if (input.pos.y < -bound) {
    input.pos.y = -bound;
  }
  if (input.pos.x > bound) {
    input.pos.x = bound;
  }
  if (input.pos.y > bound) {
    input.pos.y = bound;
  }
}
Input input_get() {
  return input;
}

#endif   // INPUT_IMPLEMENTATION
#endif   //_INPUT_H_

