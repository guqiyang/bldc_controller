/*
    Copyright (C) 2014 Anthony Hutchinson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

// What should the diagnostics pin be raised?
//#define DIAG_ZC_INTERRUPT
//#define DIAG_ZC
#define DIAG_COMMUTATION_CYCLE
//#define DIAG_COMMUTATION_INTERRUPT

#define SPEED_PIN A5

// The lowest prescale that works is 8x, anything lower
// and commutation_period overflows.
#define PRESCALE 8
#define PRESCALE_BITS _BV(CS11)
#define TICKS_PER_MICROSECOND (16 / PRESCALE)
#define MICROSECONDS_PER_TICK (1.0 / TICKS_PER_MICROSECOND)


// lots of hard coded timer stuff here....
#define disable_timer1_compb();  cbi(TIMSK1, OCIE1B);
#define enable_timer1_compb();  sbi(TIMSK1, OCIE1B);

class Motor {
  
  public:
    Motor(int poles, int speed_pin);
    void start();
    unsigned int rpm();
    unsigned int rpm(unsigned int __commutation_period) { 
        return (unsigned int)(60 * 1000000.0 / (__commutation_period) / (MICROSECONDS_PER_TICK * poles)); 
    }

    void next_commutation(); // advance to the next commutation
    void pwm_on();  // called when pwm should be on 
    void pwm_off(); // called when pwm should be off
    unsigned int speed_control();
    void zero_crossing_interrupt();  // called from global
    int desired_rpm;
    
  //private:
    int poles;       // number of pole pairs
    int speed_pin;   // pin the speed potentiometer is connected to
    char direction;
    boolean auto_phase_shift;
    boolean sensing; // is commutation driven by interrupts (rather than timing)

    // even/odd debugging/phase shifting
    unsigned int last_even;
    unsigned int last_odd;
    unsigned int last_commutation; // for commutation to zero crossing timing
    
    // commutation
    int interrupt_count;              // # of interrupts since last reset()
    int phase_shift;                  // # of ticks to shift commutation by
    unsigned int commutation_period;  // time of last commutation cycle, in timer1 ticks
    unsigned int commutation_period_accumulator; // sum of commutation periods since begining of last cycle
    byte commutation;                 // current commutation step
    byte _commutation;                // the current commutation power bits
    
    void reset();
    void initialize_timers();

    // control input smoothing
    unsigned int inputs[16];
    byte inputs_idx;
    unsigned int inputs_sum;
 
};

#endif

