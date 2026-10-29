#ifdef    MONO_BUILD
#define SHAPES_IMPLEMENTATION
#endif // MONO_BUILD
#ifndef    _SHAPES_H_
#define    _SHAPES_H_
// shapes interface
#include "rayinc.h"
void shapes_draw_plus(Vector2 position, float wing_size, Color color);

#ifdef      SHAPES_IMPLEMENTATION
// shapes implementation

void shapes_draw_plus(Vector2 position, float wing_size, Color color) {
    Vector2 top = vector2_add(position, (Vector2){ 0, -wing_size});
    Vector2 bot = vector2_add(position, (Vector2){ 0,  wing_size});
    Vector2 lef = vector2_add(position, (Vector2){-wing_size,  0});
    Vector2 rig = vector2_add(position, (Vector2){ wing_size,  0});
    draw_line_v(top, bot, color);
    draw_line_v(lef, rig, color);
}



#endif   // SHAPES_IMPLEMENTATION
#endif   //_SHAPES_H_

