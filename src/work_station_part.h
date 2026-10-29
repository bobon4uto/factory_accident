#ifdef    MONO_BUILD
#define WORK_STATION_PART_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _WORK_STATION_PART_H_
#define    _WORK_STATION_PART_H_
// work_station_part interface

#include "entity.h"
#include "transform2d.h"
#include "animation.h"
#include "assets.h"
#include "random.h"
#include "particle.h"

typedef struct sWorkStationPart {
  Transform2D transform;
  SpriteID sprite;
  SpriteID particle;
  Animation animation;
} WorkStationPart;

static_assert(sizeof(WorkStationPart) <= MAX_ENTITY_SIZE, "Must fit in entity");

WorkStationPart  work_station_part_base();
WorkStationPart work_station_part_new(SpriteID sprite, AnimationID animation);
u       work_station_part_spawn(WorkStationPart wsp);
void    work_station_part_update(Entity* e);
void    work_station_part_draw(Entity* e);

#ifdef      WORK_STATION_PART_IMPLEMENTATION
// work_station_part implementation

WorkStationPart  work_station_part_base() {
  WorkStationPart wsp = {0};

  wsp.transform = (Transform2D) {
    .position = (Vector2){0.0f, 0.0f},
    .scale = (Vector2){1.0f, 1.0f},
    .rotation = 0.0f,
  };
  wsp.sprite = SPRITE_WORK_STATION_PRESS_PART_PISTONS;
  wsp.particle = SPRITE_PARTICLE_FIRE_1;

  return wsp;
}
WorkStationPart work_station_part_new(SpriteID sprite, AnimationID animation) {
  WorkStationPart wsp = work_station_part_base();

  wsp.sprite = sprite;
  wsp.animation = animation_get(animation);

  return wsp;
}
u       work_station_part_spawn(WorkStationPart wsp) {
  Entity e = {0};

  e.update_fn = work_station_part_update;
  e.draw_fn   = work_station_part_draw;

  *e_as(WorkStationPart, &e) = wsp;

  return entity_spawn(e);
}
void    work_station_part_update(Entity* e) {
  WorkStationPart* wsp = e_as(WorkStationPart, e);

  // anim action = particles
  if ( animation_apply(&wsp->animation, &wsp->transform) ) {
    for (u i = 0; i < 4; ++i) {
      particle_spawn_at(wsp->particle + random_u32()%3,
        wsp->transform.position, (Vector2){ 0.0f, 0.0f },
        (Vector2){0,0}, (Vector2){ 1.0f, 1.0f}
        );
    }
  }

}
void    work_station_part_draw(Entity* e) {
  WorkStationPart* wsp = e_as(WorkStationPart, e);

  transform2d_draw_texture(wsp->transform, a_sprite(wsp->sprite), WHITE);
}


#endif   // WORK_STATION_PART_IMPLEMENTATION
#endif   //_WORK_STATION_PART_H_

