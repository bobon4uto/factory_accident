#ifdef    MONO_BUILD
#define TRANSFORM2D_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _TRANSFORM2D_H_
#define    _TRANSFORM2D_H_
// transform2d interface

#include "rayinc.h"
#include "fmath.h"
// I didnt like rectangle approach tbh so two vectors!
typedef struct sTransform2D {
  Vector2 position;
  Vector2 scale;
  float rotation;
} Transform2D;
void transform2d_draw_texture(Transform2D transform, Texture sprite, Color tint);
Transform2D transform2d_add_delta(Transform2D transform, Transform2D delta);
Transform2D transform_interpolate_linear(Transform2D transform0, Transform2D transform1, float progress);

#ifdef      TRANSFORM2D_IMPLEMENTATION
// transform2d implementation

void transform2d_draw_texture(Transform2D transform, Texture sprite, Color tint) {
  Rectangle src_rec = (Rectangle){ 0, 0, sprite.width, sprite.height };
  Rectangle dst_rec = (Rectangle) {
    transform.position.x - (transform.scale.x-1.0f)*sprite.width/2,
    transform.position.y - (transform.scale.y-1.0f)*sprite.height/2,
    sprite.width  * transform.scale.x,
    sprite.height * transform.scale.y,
  };
  Vector2 origin = (Vector2){ sprite.width/2, sprite.height/2 };
  float rotation = transform.rotation;
  draw_texture_pro(sprite, src_rec, dst_rec, origin, rotation, tint);
}
Transform2D transform2d_add_delta(Transform2D transform, Transform2D delta) {
  return (Transform2D) {
    .position = vector2_add(transform.position, delta.position),
    .scale =  vector2_add(transform.scale, delta.scale),
    .rotation = transform.rotation + delta.rotation,
  };
}



Transform2D transform_interpolate_linear(Transform2D transform0, Transform2D transform1, float progress) {
  Transform2D transform = {0};

  transform.position = vector2_lerp(transform0.position, transform1.position, progress);
  transform.scale = vector2_lerp(transform0.scale, transform1.scale, progress);
  transform.rotation = float_lerp(transform0.rotation, transform1.rotation, progress);

  return transform;
}

#endif   // TRANSFORM2D_IMPLEMENTATION
#endif   //_TRANSFORM2D_H_

