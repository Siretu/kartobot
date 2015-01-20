//////////////////////////////////////////////////////////////////////////////
// Product: BLINKY example application
// Date of the Last Update:  Jun 29, 2007
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2007 Quantum Leaps, LLC. All rights reserved.
//
// Contact information:
// Quantum Leaps Web site:  http://www.quantum-leaps.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////

#include "bsp.h"                                      // Board Support Package
#include "isr.h"                         // interface to the ISRs (foreground)

// class Blinky ..............................................................
class Blinky {
private:
    uint8_t state_;
    uint8_t id_;
    uint32_t offDelay_;
    uint32_t onDelay_;
    uint32_t ctr_;

public:
    Blinky(uint8_t id, uint32_t offDelay, uint32_t onDelay);           // ctor
    virtual ~Blinky();                                         // virtual xtor
    virtual void dispatch(void);
};

enum BlinkyStates {
    BLINKY_OFF,
    BLINKY_ON
};

//............................................................................
Blinky::Blinky(uint8_t id, uint32_t offDelay, uint32_t onDelay)
    : id_(id), offDelay_(offDelay), onDelay_(onDelay)
{
    state_ = BLINKY_OFF;                   // initial transition to BLINKY_OFF
    ctr_   = offDelay;
    LED_OFF(id_);
}
//............................................................................
Blinky::~Blinky() {
}
//............................................................................
void Blinky::dispatch(void) {
    switch (state_) {
        case BLINKY_OFF: {
           --ctr_;
           if (ctr_ == 0) {
               state_ = BLINKY_ON;                   // transition to ON state
               ctr_ = onDelay_;
               LED_ON(id_);
           }
           break;
        }
        case BLINKY_ON: {
           --ctr_;
           if (ctr_ == 0) {
               state_ = BLINKY_OFF;                 // transition to OFF state
               ctr_ = offDelay_;
               LED_OFF(id_);
           }
           break;
        }
    }
}

// test harness --------------------------------------------------------------
static Blinky blinky_pit   (1,     9,     1);                   // static ctor
static Blinky blinky_timer0(2,  9000,  1000);                   // static ctor
static Blinky blinky_timer1(3,  9000,  1000);                   // static ctor
static Blinky blinky_idle  (0, 18000,  2000);                   // static ctor

static Blinky *pBlinky[] = {             // pointers to Blinky (.data section)
    &blinky_pit,
    &blinky_timer0,
    &blinky_timer1
};

int main (void) {
    BSP_init();                        // initialize the Board Support Package

    for (;;) {                                                     // for-ever
        if (eventFlagCheck(PIT_FLAG)) {
            pBlinky[PIT_FLAG]->dispatch();                     // late binding
        }

        if (eventFlagCheck(TIMER0_FLAG)) {
            pBlinky[TIMER0_FLAG]->dispatch();                  // late binding
        }

        if (eventFlagCheck(TIMER1_FLAG)) {
            pBlinky[TIMER1_FLAG]->dispatch();                  // late binding
        }

        blinky_idle.dispatch();                               // early binding
    }

    return 0;    // unreachable; this return is only to avoid compiler warning
}
