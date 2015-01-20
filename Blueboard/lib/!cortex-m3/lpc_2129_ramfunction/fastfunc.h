#define FASTRUN __attribute__ ((long_call, section (".fastrun")))

extern FASTRUN void setLED1(int on);
extern FASTRUN void setLED2(int on);
