#ifndef FF_TEST_TERM_H_
#define FF_TEST_TERM_H_

#include "ff.h"
#include "diskio.h"

extern FATFS Fatfs[_VOLUMES];

void ff_test_term_timer_callback(void); //  __attribute__ ((section(".ramfunc")));
int ff_test_term(void);
void put_rc(FRESULT rc);
void put_ds(DSTATUS rc);

#endif

