#ifdef    MONO_BUILD
#define ANIMATION_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _ANIMATION_H_
#define    _ANIMATION_H_
// animation interface

#include "noutil.h"
#include "transform2d.h"

typedef struct sAnimationActorState {
  Transform2D transform;
  bool do_action;
} AnimationActorState;

typedef struct sFrame {
  AnimationActorState state;
  float time_stamp;
} Frame;

typedef struct sFrameView {
  u count;
  Frame* items;
} FrameView;


typedef struct sAnimation {
  u current_frame;
  float current_time;
  FrameView frames;
} Animation;

typedef enum {
  ANIM_NULL,
  ANIM_PRESS_PART_PISTON_IDLE,
  ANIM_PRESS_PART_PISTON_INCIDENT,
  ANIM_PRESS_PART_PLATES_IDLE,
  ANIM_PRESS_PART_PLATES_INCIDENT,
  ANIM_PRESS_PART_TOP_IDLE,
  ANIM_PRESS_PART_TOP_INCIDENT,
  ANIM_PRESS_WORKER_1_IDLE,
  ANIM_PRESS_WORKER_2_IDLE,
  ANIM_PRESS_WORKER_3_IDLE,
  ANIM_PRESS_WORKER_4_IDLE,
  ANIM_PRESS_WORKER_1_INCIDENT,
  ANIM_PRESS_WORKER_2_INCIDENT,
  ANIM_PRESS_WORKER_3_INCIDENT,
  ANIM_PRESS_WORKER_4_INCIDENT,
  ANIM_PRESS_WORKER_VICTIM,

  ANIM_CAVE_PART_BASE,
  ANIM_CAVE_WORKER_1_IDLE,
  ANIM_CAVE_WORKER_2_IDLE,
  ANIM_CAVE_WORKER_3_IDLE,
  ANIM_CAVE_WORKER_4_IDLE,

  ANIM_W_BASE_IDLE,
  ANIM_W_BASE_INCIDENT,
  ANIM_W_WINGS_IDLE,
  ANIM_W_WINGS_INCIDENT,
  ANIM_W_WORKER_1_IDLE,
  ANIM_W_WORKER_2_IDLE,
  ANIM_W_WORKER_3_IDLE,
  ANIM_W_WORKER_4_IDLE,
  ANIM_W_WORKER_1_INCIDENT,
  ANIM_W_WORKER_2_INCIDENT,
  ANIM_W_WORKER_3_INCIDENT,
  ANIM_W_WORKER_4_INCIDENT,
  ANIM_W_WORKER_VICTIM,

  ANIM_P_A_ENERGY_IDLE,
  ANIM_P_A_ENERGY_INCIDENT,
  ANIM_P_A_BLACK_HOLE_IDLE,
  ANIM_P_A_BLACK_HOLE_INCIDENT,
  ANIM_P_A_WALLS_IDLE,
  ANIM_P_A_WALLS_INCIDENT,
  ANIM_P_A_WORKER_1_IDLE,
  ANIM_P_A_WORKER_2_IDLE,
  ANIM_P_A_WORKER_3_IDLE,
  ANIM_P_A_WORKER_4_IDLE,
  ANIM_P_A_WORKER_1_INCIDENT,
  ANIM_P_A_WORKER_2_INCIDENT,
  ANIM_P_A_WORKER_3_INCIDENT,
  ANIM_P_A_WORKER_4_INCIDENT,
  ANIM_P_A_WORKER_VICTIM,

  ANIM_COUNT,
} AnimationID;

void animations_init();
AnimationActorState aas_interpolate_linear(AnimationActorState aas0, AnimationActorState aas1, float progress);
float animation_get_frame_progress(Animation* animation);
bool animation_apply(Animation* animation, Transform2D* transform); // returns true if entity needs to be deleted
Animation animation_get(AnimationID anim);
void animation_shift_position(AnimationID anim, Vector2 position);

#ifdef      ANIMATION_IMPLEMENTATION
// animation implementation

#include <assert.h>
#include "timer.h"
#include "logging.h"
typedef struct sAnimationIndex {
  u start;
  u count;
} AnimationIndex;

NOU(Frame);

DA_(Frame) animation_vault = {0};
AnimationIndex animation_index[ANIM_COUNT];

