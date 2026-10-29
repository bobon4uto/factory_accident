#ifdef    MONO_BUILD
#define PARTICLE_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _PARTICLE_H_
#define    _PARTICLE_H_
// particle interface

#include "rayinc.h"
#include "assets.h"
#include "entity.h"
#include "transform2d.h"

typedef struct sParticle {
  Transform2D transform;
  Transform2D delta;
  SpriteID sprite;
  float left_to_live;
} Particle;

static_assert(sizeof(Particle) <= MAX_ENTITY_SIZE, "Must fit in entity");

u particle_spawn (Particle p);
u particle_spawn_at(SpriteID sprite,
    Vector2 position, Vector2 random_position,
    Vector2 velocity, Vector2 random_velocity
    );
void         particle_update(Entity* e);
void         particle_draw  (Entity* e);

#ifdef      PARTICLE_IMPLEMENTATION
// particle implementation

#include "random.h"
#include "timer.h"
u particle_spawn (Particle p) {
  Entity e = {0};

  e.update_fn = particle_update;
  e.draw_fn = particle_draw;

  *e_as(Particle, &e) = p;

  //printf("spawning particle\n");
  //printf("%f\n", e_as(Particle, &e)->transform.scale.x);
  //fflush(stdout);

  return entity_spawn(e);
}
u particle_spawn_at(SpriteID sprite,
    Vector2 position, Vector2 random_position,
    Vector2 velocity, Vector2 random_velocity
    ) {
  Particle p = {0};

  p.sprite = sprite;
  p.transform.position = position;
  p.transform.position.x += random_float_in_range(-random_position.x, random_position.x);
  p.transform.position.y += random_float_in_range(-random_position.y, random_position.y);
  p.transform.scale = (Vector2){1.0f, 1.0f};
  p.transform.rotation = 0.0f;

  p.delta.position = velocity;
  p.delta.position.x += random_float_in_range(-random_velocity.x, random_velocity.x);
  p.delta.position.y += random_float_in_range(-random_velocity.y, random_velocity.y);
  p.delta.scale = (Vector2){0.0f, 0.0f};
  p.delta.rotation = 0.0f;

  p.left_to_live = 1.0f;

  return particle_spawn(p);
}
void         particle_update(Entity* e) {
  Particle* p = e_as(Particle, e);
  timer_countdown_frame(&p->left_to_live);
  if (p->left_to_live < 0.0f) {
    entity_delete(e);
    return;
  }
  p->transform = transform2d_add_delta(p->transform, p->delta);

}
void         particle_draw  (Entity* e) {
  Particle* p = e_as(Particle, e);
  transform2d_draw_texture(p->transform, a_sprite(p->sprite), fade(WHITE, p->left_to_live));
}


#endif   // PARTICLE_IMPLEMENTATION
#endif   //_PARTICLE_H_

