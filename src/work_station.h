#ifdef    MONO_BUILD
#define WORK_STATION_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _WORK_STATION_H_
#define    _WORK_STATION_H_
// work_station interface

#include "rayinc.h"
#include "animation.h"
#include "entity.h"
#include "logging.h"

#define MAX_WORK_STATION_WORKERS 4
#define MAX_WORK_STATION_PARTS 4

typedef struct sWorkStation {
  Vector2 position;
  u income;
  float min_incident_time;
  float max_incident_time;
  float incident_countdown;
  float incident_ongoing_time_reset;
  EntityRef workers[MAX_WORK_STATION_WORKERS];
  EntityRef parts[MAX_WORK_STATION_PARTS];

  AnimationID workers_idle[MAX_WORK_STATION_WORKERS];
  AnimationID parts_idle[MAX_WORK_STATION_PARTS];

  AnimationID workers_victim_incident;
  AnimationID workers_incident[MAX_WORK_STATION_WORKERS];
  AnimationID parts_incident[MAX_WORK_STATION_PARTS];

  Rectangle interaction_area;
  bool incident_is_on;
} WorkStation;

static_assert(sizeof(WorkStation) <= MAX_ENTITY_SIZE, "Must fit in entity");

WorkStation  work_station_base();

void work_station_spawn_all();

u       work_station_spawn(WorkStation w);
void    work_station_update(Entity* e);
void    work_station_draw(Entity* e);
void work_station_do_incident(WorkStation* ws);
void work_station_do_idle    (WorkStation* ws);
bool work_stations_need_workers();
bool work_stations_add_worker(Entity* e_worker); // will be false if no space
u get_w();
u get_press();
u get_cave();
u get_p_a();

#ifdef      WORK_STATION_IMPLEMENTATION
// work_station implementation

#include "work_station_part.h"
#include "bank.h"
#include "timer.h"
#include "input.h"
#include "random.h"
#include "worker.h"
#include "config.h"
#include "entity_selector.h"
void work_station_attach(WorkStation* ws, WorkStationPart wsp, u i);
void work_station_stop_asking_for_workers(Entity* e);
void work_station_ask_for_workers(Entity* e);
void work_station_shift_anim_position(WorkStation ws);


static u w = 0;
static u press = 0;
static u cave = 0;
static u p_a = 0;
u get_w() {
  return w;
}
u get_press() {
  return press;
}
u get_cave() {
  return cave;
}
u get_p_a() {
  return p_a;
}

EntityRef asking_for_workers = {0};
bool work_stations_need_workers() {
  return asking_for_workers.id != 0;
}
bool work_stations_add_worker(Entity* e_worker) {
  Entity* e_station = entity_ref_get(&asking_for_workers);
  WorkStation* ws = e_as(WorkStation, e_station);

  for (u i = 0; i < MAX_WORK_STATION_WORKERS; ++i) {
    Entity* w = entity_ref_get(&ws->workers[i]);
    if (w != NULL) {
      // taken
    } else {
      ws->workers[i] = entity_ref_make(e_worker);

      Worker* w = e_as(Worker, e_worker);

      w->work_station = e_station->id;

      w->animation = animation_get(ws->workers_idle[i]);
      return true;
    }
  }

  // no space, stop asking for workers
  asking_for_workers.id = 0;

  return false;
}