Frame base_frame = (Frame){
  .state = (AnimationActorState) {
    .transform = (Transform2D) {
      .position = (Vector2){0.0f, 0.0f},
      .scale    = (Vector2){1.0f, 1.0f},
      .rotation = 0.0f,
    },
    .do_action = false,
  },
  .time_stamp = 0.0f,
};
void frame_reset_base_frame() {
  base_frame = (Frame){
    .state = (AnimationActorState) {
      .transform = (Transform2D) {
        .position = (Vector2){0.0f, 0.0f},
        .scale    = (Vector2){1.0f, 1.0f},
        .rotation = 0.0f,
      },
      .do_action = false,
    },
    .time_stamp = 0.0f,
  };
}
Frame frame_none(float dt) {
  base_frame.time_stamp += dt;
  Frame f = base_frame;

  return f;
}
Frame frame_all(Transform2D transform, float dt) {
  Frame f = frame_none(dt);

  f.state.transform = transform;

  base_frame = f;
  return f;
}
Frame frame_pos_n_rot( Vector2 position, float rotation, float dt ) {
  Frame f = frame_none(dt);

  f.state.transform.position = position;
  f.state.transform.rotation = rotation;

  base_frame = f;
  return f;
}
Frame frame_pos_n_scale( Vector2 position, Vector2 scale, float dt ) {
  Frame f = frame_none(dt);

  f.state.transform.position = position;
  f.state.transform.scale = scale;

  base_frame = f;
  return f;
}
Frame frame_pos( Vector2 position, float dt ) {
  Frame f = frame_none(dt);

  f.state.transform.position = position;

  base_frame = f;
  return f;
}
Frame frame_act( float dt ) {
  Frame f = frame_none(dt);

  f.state.do_action = true;

  base_frame = f;
  return f;
}
Frame frame_stop_act( float dt ) {
  Frame f = frame_none(dt);

  f.state.do_action = false;

  base_frame = f;
  return f;
}
Frame frame_rot( float rotation, float dt ) {
  Frame f = frame_none(dt);

  f.state.transform.rotation = rotation;

  base_frame = f;
  return f;
}

void animation_shift_position(AnimationID anim, Vector2 position) {
  Animation animation = animation_get(anim);
  for (u i = 0; i < animation.frames.count; ++i) {
    Frame* current = animation.frames.items+i;
    current->state.transform.position = vector2_add(current->state.transform.position, position);
  }
}

