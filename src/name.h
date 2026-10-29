#ifdef    MONO_BUILD
#define NAME_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _NAME_H_
#define    _NAME_H_
// name interface

#include "noutil.h"

typedef u NameID;

const char* name_get(NameID id);
u name_count();
// for consistency
#define NAME_COUNT name_count()


#ifdef      NAME_IMPLEMENTATION
// name implementation

const char* names[] = {
  "V1",
  "V2",
  "A-371",
  "A-890",
  "A-023",
  "B-369",
  "B-888",
  "B-022",
  "C-366",
  "C-885",
  "C-019",
  "Al",
  "Tic-Tok",
  "Metal",
  "Woodie",
  "Gnut",
  "Kay",
  "Jay",
  "Lay",
  "May",
  "Fay",
  "Bors",
  "Fur",
  "I-87",
  "I-68",
  "Fortune Mod",
  "Yod",
  "Grep",
  "Ls",
  "Cd",
  "Fd",
  "Tux",
  "Mow",
  "Muw",
  "X",
  "Y",
  "Z",
  "Adam",
  "Lizard",
  "Bob",
  "Bill",
  "Charlie",
  "Gretta",
  "Kyle",
  "Greg",
  "Ikarus",
  "Oswald",
  "Zintara",
  "Albina",
  "[REDACTED]",
};
u name_count() {
  return ARRAY_LEN(names);
}
const char* name_get(NameID id) {
  return names[id];
}


#endif   // NAME_IMPLEMENTATION
#endif   //_NAME_H_

