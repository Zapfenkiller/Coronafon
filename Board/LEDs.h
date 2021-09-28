/*
  * The USB-Userport *
  Copyright 2020  René Trapp (rene [dot] trapp (-at-) web [dot] de)

  * LUFA Library *
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/


/** \file
 *
 *  LED definitions for Sparkfun ProMicro.
 *  \note The LEDs are lit when the port drives '0'.
 */

#ifndef __LEDS_USER_H__
#define __LEDS_USER_H__
   /* Includes: */
      #include <avr/io.h>


   /* Enable C linkage for C++ Compilers: */
      #if defined(__cplusplus)
         extern "C" {
      #endif

   /* Preprocessor Checks: */
      #if !defined(__INCLUDE_FROM_LEDS_H)
         #error Do not include this file directly. Include LUFA/Drivers/Board/LEDS.h instead.
      #endif

   /* Public Interface - May be used in end-application: */
   /* Macros: */
      /** LED mask for the first LED on the board. */
      #define LEDS_LED1        (1 << 0)  // PB0

      /** LED mask for the second LED on the board. */
      #define LEDS_LED2        (1 << 5)  // PD5

      /** LED mask for all LEDs on the board. */
      #define LEDS_ALL_LEDS    (LEDS_LED1 | LEDS_LED2)

      /** LED mask for no LED on the port */
      #define LEDS_NO_LEDS     0

      /* Inline Functions: */
      #if !defined(__DOXYGEN__)
         static inline void LEDs_Init(void)
         {
            DDRB  |=  LEDS_LED1;
            PORTB |=  LEDS_LED1;
            DDRD  |=  LEDS_LED2;
            PORTD |=  LEDS_LED2;
         }

         static inline void LEDs_Disable(void)
         {
            DDRB  &= ~LEDS_LED1;
            PORTB &= ~LEDS_LED1;
            DDRD  &= ~LEDS_LED2;
            PORTD &= ~LEDS_LED2;
         }

         static inline void LEDS_LED1_ON(void)
         {
            PORTB &= ~LEDS_LED1;
         }

         static inline void LEDS_LED1_OFF(void)
         {
            PORTB |= LEDS_LED1;
         }

         static inline void LEDS_LED2_ON(void)
         {
            PORTD &= ~LEDS_LED2;
         }

         static inline void LEDS_LED2_OFF(void)
         {
            PORTD |= LEDS_LED2;
         }

         static inline void LEDs_TurnOnLEDs(const uint8_t LEDMask)
         {
            PORTB &= ~(LEDMask & LEDS_LED1);
            PORTD &= ~(LEDMask & LEDS_LED2);
         }

         static inline void LEDs_TurnOffLEDs(const uint8_t LEDMask)
         {
            PORTB |=  (LEDMask & LEDS_LED1);
            PORTD |=  (LEDMask & LEDS_LED2);
         }

         static inline void LEDs_SetAllLEDs(const uint8_t LEDMask)
         {
            PORTB = ((PORTB | LEDS_LED1) & ~(LEDMask & LEDS_LED1));
            PORTD = ((PORTD | LEDS_LED2) & ~(LEDMask & LEDS_LED2));
         }

         static inline void LEDs_ChangeLEDs(const uint8_t LEDMask, const uint8_t ActiveMask)
         {
            PORTB = ((PORTB | (LEDMask & LEDS_LED1)) & ~(ActiveMask & LEDS_LED1));
            PORTD = ((PORTD | (LEDMask & LEDS_LED2)) & ~(ActiveMask & LEDS_LED2));
         }

         static inline void LEDs_ToggleLEDs(const uint8_t LEDMask)
         {
            PORTB ^= (LEDMask & LEDS_LED1);
            PORTD ^= (LEDMask & LEDS_LED2);
         }

         static inline uint8_t LEDs_GetLEDs(void) ATTR_WARN_UNUSED_RESULT;
         static inline uint8_t LEDs_GetLEDs(void)
         {
            return (((PORTB & LEDS_LED1) | (PORTD & LEDS_LED2)) ^ LEDS_ALL_LEDS);
         }
      #endif

   /* Disable C linkage for C++ Compilers: */
      #if defined(__cplusplus)
         }
      #endif
#endif
