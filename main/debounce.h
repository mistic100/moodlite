class CDebounce {
  public:
  unsigned long mLast = 0;
  unsigned long mPeriod;
  CDebounce(unsigned long period) { mPeriod = period; }
  operator bool() { 
    unsigned long curr = millis();
    bool done = curr - mLast > mPeriod;
    mLast = curr;
    return done;
  }
};

#define DEBOUNCE_I(NAME, TIME) static CDebounce NAME(TIME); if (NAME)
#define DEBOUNCE(TIME) DEBOUNCE_I(CONCAT_MACRO(deb, __COUNTER__), TIME)
