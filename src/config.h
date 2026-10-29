#ifdef    MONO_BUILD
#define CONFIG_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _CONFIG_H_
#define    _CONFIG_H_
// config interface

#include "noutil.h"

bool config_bool(const char* cfg);
void config_bool_toggle(const char* cfg);

#define SCREEN_X_ 720
#define SCREEN_Y_ 720

#ifdef      CONFIG_IMPLEMENTATION
// config implementation

typedef struct sKeyBool {
  const char* key;
  bool val;
} KeyBool;

KeyBool config[] = {
  (KeyBool){"debug", false},
  (KeyBool){NULL, false},
};

bool config_bool(const char* cfg) {
  for (u i = 0; config[i].key != NULL; ++i) {
    if ( strcmp(cfg, config[i].key)==0 ) {
      return config[i].val;
    }
  }
  return false;
}
void config_bool_toggle(const char* cfg) {
  for (u i = 0; config[i].key != NULL; ++i) {
    if ( strcmp(cfg, config[i].key)==0 ) {
      config[i].val = !config[i].val;
    }
  }
}

#endif   // CONFIG_IMPLEMENTATION
#endif   //_CONFIG_H_

