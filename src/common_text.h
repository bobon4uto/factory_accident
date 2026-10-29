#ifdef    MONO_BUILD
#define COMMON_TEXT_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _COMMON_TEXT_H_
#define    _COMMON_TEXT_H_
// common_text interface

#include "rayinc.h"
#include "assets.h"

void text_draw(const char* text, Vector2 position, Color color);
void text_draw_above(const char* text, Vector2 position, Color color);
void text_draw_bounded(const char* text, Rectangle bound_rec, Color color);

#ifdef      COMMON_TEXT_IMPLEMENTATION
// common_text implementation

#define FONT_SCALE 32

void text_draw(const char* text, Vector2 position, Color color) {
  draw_text_pro(a_font(FONT_IOSEVKA), text, position, (Vector2){0,0}, 0.0f, FONT_SCALE, 0.0f, color);
}
void text_draw_above(const char* text, Vector2 position, Color color) {
  Vector2 size = measure_text_ex(a_font(FONT_IOSEVKA), text, FONT_SCALE, 0.0f);
  Vector2 above = position;
  above.y = position.y - size.y;
  draw_text_pro(a_font(FONT_IOSEVKA), text, above, (Vector2){0,0}, 0.0f, FONT_SCALE, 0.0f, color);
}
void text_draw_bounded(const char* text, Rectangle bound_rec, Color color) {
  Vector2 size = measure_text_ex(a_font(FONT_IOSEVKA), text,FONT_SCALE, 0.0f);
  float x_ = bound_rec.width;
  float y_ = bound_rec.height;

  float draw_dx = (x_ - size.x)/2;
  float draw_dy = (y_ - size.y)/2;

  Vector2 pos = {0};
  pos.x = bound_rec.x + draw_dx;
  pos.y = bound_rec.y + draw_dy;
  text_draw(text, pos, color);
}


#endif   // COMMON_TEXT_IMPLEMENTATION
#endif   //_COMMON_TEXT_H_

