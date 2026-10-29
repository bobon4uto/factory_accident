#ifdef    MONO_BUILD
#define ENTITY_IMPLEMENTATION
#endif // MONO_BUILD
#ifndef    _ENTITY_H_
#define    _ENTITY_H_
// entity interface

#include "noutil.h"


typedef struct sEntityRef {
  u id;
  u index_hint;
} EntityRef;

typedef struct sEntity Entity;
typedef void(*EntityFn)(Entity*);

#define MAX_ENTITY_SIZE 256

typedef struct sEntity {
  u id;
  EntityFn update_fn;
  EntityFn draw_fn;
  EntityFn free_fn;

  char data[MAX_ENTITY_SIZE];

} Entity;
#define e_as(SUB_TYPE, e) ((SUB_TYPE*)(void*)(e)->data)

u         entity_spawn(Entity e);                   // Entitys must not be created any other way. Returns id which is only valid on next iteration.
Entity*   entity_spawn_now(Entity e);               // DO NOT use
bool      entity_delete_now(Entity* e);             // free data and delete entity. DO NOT use inside entity functions.
                                                    // use entity_delete instead
bool      entity_delete_now_by_ref(EntityRef* ref);
void      entity_delete(Entity* e);                 // schedule a delition. Will happen after all entitys done their update
void      entity_delete_by_ref(EntityRef* ref);
Entity*   entity_ref_get(EntityRef* ref);           // if not found returns NULL
EntityRef entity_ref_make(Entity* e);               // Its not always possible to get a pointer out of ref
                                                    // but its always possible to make ref out of a pointer.

bool      entity_ref_fix(EntityRef* ref);           // returns false if ref invalid, fixes index_hint if its wrong.

void entity_update(Entity* e);
void entity_draw  (Entity* e);
void entity_free  (Entity* e);

void entitys_update();
void entitys_draw  ();
void entitys_free();

#ifdef      ENTITY_IMPLEMENTATION
// entity implementation

#include <assert.h>

NOU(Entity);
DA_(Entity) world = {0};
u glob_id = 1;  // 0 is reserved for deletion schedule

static DA_(Entity) spawn_queue = {0};
u         entity_spawn(Entity e) {
  e.id = glob_id++;
  da_push(&spawn_queue, e);
  return e.id;
}
Entity*    entity_spawn_now(Entity e) {
  da_push(&world, e);
  return da_last_ptr(&world);
}

bool entity_delete_now(Entity* e) {
  EntityRef ref = entity_ref_make(e);
  return entity_delete_now_by_ref(&ref);
}
bool    entity_delete_now_by_ref(EntityRef* ref) {
  if (entity_ref_fix(ref)) {
    assert(ref->id == 0);
    assert(ref->index_hint < world.count);
    // right after a fix index_hint is correct.
    
    entity_free(entity_ref_get(ref));
    da_remove_at(&world, ref->index_hint);
    return true;
  } else {
    return false;
    // invalid ref, entity might be already deleted?
  }
}
void      entity_delete(Entity* e) {
  e->id = 0;
}
void      entity_delete_by_ref(EntityRef* ref) {
  Entity* e = entity_ref_get(ref);
  if (e!=NULL) {
    entity_delete(e);
  }
}




Entity*   entity_ref_get(EntityRef* ref) {
  if (entity_ref_fix(ref)) {
    return da_i_ptr(&world, ref->index_hint);
  }
  return NULL;
}
EntityRef entity_ref_make(Entity* e) {
  EntityRef ref = {0};
  ref.id = e->id;
  ref.index_hint = (e - world.items);
  return ref;
}
bool entity_ref_fix_no_bound(EntityRef* ref) {
  // ref->index_hint is assumed to be inside world
  for (u i = ref->index_hint; i > 0; i--) {
    // It is more likely that entity got shifted left because
    // some other entity was destroyed, so we check into that direction
    if (world.items[i].id == ref->id) {
      ref->index_hint = i;
      return true;
    }
  }

  if (world.items[0].id == ref->id) {
    ref->index_hint = 0;
    return true;
  }


  for (u i = ref->index_hint; i < world.count; i++) {
    // In current code there are actually NO entitys that spawn into
    // the front of the world, so this code is useless right now.
    if (world.items[i].id == ref->id) {
      ref->index_hint = i;
      return true;
    }
  }

  return false;
}
bool      entity_ref_fix(EntityRef* ref) {
  bool in_bounds           = ref->index_hint < world.count;
  bool mismatched_id       = true;
  if (in_bounds) {
    mismatched_id = ref->id != world.items[ref->index_hint].id;
  }
  if ( mismatched_id ) {
    // different id, position shifted probably, need to find new pos.
    return  entity_ref_fix_no_bound(ref);
  }
  // all good
  return true;
}


void entity_update(Entity* e) {
  if (e->update_fn != NULL) {
    e->update_fn(e);
  }
}
void entity_draw  (Entity* e) {
  if (e->draw_fn != NULL) {
    e->draw_fn(e);
  }
}
void entity_free  (Entity* e) {
  if (e->free_fn != NULL) {
    e->free_fn(e);
  }
}

DA_MAKE(EntityRef);

static DA_(EntityRef) deletion_queue = {0};
void entitys_update() {
  for (u i = 0; i < world.count; ++i) {
    Entity* current = da_i_ptr(&world, i);
    entity_update( current ); // thew world is not allowed to allocate memory during this, that is why queues are in place.
    if (current->id == 0) {
      da_push(&deletion_queue, entity_ref_make(current) );
    }
  }
  // maybe i should introduce foreach_reverse macro?
  //
  // Going in reverse since it prevents moving other entitys that we have to delete.
  // I considered making a bool flag for deletion, but this project is too small to have big problems with deletion.
  //
  // Note that although EntityRef will still work its for wrong reasons: it accepts id = 0  but it expects id to be unique ( and it is not )
  // But it will just get the first entity that has that id, and if we go in forward direction it will always be the closest to the start.
  // And it we go from the end if it encounters mismatch it will start from start and ruin others so they will be mismatch.
  // (reverse direction does not have that problem)
  //
  // In the end i used ref although it wasnt even the immidiate problem
  while (deletion_queue.count > 0) {
    EntityRef current = da_pop(&deletion_queue);
    entity_delete_now_by_ref(&current);
  }
  da_pushb(&world, spawn_queue.items, spawn_queue.count);
  spawn_queue.count = 0;

}
void entitys_draw  () {
  for (u i = 0; i < world.count; ++i ) {
     entity_draw( da_i_ptr(&world, i) );
  }
  // WHAT??? It reallocates here???
  // foreach (Entity in world) {
  //   (void)item;
  //   entity_draw(current);
  // }
}
void entitys_free() {
  foreach(Entity in world) {
    (void)item;
    entity_free(current);
  }
  world.count = 0;
}


#endif   // ENTITY_IMPLEMENTATION
#endif   //_ENTITY_H_