WorkStation  work_station_base() {
  WorkStation ws = {0};

  ws.position = (Vector2){0,0};
  ws.income = 1;
  ws.min_incident_time = 10.0f;
  ws.max_incident_time = 100.0f;
  ws.incident_countdown = 5.0f;
  ws.incident_ongoing_time_reset = 5.0f;

  ws.incident_is_on = false;
  ws.interaction_area = (Rectangle){ 0.0f, 0.0f, 10.0f, 10.0f };

  return ws;
}
u work_station_spawn_press() {
  WorkStation ws = work_station_base();

  ws.interaction_area = (Rectangle){ -250.0f -500.0f, -350.0f , 500.0f, 375.0f };
  ws.position = (Vector2){-500.0f, 0.0f};
  ws.incident_ongoing_time_reset = 11.6f;

  ws.min_incident_time = 30.0f;
  ws.max_incident_time = 60.0f;
  ws.incident_countdown = 10.0f;
  ws.income = 5;

  u part_i = 0;
  u worker_i = 0;
#define PART_(sprite, animation) \
  work_station_attach(&ws, work_station_part_new(sprite, animation##_IDLE), part_i); \
  ws.parts_idle[part_i] = animation##_IDLE; \
  ws.parts_incident[part_i] = animation##_INCIDENT; \
  part_i++

#define WORKER_(animation) \
  ws.workers_idle[worker_i] = animation##_IDLE; \
  ws.workers_incident[worker_i] = animation##_INCIDENT; \
  worker_i++


  PART_(SPRITE_WORK_STATION_PRESS_PART_PLATES,  ANIM_PRESS_PART_PLATES);
  PART_(SPRITE_WORK_STATION_PRESS_PART_PISTONS, ANIM_PRESS_PART_PISTON);
  PART_(SPRITE_WORK_STATION_PRESS_PART_TOP,     ANIM_PRESS_PART_TOP);

  WORKER_(ANIM_PRESS_WORKER_1);
  WORKER_(ANIM_PRESS_WORKER_2);
  WORKER_(ANIM_PRESS_WORKER_3);
  WORKER_(ANIM_PRESS_WORKER_4);
  ws.workers_victim_incident = ANIM_PRESS_WORKER_VICTIM;


  work_station_shift_anim_position(ws);
  return work_station_spawn(ws);
}
u work_station_spawn_cave() {
  WorkStation ws = work_station_base();
  ws.incident_ongoing_time_reset = 0.0f;
  ws.income = 100;
  ws.position = (Vector2){ 0.0f, 400.0f };
  ws.interaction_area = (Rectangle){ -200.0f + ws.position.x, -150.0f + ws.position.y, 400.0f, 320.0f };


  work_station_attach(&ws, work_station_part_new(SPRITE_WORK_STATION_CAVE_PART_MAIN, ANIM_CAVE_PART_BASE), 0);
  ws.parts_idle[0] = ANIM_CAVE_PART_BASE;

  ws.workers_idle[0] = ANIM_CAVE_WORKER_1_IDLE;
  ws.workers_idle[1] = ANIM_CAVE_WORKER_2_IDLE;
  ws.workers_idle[2] = ANIM_CAVE_WORKER_3_IDLE;
  ws.workers_idle[3] = ANIM_CAVE_WORKER_4_IDLE;

  work_station_shift_anim_position(ws);
  return work_station_spawn(ws);
}
u work_station_spawn_w() {
  WorkStation ws = work_station_base();
  ws.incident_ongoing_time_reset = 9.0f;


  ws.min_incident_time = 60.0f;
  ws.max_incident_time = 120.0f;
  ws.incident_countdown = 120.0f;

  ws.income = 1;
  ws.position = (Vector2){ 700.0f, 0.0f };
  ws.interaction_area = (Rectangle){ -250.0f + ws.position.x, -250.0f + ws.position.y, 500.0f, 500.0f };


  u part_i = 0;
  u worker_i = 0;
  PART_(SPRITE_WORK_STATION_W_BASE,  ANIM_W_BASE);
  PART_(SPRITE_WORK_STATION_W_WINGS, ANIM_W_WINGS);

  WORKER_(ANIM_W_WORKER_1);
  WORKER_(ANIM_W_WORKER_2);
  WORKER_(ANIM_W_WORKER_3);
  WORKER_(ANIM_W_WORKER_4);
  ws.workers_victim_incident = ANIM_W_WORKER_VICTIM;

  work_station_shift_anim_position(ws);
  return work_station_spawn(ws);
}
u work_station_spawn_p_a() {
  WorkStation ws = work_station_base();
  ws.incident_ongoing_time_reset = 11.0f;
  ws.position = (Vector2){ 0.0f, -800.0f };
  ws.interaction_area = (Rectangle){ -250.0f + ws.position.x, -250.0f + ws.position.y, 500.0f, 500.0f };

  ws.min_incident_time = 10.0f;
  ws.max_incident_time = 15.0f;
  ws.incident_countdown = 10.0f;
  ws.income = 10;

  u part_i = 0;
  u worker_i = 0;
  PART_(SPRITE_WORK_STATION_P_A_BLACK_HOLE,  ANIM_P_A_BLACK_HOLE);
  PART_(SPRITE_WORK_STATION_P_A_ENERGY, ANIM_P_A_ENERGY);
  PART_(SPRITE_WORK_STATION_P_A_WALLS, ANIM_P_A_WALLS);


  WORKER_(ANIM_P_A_WORKER_1);
  WORKER_(ANIM_P_A_WORKER_2);
  WORKER_(ANIM_P_A_WORKER_3);
  WORKER_(ANIM_P_A_WORKER_4);
  ws.workers_victim_incident = ANIM_P_A_WORKER_VICTIM;

  work_station_shift_anim_position(ws);
  return work_station_spawn(ws);
}

void work_station_spawn_all() {
  w = work_station_spawn_w();
  press = work_station_spawn_press();
  cave = work_station_spawn_cave();
  p_a = work_station_spawn_p_a();
}

void work_station_shift_anim_position(WorkStation ws) {

  animation_shift_position(ws.parts_idle[0], ws.position);
  animation_shift_position(ws.parts_idle[1], ws.position);
  animation_shift_position(ws.parts_idle[2], ws.position);
  animation_shift_position(ws.parts_idle[3], ws.position);
  animation_shift_position(ws.parts_incident[0], ws.position);
  animation_shift_position(ws.parts_incident[1], ws.position);
  animation_shift_position(ws.parts_incident[2], ws.position);
  animation_shift_position(ws.parts_incident[3], ws.position);

  animation_shift_position(ws.workers_idle[0], ws.position);
  animation_shift_position(ws.workers_idle[1], ws.position);
  animation_shift_position(ws.workers_idle[2], ws.position);
  animation_shift_position(ws.workers_idle[3], ws.position);
  animation_shift_position(ws.workers_incident[0], ws.position);
  animation_shift_position(ws.workers_incident[1], ws.position);
  animation_shift_position(ws.workers_incident[2], ws.position);
  animation_shift_position(ws.workers_incident[3], ws.position);

  animation_shift_position(ws.workers_victim_incident, ws.position);

}
void work_station_attach(WorkStation* ws, WorkStationPart wsp, u i) {
  u id = work_station_part_spawn(wsp);

  EntityRef ref = (EntityRef){
    .id = id,
    .index_hint = 0,
  };
  ws->parts[i] = ref;
}
u       work_station_spawn(WorkStation ws) {
  Entity e = {0};

  e.update_fn = work_station_update;
  e.draw_fn = work_station_draw;

  *e_as(WorkStation, &e) = ws;

  return entity_spawn(e);
}
void    work_station_draw(Entity* e) {
  WorkStation* ws = e_as(WorkStation, e);
  Input i = input_get();
  if (check_collision_point_rec(i.pos, ws->interaction_area)) {
    draw_rectangle_rec(ws->interaction_area, fade(BLUE, 0.2f));
  }
}
void    work_station_update(Entity* e) {
  WorkStation* ws = e_as(WorkStation, e);
  for (u i = 0; i < MAX_WORK_STATION_WORKERS; ++i) {
    Entity* e_w = entity_ref_get(&ws->workers[i]);
    if (e_w != NULL) {
      Worker* w = e_as(Worker, e_w);
      if (w->work_station != e->id) {
        // this worker no longer works here.
        ws->workers[i].id = 0;
      } else {
        bank_add_income( ws->income );
        if (ws->incident_ongoing_time_reset == 0.0f) {
          // prevent selection for immidiate incident (cave)
          w->selected = false;
        }
      }

    } else {
      // There is no worker. No income from here.
    }
  }

  if (!ws->incident_is_on) {
    timer_countdown_frame(&ws->incident_countdown);
    if (ws->incident_countdown < 0.0f) {
      // its bad to do != on float but it should pass
      if (ws->incident_ongoing_time_reset != 0.0f) {
        work_station_do_incident(ws);
      }
    }
  } else {
    timer_countdown_frame(&ws->incident_countdown);
    if (ws->incident_countdown < 0.0f) {
      ws->incident_is_on = false;
      work_station_do_idle(ws);
    }
  }

#include <stdio.h>
  work_station_stop_asking_for_workers(e);
  if (!ws->incident_is_on) {
    if( just_finished_selecting() ) {
      logi("finished selecting\n");
      Rectangle sel = get_selection();
      if ( check_collision_recs( sel, ws->interaction_area ) ) {
        if (sel.width * sel.height < 100.0f*100.0f) {
           logi("registered as ws interaction"); fflush(stdout);
          force_close_selection();
          work_station_ask_for_workers(e);
        }
      }
    }
  } else {
    if( just_finished_selecting() ) {
      logi("finished selecting, but incident is ongoing");
    }
  }
}
void work_station_stop_asking_for_workers(Entity* e) {
  if (asking_for_workers.id == e->id) {
    asking_for_workers.id = 0;
  }
}
void work_station_ask_for_workers(Entity* e) {
  asking_for_workers = entity_ref_make(e);

}
void work_station_do_incident(WorkStation* ws) {

  logi("doing_incident");
  ws->incident_countdown = ws->incident_ongoing_time_reset;

  Worker* victim = NULL;
  for (u i = 0; i < MAX_WORK_STATION_WORKERS; ++i) {
    Entity* e_w = entity_ref_get(&ws->workers[i]);
    if (e_w != NULL) {
      Worker* w = e_as(Worker, e_w);
      if (victim != NULL) {
        if ( random_bool() ) {
          victim = w;
        }
      } else {
        victim = w;
      }
      w->animation = animation_get(ws->workers_incident[i]);
      w->selected = false;
    } else {
      // There is no worker. No one to set animation on
    }
  }
  if (victim != NULL) {

    ws->incident_is_on = true;
    victim->animation = animation_get(ws->workers_victim_incident);
    say_help(victim);


    for (u i = 0; i < MAX_WORK_STATION_PARTS; ++i) {
      Entity* e_part = entity_ref_get(&ws->parts[i]);
      if (e_part != NULL) {
        WorkStationPart* wsp = e_as(WorkStationPart, e_part);
        wsp->animation = animation_get(ws->parts_incident[i]);
      }
    }

  } else {
    // skip incident, no one is working.
    logi("incident skipped");
    ws->incident_countdown = random_float_in_range(ws->min_incident_time, ws->max_incident_time);
  }


}
void work_station_do_idle    (WorkStation* ws) {
  logi("doing_idle");
  ws->incident_is_on = false;
  ws->incident_countdown = random_float_in_range(ws->min_incident_time, ws->max_incident_time);

  for (u i = 0; i < MAX_WORK_STATION_WORKERS; ++i) {
    Entity* e_w = entity_ref_get(&ws->workers[i]);
    if (e_w != NULL) {
      Worker* w = e_as(Worker, e_w);
      w->animation = animation_get(ws->workers_idle[i]);
    }
  }

  for (u i = 0; i < MAX_WORK_STATION_PARTS; ++i) {
    Entity* e_p = entity_ref_get(&ws->parts[i]);
    if (e_p != NULL) {
      WorkStationPart* wsp = e_as(WorkStationPart, e_p);
      wsp->animation = animation_get(ws->parts_idle[i]);
    }
  }
}


#endif   // WORK_STATION_IMPLEMENTATION
#endif   //_WORK_STATION_H_

