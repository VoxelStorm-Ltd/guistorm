#if defined(GUISTORM_ROUND_NEAREST_OUT) || defined(GUISTORM_ROUND_NEAREST_ALL)
  #if defined(GUISTORM_ROUND_NEARBYINT)
    #pragma message "GUIStorm rounding mode: std::nearbyint"
    #define GUISTORM_ROUND(x) std::nearbyint(x)
  #elif defined(GUISTORM_ROUND_RINT)
    #pragma message "GUIStorm rounding mode: std::rint"
    #define GUISTORM_ROUND(x) std::rint(x)
  #elif defined(GUISTORM_ROUND_ROUND)
    #pragma message "GUIStorm rounding mode: std::round"
    #define GUISTORM_ROUND(x) std::round(x)
  #elif defined(GUISTORM_ROUND_CAST)
    #pragma message "GUIStorm rounding mode: cast"
    #define GUISTORM_ROUND(x) static_cast<float>(static_cast<int>(x))
    //#define GUISTORM_ROUND(x) (float)((int)x)
  #elif defined(GUISTORM_ROUND_REAL2INT)
    #pragma message "GUIStorm rounding mode: REAL2INT"
    inline int32_t real2int(double val) {
      constexpr double double2fixmagic = 68719476736.0 * 1.5;                   // 2^36 * 1.5, (52-_shiftamt=36) uses limited precision to floor
      val = val + double2fixmagic;
      #if BigEndian_
        #define iman_ 1
      #else
        #define iman_ 0
      #endif //BigEndian_
      return ((int32_t*)&val)[iman_] >> 16;                                     // 16.16 fixed point representation
    }
    inline int32_t real2int(float val) {
      return real2int((double)val);
    }
    #define GUISTORM_ROUND(x) real2int(x)
  #elif defined(GUISTORM_ROUND_FISTP)
    #pragma message "GUIStorm rounding mode: FISTP"
    inline double fistp(double val) {
      int out;
      asm("fld val");
      asm("fistp out");
      return out;
    }
    inline float fistp(float val) {
      int out;
      asm("fld val");
      asm("fistp out");
      return out;
    }
    #define GUISTORM_ROUND(x) fistp(x)
  #elif defined(GUISTORM_ROUND_FISTP2)
    #pragma message "GUIStorm rounding mode: FISTP2"
    inline double fistp2(double val) {
      int out;
      __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (val) : "st");
      return static_cast<double>(out);
    }
    inline float fistp2(float val) {
      int out;
      __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (val) : "st");
      return static_cast<float>(out);
    }
    #define GUISTORM_ROUND(x) fistp2(x)
  #elif defined(GUISTORM_ROUND_STOREINT)
    #pragma message "GUIStorm rounding mode: storeint"
    // just do nothing in this mode
    #define GUISTORM_ROUND(x) x
  #else
    #error When rounding, a suitable rounding mode must be selected.
  #endif // GUISTORM_ROUND_...
#endif // defined(GUISTORM_ROUND_NEAREST_OUT) || defined(GUISTORM_ROUND_NEAREST_ALL)