void animations_init() {


#define START_ANIM(ANIM) animation_index[ANIM].start = animation_vault.count; frame_reset_base_frame()
#define   END_ANIM(ANIM) animation_index[ANIM].count = animation_vault.count - animation_index[ANIM].start

  START_ANIM(ANIM_NULL);

  frame_reset_base_frame();
  da_push(&animation_vault,
    frame_all(
      (Transform2D) {
        .position = (Vector2){-100.0f, -100.0f},
        .scale    = (Vector2){1.0f, 1.0f},
        .rotation = -45.0f,
      },
      1.0f)
  );
  da_push(&animation_vault,
      frame_pos_n_rot( (Vector2){100.0f, 0.0f}, 60.0f, 1.0f )
  );
  da_push(&animation_vault,
    frame_pos( (Vector2){100.0f, -100.0f}, 1.0f )
  );
  da_push(&animation_vault,
    frame_rot( 180.0f, 1.0f )
  );

  END_ANIM(ANIM_NULL);

  START_ANIM(ANIM_PRESS_PART_PISTON_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -200.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -120.0f}, 1.0f ) );
  END_ANIM  (ANIM_PRESS_PART_PISTON_IDLE);

  START_ANIM(ANIM_PRESS_PART_PISTON_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -200.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -200.0f}, 4.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -180.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -140.0f}, 4.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -120.0f}, 0.1f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -120.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -200.0f}, 1.0f ) );
  END_ANIM  (ANIM_PRESS_PART_PISTON_INCIDENT);

  START_ANIM(ANIM_PRESS_PART_PLATES_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 0.0f}, 2.0f ) );
  END_ANIM  (ANIM_PRESS_PART_PLATES_IDLE);

  START_ANIM(ANIM_PRESS_PART_PLATES_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 0.0f}, 0.01f ) );
    da_push( &animation_vault, frame_act(0.01f) );
    da_push( &animation_vault, frame_stop_act(4.7f) );
    da_push( &animation_vault, frame_stop_act(10.0f) );
  END_ANIM  (ANIM_PRESS_PART_PLATES_INCIDENT);

  START_ANIM(ANIM_PRESS_PART_TOP_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -250.0f}, 2.0f ) );
  END_ANIM  (ANIM_PRESS_PART_TOP_IDLE);

  START_ANIM(ANIM_PRESS_PART_TOP_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -250.0f}, 2.0f ) );
  END_ANIM  (ANIM_PRESS_PART_TOP_INCIDENT);


  START_ANIM(ANIM_PRESS_WORKER_1_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){-200.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-210.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-190.0f, 150.0f}, 2.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_1_IDLE);


  START_ANIM(ANIM_PRESS_WORKER_2_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){-100.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){- 90.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-110.0f, 150.0f}, 2.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_2_IDLE);


  START_ANIM(ANIM_PRESS_WORKER_3_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-10.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){ 10.0f, 150.0f}, 2.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_3_IDLE);

  START_ANIM(ANIM_PRESS_WORKER_4_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){100.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){90.0f, 150.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){110.0f, 150.0f}, 2.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_4_IDLE);



  START_ANIM(ANIM_PRESS_WORKER_1_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){200.0f, 150.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-200.0f, 150.0f}, 1.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_1_INCIDENT);


  START_ANIM(ANIM_PRESS_WORKER_2_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){-200.0f, 150.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){200.0f, 150.0f}, 1.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_2_INCIDENT);


  START_ANIM(ANIM_PRESS_WORKER_3_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){200.0f, 150.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-200.0f, 150.0f}, 1.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_3_INCIDENT);

  START_ANIM(ANIM_PRESS_WORKER_4_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){200.0f, 150.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-200.0f, 150.0f}, 1.0f ) );
  END_ANIM  (ANIM_PRESS_WORKER_4_INCIDENT);


  START_ANIM(ANIM_PRESS_WORKER_VICTIM);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 40.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -40.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, -40.0f}, 3.5f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, -20.0f}, (Vector2){2.0f, 0.33f}, 4.0f ) );
    da_push( &animation_vault, frame_act(0.01f) );


   // da_push( &animation_vault, frame_pos( (Vector2){0.0f, -200.0f}, 1.0f ) );
   // da_push( &animation_vault, frame_pos( (Vector2){0.0f, -200.0f}, 4.0f ) );
   // da_push( &animation_vault, frame_pos( (Vector2){0.0f, -180.0f}, 0.5f ) );
   // da_push( &animation_vault, frame_pos( (Vector2){0.0f, -140.0f}, 4.0f ) );
   // da_push( &animation_vault, frame_pos( (Vector2){0.0f, -120.0f}, 0.1f ) );
   // da_push( &animation_vault, frame_pos( (Vector2){0.0f, -120.0f}, 1.0f ) );
   // da_push( &animation_vault, frame_pos( (Vector2){0.0f, -200.0f}, 1.0f ) );

  END_ANIM  (ANIM_PRESS_WORKER_VICTIM);



  START_ANIM(ANIM_CAVE_PART_BASE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 0.0f}, 1.0f ) );
  END_ANIM  (ANIM_CAVE_PART_BASE);



  START_ANIM(ANIM_CAVE_WORKER_1_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 125.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 125.0f}, (Vector2){0.33f, 0.33f}, 0.5f ) );
    da_push( &animation_vault, frame_act(0.01f) );
    da_push( &animation_vault, frame_act(0.01f) );
  END_ANIM  (ANIM_CAVE_WORKER_1_IDLE);

  START_ANIM(ANIM_CAVE_WORKER_2_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 125.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 125.0f}, (Vector2){0.33f, 0.33f}, 0.5f ) );
    da_push( &animation_vault, frame_act(0.01f) );
    da_push( &animation_vault, frame_act(0.01f) );
  END_ANIM  (ANIM_CAVE_WORKER_2_IDLE);

  START_ANIM(ANIM_CAVE_WORKER_3_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 125.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 125.0f}, (Vector2){0.33f, 0.33f}, 0.5f ) );
    da_push( &animation_vault, frame_act(0.01f) );
    da_push( &animation_vault, frame_act(0.01f) );
  END_ANIM  (ANIM_CAVE_WORKER_3_IDLE);


  START_ANIM(ANIM_CAVE_WORKER_4_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 125.0f}, 1.0f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 125.0f}, (Vector2){0.33f, 0.33f}, 0.5f ) );
    da_push( &animation_vault, frame_act(0.01f) );
    da_push( &animation_vault, frame_act(0.01f) );
  END_ANIM  (ANIM_CAVE_WORKER_4_IDLE);


  START_ANIM(ANIM_W_BASE_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 0.0f}, 1.0f ) );
  END_ANIM  (ANIM_W_BASE_IDLE);

  START_ANIM(ANIM_W_BASE_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 0.0f}, 0.0f ) );
    da_push( &animation_vault, frame_act(0.1f) );
    da_push( &animation_vault, frame_act(0.1f) );
    da_push( &animation_vault, frame_stop_act(1.0f) );
    da_push( &animation_vault, frame_stop_act(10.0f) );
    da_push( &animation_vault, frame_stop_act(10.0f) );
  END_ANIM  (ANIM_W_BASE_INCIDENT);

  START_ANIM(ANIM_W_WINGS_IDLE);
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 1.0f ) );
  END_ANIM(ANIM_W_WINGS_IDLE);

  START_ANIM(ANIM_W_WINGS_INCIDENT);
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 3.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 1.0f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.75f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.75f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.5f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.5f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.5f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.25f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.25f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.25f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );

    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.5f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.75f ) );

  END_ANIM(ANIM_W_WINGS_INCIDENT);

  float a = 0.0f;
  a = -300.0f;
  START_ANIM(ANIM_W_WORKER_1_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){a + 50.0f,  350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + -50.0f, 350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_1_IDLE);

  a  = 300.0f;
  START_ANIM(ANIM_W_WORKER_2_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + -50.0f, 350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 50.0f,  350.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_2_IDLE);

  a  = 100.0f;
  START_ANIM(ANIM_W_WORKER_3_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 50.0f,  350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + -50.0f, 350.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_3_IDLE);

  a  = -100.0f;
  START_ANIM(ANIM_W_WORKER_4_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){a + -50.0f, 350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 50.0f,  350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_4_IDLE);



  a = -300.0f;
  START_ANIM(ANIM_W_WORKER_1_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,  350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f, 350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_1_INCIDENT);

  a  = 300.0f;
  START_ANIM(ANIM_W_WORKER_2_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f, 350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,  350.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_2_INCIDENT);

  a  = 100.0f;
  START_ANIM(ANIM_W_WORKER_3_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,  350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f, 350.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_3_INCIDENT);

  a  = -100.0f;
  START_ANIM(ANIM_W_WORKER_4_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f, 350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,  350.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){a + 0.0f,    300.0f}, 0.5f ) );
  END_ANIM(ANIM_W_WORKER_4_INCIDENT);




  START_ANIM(ANIM_W_WORKER_VICTIM);
    da_push( &animation_vault, frame_pos_n_rot( (Vector2){0.0f, 150.0f}, 0.0f, 1.0f ) );
    da_push( &animation_vault, frame_pos_n_rot( (Vector2){0.0f, 0.0f}, 360.0f, 1.0f ) );
    da_push( &animation_vault, frame_pos_n_rot( (Vector2){0.0f, 0.0f}, 360.0f, 1.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 1.0f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.75f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.75f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.5f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.5f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.5f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.25f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.25f ) );
    da_push( &animation_vault, frame_act( 0.01f ) );
    da_push( &animation_vault, frame_act( 0.01f ) );
  END_ANIM(ANIM_W_WORKER_VICTIM);




  
  
  START_ANIM(ANIM_P_A_ENERGY_IDLE);
    da_push( &animation_vault, frame_rot( 45.0f, 1.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 1.0f ) );
    da_push( &animation_vault, frame_rot( -45.0f, 1.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 1.0f ) );
  END_ANIM(ANIM_P_A_ENERGY_IDLE);
  
  
  START_ANIM(ANIM_P_A_ENERGY_INCIDENT);
    da_push( &animation_vault, frame_none( 1.0f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 0.0f}, (Vector2){0.00f, 0.00f}, 0.1f ) );
    da_push( &animation_vault, frame_none( 10.0f ) );
  END_ANIM(ANIM_P_A_ENERGY_INCIDENT);
  
  
  START_ANIM(ANIM_P_A_BLACK_HOLE_IDLE);
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 0.0f}, (Vector2){0.01f, 0.01f}, 0.01f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 0.0f}, (Vector2){0.05f, 0.05f}, 0.1f ) );
  END_ANIM(ANIM_P_A_BLACK_HOLE_IDLE);
  
  
  START_ANIM(ANIM_P_A_BLACK_HOLE_INCIDENT);
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 0.0f}, (Vector2){0.05f, 0.05f}, 0.5f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 0.0f}, (Vector2){1.00f, 1.00f}, 1.0f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 3.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 3.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.0f ) );
    da_push( &animation_vault, frame_rot( 360.0f, 3.0f ) );
    da_push( &animation_vault, frame_rot( 0.0f, 0.0f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 0.0f}, (Vector2){0.05f, 0.05f}, 0.5f ) );
  END_ANIM(ANIM_P_A_BLACK_HOLE_INCIDENT);
  //logi("time: %f", base_frame.time_stamp);
  
  
  START_ANIM(ANIM_P_A_WALLS_IDLE);
    da_push( &animation_vault, frame_none( 1.0f ) );
  END_ANIM(ANIM_P_A_WALLS_IDLE);
  
  
  START_ANIM(ANIM_P_A_WALLS_INCIDENT);
    da_push( &animation_vault, frame_none( 1.0f ) );
  END_ANIM(ANIM_P_A_WALLS_INCIDENT);
  
  a = 0.0f;
  float x = 0.0f;
  float y = 0.0f;

  x = -150.0f; y = 300.0f;
  START_ANIM(ANIM_P_A_WORKER_1_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y +  10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +  10.0f, y +   0.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y + -10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x + -10.0f, y +   0.0f}, 0.5f ) );
  END_ANIM(ANIM_P_A_WORKER_1_IDLE);
  
  
  x = -50.0f; y = 300.0f;
  START_ANIM(ANIM_P_A_WORKER_2_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y +  10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +  10.0f, y +   0.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y + -10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x + -10.0f, y +   0.0f}, 0.5f ) );
  END_ANIM(ANIM_P_A_WORKER_2_IDLE);
  
  
  x = 50.0f; y = 300.0f;
  START_ANIM(ANIM_P_A_WORKER_3_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y +  10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +  10.0f, y +   0.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y + -10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x + -10.0f, y +   0.0f}, 0.5f ) );
  END_ANIM(ANIM_P_A_WORKER_3_IDLE);
  
  
  x = 150.0f; y = 300.0f;
  START_ANIM(ANIM_P_A_WORKER_4_IDLE);
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y +  10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +  10.0f, y +   0.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x +   0.0f, y + -10.0f}, 0.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){x + -10.0f, y +   0.0f}, 0.5f ) );
  END_ANIM(ANIM_P_A_WORKER_4_IDLE);
  
  
  START_ANIM(ANIM_P_A_WORKER_1_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 300.0f}, 0.1f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-30.0f, 330.0f}, 3.4f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 200.0f}, 3.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-30.0f, 330.0f}, 4.4f ) );
  END_ANIM(ANIM_P_A_WORKER_1_INCIDENT);
  
  
  START_ANIM(ANIM_P_A_WORKER_2_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 300.0f}, 0.1f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-10.0f, 330.0f}, 3.4f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 200.0f}, 3.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){-10.0f, 330.0f}, 4.4f ) );
  END_ANIM(ANIM_P_A_WORKER_2_INCIDENT);
  
  
  START_ANIM(ANIM_P_A_WORKER_3_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 300.0f}, 0.1f ) );
    da_push( &animation_vault, frame_pos( (Vector2){10.0f, 330.0f}, 3.4f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 200.0f}, 3.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){10.0f, 330.0f}, 4.4f ) );
  END_ANIM(ANIM_P_A_WORKER_3_INCIDENT);
  
  
  START_ANIM(ANIM_P_A_WORKER_4_INCIDENT);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 300.0f}, 0.1f ) );
    da_push( &animation_vault, frame_pos( (Vector2){30.0f, 330.0f}, 3.4f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 200.0f}, 3.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){30.0f, 330.0f}, 4.5f ) );
  END_ANIM(ANIM_P_A_WORKER_4_INCIDENT);
  
  
  START_ANIM(ANIM_P_A_WORKER_VICTIM);
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 300.0f}, 0.1f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 330.0f}, 3.4f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 200.0f}, 3.5f ) );
    da_push( &animation_vault, frame_pos( (Vector2){0.0f, 0.0f}, 2.0f ) );
    da_push( &animation_vault, frame_pos_n_scale( (Vector2){0.0f, 0.0f}, (Vector2){0.05f, 0.05f}, 1.8f ) );
    da_push( &animation_vault, frame_act( 0.1f ) );
    da_push( &animation_vault, frame_act( 0.1f ) );
  END_ANIM(ANIM_P_A_WORKER_VICTIM);


}
Animation animation_get(AnimationID anim) {
  if (anim == ANIM_NULL) {
    return (Animation){0};
  }
  AnimationIndex index = animation_index[anim];
  return (Animation){
    .current_frame = 0,
    .current_time = 0.0f,
    .frames = (FrameView){
      .count = index.count,
      .items = animation_vault.items + index.start,
    }
  };
}

