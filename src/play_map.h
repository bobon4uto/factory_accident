#ifdef    MONO_BUILD
#define PLAY_MAP_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _PLAY_MAP_H_
#define    _PLAY_MAP_H_
// play_map interface

void map_spawn();

#ifdef      PLAY_MAP_IMPLEMENTATION
// play_map implementation

#include "entity.h"
#include "random.h"
#include "worker.h"
#include "work_station.h"
#include "name.h"
#include "transform2d.h"
#include "animation.h"

void map_spawn() {

  work_station_spawn_all();



  Worker w1 = worker_base();
  w1.transform.position =  (Vector2){-100,100};
  Worker w2 = worker_base();
  w2.transform.position =  (Vector2){-100,-20};
  Worker w3 = worker_base();
  w3.transform.position =  (Vector2){100,100};
  Worker w4 = worker_base();
  w4.transform.position =  (Vector2){100,-20};
  worker_spawn(w1);
  worker_spawn(w2);
  worker_spawn(w3);
  worker_spawn(w4);

}


#endif   // PLAY_MAP_IMPLEMENTATION
#endif   //_PLAY_MAP_H_

