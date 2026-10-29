#ifdef    MONO_BUILD
#define FMATH_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _FMATH_H_
#define    _FMATH_H_
// fmath interface

float float_middle(float a, float b);
float float_lerp(float f1, float f2, float amount);

#ifdef      FMATH_IMPLEMENTATION
// fmath implementation


float float_middle(float a, float b) {
  return (a+b)/2;
}
float float_lerp(float f1, float f2, float amount) {
    float result = 0.0f;

    result = f1 + amount*(f2 - f1);

    return result;
}

#endif   // FMATH_IMPLEMENTATION
#endif   //_FMATH_H_

