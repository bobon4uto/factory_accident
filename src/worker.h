#ifdef    MONO_BUILD
#define WORKER_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _WORKER_H_
#define    _WORKER_H_
// worker interface


#include "entity.h"
#include "rayinc.h"
#include "name.h"
#include "assets.h"
#include "transform2d.h"
#include "animation.h"

typedef struct sWorker {
  Vector2 home;
  Transform2D transform;
  Animation animation;
  SpriteID sprite;
  NameID name;
  u work_station;
  u sound_type;
  bool selected;
} Worker;

static_assert(sizeof(Worker) <= MAX_ENTITY_SIZE, "Must fit in entity");

Worker worker_base();
u worker_spawn(Worker w);
Worker* worker_in (Entity* e);
void worker_update(Entity* e);
void worker_draw  (Entity* e);
void worker_free  (Entity* e);
void say_help(Worker* w);

#ifdef      WORKER_IMPLEMENTATION
// worker implementation

#include "assert.h"
#include "input.h"
#include "entity_selector.h"
#include "config.h"
#include "shapes.h"
#include "bank.h"
#include "common_text.h"
#include "particle.h"
#include "random.h"
#include "work_station.h"
#include "spawner.h"

static const float interaction_range = 64.0f;

void say_whee(Worker* w) {
  a_sound_play(SOUND_WHEE_1 + w->sound_type);
}
void say_got_it(Worker* w) {
  a_sound_play(SOUND_GOT_IT_1 + w->sound_type);
}
void say_ok(Worker* w) {
  a_sound_play(SOUND_OK_1+ w->sound_type);
}
void say_help(Worker* w) {
  a_sound_play(SOUND_HELP_1 + w->sound_type);
}

Worker worker_base() {
  Worker w = {0};
  w.transform = (Transform2D){
    .position = (Vector2){ 0.0f, 0.0f},
    .scale = (Vector2){ 1.0f, 1.0f},
    .rotation = 0.0f,
  };
  w.home = w.transform.position;
  w.sprite = SPRITE_BOT_BODY_STEEL+random_u32()%3;
  w.name = random_u32()%NAME_COUNT;
  w.sound_type = random_u32()%3;
  return w;
}
u worker_spawn(Worker w) {
  Entity e = {0};

  e.update_fn = worker_update;
  e.draw_fn   = worker_draw;
  e.free_fn   = worker_free;

  w.home = w.transform.position;
  //Worker* e_worker = e_as(Worker, &e);
  *e_as(Worker, &e) = w;

  return entity_spawn(e);
}
Worker* worker_in(Entity* e) {
  if (e->update_fn != worker_update) {
    return NULL;
  }
  return e_as(Worker, e);
}
void worker_update(Entity* e) {
  Worker* w = worker_in(e);
  Input i =   input_get();
  (void)w;
  (void)i;
  if ( animation_apply(&w->animation, &w->transform) ) {
    entity_delete(e);
  }

  if ( just_finished_selecting() ) {
    if ( check_collision_circle_rec( w->transform.position, interaction_range, get_selection() ) ) {
      w->selected = true;
    }
  }


  if (i.undo) {
    w->selected = false;
  }


  if (w->selected) {
    if ( work_stations_need_workers() ) {
      if ( work_stations_add_worker(e) ) {
        if ( w->work_station == get_cave() ) {
          say_whee(w);
        } else {
          if ( random_bool() ) {
            if ( random_bool() ) {
              if ( random_bool() ) {
                say_got_it(w);
              } else {
                say_ok(w);
              }
            }
          }
        }
        w->selected = false;
      }
    }

    if ( i.click ) {
      if (check_collision_point_rec( i.pos, HOME_REC )) {
        w->work_station = 0;
        w->animation = (Animation){0};
      }
    }
  }

  if (w->work_station == 0) {
    w->transform.scale  = (Vector2){1.0f, 1.0f};
    w->transform.rotation = 0.0f;
    w->transform.position = vector2_move_towards(w->transform.position, w->home, 10.0f);
  }


  //if (i.click) {
  //  entity_delete(e);
  //}

}
void worker_draw(Entity* e) {
  Worker* w = worker_in(e);

  transform2d_draw_texture(w->transform, a_sprite(w->sprite), WHITE);

  Input i = input_get();

#define NAME_COLOR_NONE fade(WHITE, 0.0f)
#define NAME_COLOR_DEFAULT BLACK
#define NAME_COLOR_SELECTING GREEN
#define NAME_COLOR_SELECTED BLUE

  Color name_color = NAME_COLOR_NONE;
  if ( check_collision_point_circle(i.pos, w->transform.position, interaction_range) ) {
    name_color = NAME_COLOR_DEFAULT;
  }
  if (w->selected) {
    name_color = NAME_COLOR_SELECTED;
  }
  if ( is_selecting() ) {
    if ( check_collision_circle_rec( w->transform.position, interaction_range, get_selection() ) ) {
      name_color = NAME_COLOR_SELECTING;
    }
  }
  text_draw_bounded(name_get(w->name),
      (Rectangle){ w->transform.position.x - 32.0f, w->transform.position.y - 64.0f, 64.0f, 32.0f },
      name_color
  );

  if ( config_bool("debug") ) {
    shapes_draw_plus(w->transform.position, 10, RED);
  }


}
void worker_free(Entity* e) {
  Worker* w = worker_in(e);
  // explode on free
  for (u i = 0; i < 100; ++i) {
    particle_spawn_at(SPRITE_PARTICLE_ELECTRO_1 + random_u32()%3,
      w->transform.position, (Vector2){ 32.0f, 32.0f },
      (Vector2){0,0}, (Vector2){ 5.0f, 5.0f}
      );
  }
}


#endif   // WORKER_IMPLEMENTATION
#endif   //_WORKER_H_

