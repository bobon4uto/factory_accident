#ifdef    MONO_BUILD
#define TIMER_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _TIMER_H_
#define    _TIMER_H_
// timer interface

void timer_countdown_frame(float* t);
void timer_countup_frame(float* t);

#ifdef      TIMER_IMPLEMENTATION
// timer implementation

void timer_countdown_frame(float* t) {
  *t -= 1.0f/60.0f;
}
void timer_countup_frame(float* t) {
  *t += 1.0f/60.0f;
}

#endif   // TIMER_IMPLEMENTATION
#endif   //_TIMER_H_

