#ifdef    MONO_BUILD
#define ASSETS_IMPLEMENTATION
#endif // MONO_BUILD
#ifndef    _ASSETS_H_
#define    _ASSETS_H_
// assets interface

#include "rayinc.h"

void assets_init();
void assets_free();

typedef Texture Sprite;

typedef enum {
  SPRITE_BOT_BODY_STEEL,
  SPRITE_BOT_BODY_PATCHED,
  SPRITE_BOT_BODY_WOOD,
  SPRITE_WORK_STATION_PRESS_PART_TOP,
  SPRITE_WORK_STATION_PRESS_PART_PISTONS,
  SPRITE_WORK_STATION_PRESS_PART_PLATES,
  SPRITE_PARTICLE_ELECTRO_1,
  SPRITE_PARTICLE_ELECTRO_2,
  SPRITE_PARTICLE_ELECTRO_3,
  SPRITE_PARTICLE_FIRE_1,
  SPRITE_PARTICLE_FIRE_2,
  SPRITE_PARTICLE_FIRE_3,

  SPRITE_WORK_STATION_CAVE_PART_MAIN,
  SPRITE_WORK_STATION_P_A_BLACK_HOLE,
  SPRITE_WORK_STATION_P_A_ENERGY,
  SPRITE_WORK_STATION_P_A_WALLS,
  SPRITE_WORK_STATION_W_WINGS,
  SPRITE_WORK_STATION_W_BASE,
  SPRITE_EMPTY,

  SPRITE_COUNT,
} SpriteID;
typedef enum {
  FONT_IOSEVKA,
  FONT_COUNT,
} FontID;
typedef enum {
  SOUND_WHEE_1,
  SOUND_WHEE_2,
  SOUND_WHEE_3,
  SOUND_HELP_1,
  SOUND_HELP_2,
  SOUND_HELP_3,
  SOUND_OK_1,
  SOUND_OK_2,
  SOUND_OK_3,
  SOUND_GOT_IT_1,
  SOUND_GOT_IT_2,
  SOUND_GOT_IT_3,
  SOUND_COUNT,
} SoundID;

Sprite a_sprite(SpriteID id);
Font   a_font  (FontID id);
void a_sound_play (SoundID id);

#ifdef      ASSETS_IMPLEMENTATION
// assets implementation


#define MAKE_ASSET(TYPE) typedef struct s##TYPE##Info { TYPE asset; AssetInfo info; } TYPE##Info;
typedef struct sAssetInfo {
  bool has_info;
  const char* file;
  const char* title;
  const char* author;
  const char* link;
  const char* license;
  const char* license_link;
} AssetInfo;

MAKE_ASSET(Sprite);
MAKE_ASSET(Font);
MAKE_ASSET(Sound);

typedef struct sAssets {
  SpriteInfo sprites[SPRITE_COUNT];
  FontInfo   fonts[FONT_COUNT];
  SoundInfo   sounds[SOUND_COUNT];
} Assets;
Assets assets = {0};

void assets_init() {
#include "../assets/info.h"
  for (int i = 0; i < SPRITE_COUNT; ++i ) {
    assets.sprites[i].asset = load_texture(assets.sprites[i].info.file);
  }
  for (int i = 0; i < FONT_COUNT; ++i ) {
    assets.fonts[i].asset = load_font(assets.fonts[i].info.file);
  }
  for (int i = 0; i < SOUND_COUNT; ++i ) {
    assets.sounds[i].asset = load_sound(assets.sounds[i].info.file);
  }
}
void assets_free() {
  for (int i = 0; i < SPRITE_COUNT; ++i ) {
    unload_texture(assets.sprites[i].asset);
  }
  for (int i = 0; i < FONT_COUNT; ++i ) {
    unload_font(assets.fonts[i].asset);
  }
  for (int i = 0; i < SOUND_COUNT; ++i ) {
    unload_sound(assets.sounds[i].asset);
  }
  assets = (Assets){0};
}

Sprite a_sprite(SpriteID id) {
  return assets.sprites[id].asset;
}
Font   a_font  (FontID id) {
  return assets.fonts[id].asset;
}
void a_sound_play (SoundID id) {
  play_sound(assets.sounds[id].asset);
}

#endif   // ASSETS_IMPLEMENTATION
#endif   //_ASSETS_H_

