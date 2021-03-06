/*
 * Sodaq_PcInt.cpp
 *
 * Copyright (c) 2014 Kees Bakker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This module supplies a set of helper functions to use the
 * PinChange interrupt in a convenient manner, similar to
 * the standard Arduino attachInterrupt.  It was created with
 * inspiration from PcInt, PinChangeInt and PciManager.  The main
 * goal was to keep it simple and small as possible.
 *
 * The handler prototype is void (*)(void).  This makes is identical
 * to the call backs for Ardiuno's attachInterrupt.
 *
 * A simple example of its usage is as follows:
 *
 *   #include <PcInt.h>
 *
 *   void setup()
 *   {
 *     pinMode(A0, INPUT_PULLUP);
 *     PcInt::attachInterrupt(A0, handleA0);
 *   }
 *
 *   void handlerA0()
 *   {
 *     // pin A0 changed, do something
 *   }
 *
 * The user program is responsible to look at the I/O pin and see what
 * happened.  The original PcInt keeps track of old port values so that
 * it can see which of the port pins changed.
 */

#include <avr/interrupt.h>
#include <Arduino.h>

#include "Sodaq_PcInt.h"



#if defined(PCINT0_vect)
void   (*PcInt::_funcs0[8])(void);
volatile uint8_t PcInt::port0state = 0;
uint8_t PcInt::port0rising = 0;
uint8_t PcInt::port0falling = 0;
#endif
#if defined(PCINT1_vect)
void   (*PcInt::_funcs1[8])(void);
volatile uint8_t PcInt::port1state = 0;
uint8_t PcInt::port1rising = 0;
uint8_t PcInt::port1falling = 0;
#endif
#if defined(PCINT2_vect)
void   (*PcInt::_funcs2[8])(void);
volatile uint8_t PcInt::port2state = 0;
uint8_t PcInt::port2rising = 0;
uint8_t PcInt::port2falling = 0;
#endif
#if defined(PCINT3_vect)
void   (*PcInt::_funcs3[8])(void);
volatile uint8_t PcInt::port3state = 0;
uint8_t PcInt::port3rising = 0;
uint8_t PcInt::port3falling = 0;
#endif

/*
 * Set the function pointer in the array using the port's pin bit mask
 */
static void setFunc(void (*funcs[])(void), uint8_t portBitMask, void (*func)(void))
{
  for (uint8_t i = 0; i < 8; ++i) {
    if (portBitMask & 1) {
      funcs[i] = func;
      break;
    }
    portBitMask >>= 1;
  }
}

void PcInt::attachInterrupt(uint8_t pin, void(*func)(void), uint8_t modeMask)
{
  volatile uint8_t * pcicr = digitalPinToPCICR(pin);
  volatile uint8_t * pcmsk = digitalPinToPCMSK(pin);
  if (pcicr && pcmsk) {
    uint8_t pcintGroup = digitalPinToPCICRbit(pin);
    uint8_t portBitMask = digitalPinToBitMask(pin);
    switch (pcintGroup) {
#if defined(PCINT0_vect)
    case 0:
      setFunc(_funcs0, portBitMask, func);
      port0rising |= (modeMask & RISING_MODE) ? portBitMask : 0;
      port0falling |= (modeMask & FALLING_MODE) ? portBitMask : 0;
      port0state = PINA;
      break;
#endif
#if defined(PCINT1_vect)
    case 1:
      setFunc(_funcs1, portBitMask, func);
      port1rising |= (modeMask & RISING_MODE) ? portBitMask : 0;
      port1falling |= (modeMask & FALLING_MODE) ? portBitMask : 0;
      port1state = PINB;
      break;
#endif
#if defined(PCINT2_vect)
    case 2:
      setFunc(_funcs2, portBitMask, func);
      port2rising |= (modeMask & RISING_MODE) ? portBitMask : 0;
      port2falling |= (modeMask & FALLING_MODE) ? portBitMask : 0;
      port2state = PINC;
      break;
#endif
#if defined(PCINT3_vect)
    case 3:
      setFunc(_funcs3, portBitMask, func);
      port3rising |= (modeMask & RISING_MODE) ? portBitMask : 0;
      port3falling |= (modeMask & FALLING_MODE) ? portBitMask : 0;
      port3state = PIND;
      break;
#endif
    }
    *pcmsk |= _BV(digitalPinToPCMSKbit(pin));
    *pcicr |= _BV(digitalPinToPCICRbit(pin));
  }
}