AnimationActorState aas_interpolate_linear(AnimationActorState aas0, AnimationActorState aas1, float progress) {
  AnimationActorState aas01 = {0};
  aas01.transform = transform_interpolate_linear(aas0.transform, aas1.transform, progress);
  aas01.do_action = aas0.do_action;
  return aas01;
}
float frames_get_progress(Frame frame0, Frame frame1, float t) {
  float t0 = frame0.time_stamp;
  float t1 = frame1.time_stamp;

  float duration = t1 - t0;
  float elapsed  = t  - t0;

  return elapsed/duration;
}
Frame animation_get_frame_current(Animation animation) {
    return animation.frames.items[animation.current_frame];
}
Frame animation_get_frame_next(Animation animation) {
  if (animation.frames.count == 0) {
    return (Frame){0}; // invalid tbh
  }
  if (animation.frames.count == 1) {
    // there is only one frame so next is current
    return animation.frames.items[animation.current_frame];
  }
  if (animation.current_frame < animation.frames.count - 1) {
    return animation.frames.items[animation.current_frame+1];
  }
  if (animation.current_frame == animation.frames.count - 1) {
    // loop at the end
    return animation.frames.items[0];
  }
  // how?
  return animation.frames.items[0];
}
float animation_get_frame_progress(Animation* animation) {
  if (animation->frames.count <= 1) {
    return 0.0f;
  }
  if (animation->current_frame < animation->frames.count - 1) {
    Frame frame0 = animation_get_frame_current(*animation);
    Frame frame1 = animation_get_frame_next(*animation);
    if (animation->current_time >= frame1.time_stamp) {
      animation->current_frame += 1;
      return animation_get_frame_progress(animation);
    }
    return frames_get_progress(frame0, frame1, animation->current_time);
  } else {
    Frame frame0 = animation_get_frame_current(*animation);
    Frame frame1 = animation_get_frame_next(*animation);
    if (animation->current_time > frame0.time_stamp) {
      // loop back
      animation->current_time = 0.0f;
    }
    if (animation->current_time >= frame1.time_stamp) {
      animation->current_frame = 0;
      return animation_get_frame_progress(animation);
    }
    frame0.time_stamp = 0.0f;
    return frames_get_progress(frame0, frame1, animation->current_time);
  }
}
bool animation_apply(Animation* animation, Transform2D* transform) {
  if (animation->frames.count > 0) {
    timer_countup_frame(&animation->current_time);
    float progress = animation_get_frame_progress(animation);
    if (progress < 0.0f) {
      // this means we are transitioning into this animation.
      Transform2D frame1_transform = animation_get_frame_current(*animation).state.transform;
      *transform = transform_interpolate_linear(*transform, frame1_transform, 0.1f);
      return false;
    }
    AnimationActorState aas = aas_interpolate_linear (
        animation_get_frame_current(*animation).state,
        animation_get_frame_next(*animation).state,
        progress
    );
    *transform = aas.transform;
    return aas.do_action;
  } else {
    return false;
  }
}


#endif   // ANIMATION_IMPLEMENTATION
#endif   //_ANIMATION_H_