void PcInt::detachInterrupt(uint8_t pin)
{
  volatile uint8_t * pcicr = digitalPinToPCICR(pin);
  volatile uint8_t * pcmsk = digitalPinToPCMSK(pin);
  if (pcicr && pcmsk) {
    uint8_t pcintGroup = digitalPinToPCICRbit(pin);
    uint8_t portBitMask = digitalPinToBitMask(pin);
    switch (pcintGroup) {
#if defined(PCINT0_vect)
    case 0:
      setFunc(_funcs0, portBitMask, NULL);
      port0rising &= ~portBitMask;
      port0falling &= ~portBitMask; 
      break;
#endif
#if defined(PCINT1_vect)
    case 1:
      setFunc(_funcs1, portBitMask, NULL);
      port1rising &= ~portBitMask;
      port1falling &= ~portBitMask;
      break;
#endif
#if defined(PCINT2_vect)
    case 2:
      setFunc(_funcs2, portBitMask, NULL);
      port2rising &= ~portBitMask;
      port2falling &= ~portBitMask;
      break;
#endif
#if defined(PCINT3_vect)
    case 3:
      setFunc(_funcs3, portBitMask, NULL);
      port3rising &= ~portBitMask;
      port3falling &= ~portBitMask;
      break;
#endif
    }
    *pcmsk &= ~_BV(digitalPinToPCMSKbit(pin));
    //Switch off the group if all of the group are now off
    if (*pcmsk == 0x00F){ //Alternatively "if (!*pcmsk)"
      *pcicr &= ~_BV(digitalPinToPCICRbit(pin));
    }
  }
}

void PcInt::enableInterrupt(uint8_t pin)
{
  volatile uint8_t * pcmsk = digitalPinToPCMSK(pin);
  if (pcmsk) {
    *pcmsk |= _BV(digitalPinToPCMSKbit(pin));
  }
}

void PcInt::disableInterrupt(uint8_t pin)
{
  volatile uint8_t * pcmsk = digitalPinToPCMSK(pin);
  if (pcmsk) {
    *pcmsk &= ~_BV(digitalPinToPCMSKbit(pin));
  }
}

/*
 * Get the installed function pointer
 *
 * This function serves just for diagnostic purposes.
 */
void (*PcInt::getFunc(uint8_t group, uint8_t nr))(void)
{
  if (nr >= 8) {
    return 0;
  }
  void   (**funcs)(void);
  switch (group) {
  case 0:
    funcs = _funcs0;
    break;
#if defined(PCINT1_vect)
  case 1:
    funcs = _funcs1;
    break;
#endif
#if defined(PCINT2_vect)
  case 2:
    funcs = _funcs2;
    break;
#endif
#if defined(PCINT3_vect)
  case 3:
    funcs = _funcs3;
    break;
#endif
  default:
    return 0;
    break;
  }
  return funcs[nr];
}

#if defined(PCINT0_vect)
inline void PcInt::handlePCINT0()
{
  uint8_t changedPins = port0state ^ PINA;
  port0state = PINA;
  for (uint8_t nr = 0; nr < 8; ++nr) {
    if (changedPins & _BV(nr)) {
      if (((_BV(nr) & port0rising & port0state) | (_BV(nr) & port0falling & ~port0state))) {
        if (_funcs0[nr]) {
          (*_funcs0[nr])();
        }
      }
    }
  }
}
ISR(PCINT0_vect)
{
  PcInt::handlePCINT0();
}
#endif

#if defined(PCINT1_vect)
inline void PcInt::handlePCINT1()
{
  uint8_t changedPins = port1state ^ PINB;
  port1state = PINB;
  for (uint8_t nr = 0; nr < 8; ++nr) {
    if (changedPins & _BV(nr)) {
      if (((_BV(nr) & port1rising & port1state) | (_BV(nr) & port1falling & ~port1state))) {
        if (_funcs1[nr]) {
          (*_funcs1[nr])();
        }
      }
    }
  }
}
ISR(PCINT1_vect)
{
  PcInt::handlePCINT1();
}
#endif

#if defined(PCINT2_vect)
inline void PcInt::handlePCINT2()
{
  uint8_t changedPins = port2state ^ PINC;
  port2state = PINC;
  for (uint8_t nr = 0; nr < 8; ++nr) {
    if (changedPins & _BV(nr)) {
      if (((_BV(nr) & port2rising & port2state) | (_BV(nr) & port2falling & ~port2state))) {
        if (_funcs2[nr]) {
          (*_funcs2[nr])();
        }
      }
    }
  }
}
ISR(PCINT2_vect)
{
  PcInt::handlePCINT2();
}
#endif

#if defined(PCINT3_vect)
inline void PcInt::handlePCINT3()
{
  uint8_t changedPins = port3state ^ PIND;
  port3state = PIND;
  for (uint8_t nr = 0; nr < 8; ++nr) {
    if (changedPins & _BV(nr)) {
      if (((_BV(nr) & port3rising & port3state) | (_BV(nr) & port3falling & ~port3state))) {
        if (_funcs3[nr]) {
          (*_funcs3[nr])();
        }
      }
    }
  }
  
}
ISR(PCINT3_vect)
{
  PcInt::handlePCINT3();
}
#endif
