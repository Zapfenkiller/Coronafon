/*
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
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
 *  Main source file for the AudioInput demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "Coronafon.h"


static uint8_t    pushbutton_previous = 0;
static uint16_t   antibounce = 0;


/** LUFA Audio Class driver interface configuration and state information. This structure is
 *  passed to all Audio Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_Audio_Device_t Microphone_Audio_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_AudioControl,
				.StreamingInterfaceNumber = INTERFACE_ID_AudioStream,
				.DataINEndpoint           =
					{
						.Address          = AUDIO_STREAM_EPADDR,
						.Size             = AUDIO_STREAM_EPSIZE,
						.Banks            = 2,
					},
			},
	};

/** Current audio sampling frequency of the streaming audio endpoint. */
static uint32_t CurrentAudioSampleFrequency = 11025;


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	for (;;)
	{
		Audio_Device_USBTask(&Microphone_Audio_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#endif

	/* Hardware Initialization */
	LEDs_Init();
	Buttons_Init();
	ADC_Init(ADC_FREE_RUNNING | ADC_PRESCALE_64);
	ADC_SetupChannel(MIC_IN_ADC_CHANNEL);
	USB_Init();

	/* Start the ADC conversion in free running mode, 32U4: ADHSM=1 */
	ADC_StartReading(1 << 7 | ADC_REFERENCE_INT2560MV | ADC_LEFT_ADJUSTED | ADC_GET_CHANNEL_MASK(MIC_IN_ADC_CHANNEL));
}

/** ISR to handle the reloading of the data endpoint with the next sample. */
ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	uint8_t PrevEndpoint = Endpoint_GetCurrentEndpoint();

	/* Check that the USB bus is ready for the next sample to write */
	if (Audio_Device_IsReadyForNextSample(&Microphone_Audio_Interface))
	{
		int16_t AudioSample;

      uint8_t pushbutton = Buttons_GetStatus() & BUTTONS_BUTTON1;
      if (pushbutton ^ pushbutton_previous)
      {
         // state change appeared, prepare 100 ms debouncing time
         // restarted by each change during this period
         antibounce = CurrentAudioSampleFrequency / 10;
      }
      else
      {
         if (antibounce)
            antibounce--;
      }
      pushbutton_previous = pushbutton;

      // Only generate audio if the board button is being pressed
      // Starts with antibounce delay on keypress but stops immediately on key release
      AudioSample = (pushbutton && !antibounce) ? ADC_GetResult() - DC_OFFSET_LSBs : 0;

		Audio_Device_WriteSample16(&Microphone_Audio_Interface, AudioSample);

      /* 3.6. 2020:
         ----------
         Das Coronafon funktioniert jetzt und macht den Job für
         Audio-Konferenzen. Mehr braucht es nicht. Das Ding ist
         erstaunlich empfindlich, schon in der niedrigsten Stellung.
         Dafür übersteuert es auch nicht nennenswert. Aber es gibt
         Aliasing bei Pfeiftönen. Egal. Sprache kommt sauber und
         ohne Artefakte.
         
         4.6.2020:
         ---------
         Eins fehlte noch: Entprellung des Tasters!
         Die statische Offsetlage anstelle einer dynamischen
         Nachführung ist akzeptabel.
         
         20.2.2021:
         ----------
         Prinzipiell stimmt das Datenformat, signed wird aus dem ADC-Rohmaterial
         durch Abziehen eines Offsets errechnet. Aus unerfindlichen Gründen hat
         Teams in jeder TelCo Startprobleme. Kollegen berichten, dass es etwa
         die erste halbe Sekunde massiv übersteuert. Danach ist das Audio gut.
         
         Dazu braucht das Ding einen automatischen Nullabgleich. Das
         sollte ohne viel Aufriss machbar sein. Vorzeichen des Audio-
         samples bestimmt Integrationsrichtung (CB-Wandler!) und der
         läuft schön langsam hinterher. Die Datenformate geschickt nutzen,
         wegen des ADC sind die 5 LSBs vom Audio immer 0.
         
         Weitere Idee: Aussteuerung und Offset auf Neopixel oder Bargraph
         liefern; 0 dB .. -54 dB für die 10 Bit signed.
           0 dB ..  -6 dB = LED oben
          -6 dB .. -12 dB = LED darunter
          ....
         -48 dB .. -54 dB = LED unten
         Ist aber mehr als Gimmick oder zum Debuggen des Offset-Abgleichers.
         
         22.2.2021:
         ----------
         Ob ein Fade-In (ziemlich banal per Schiebereien) den Ärger mit dem Teams
         mindern könnte?
         
         Die Grenzfrequenz des analogen TP könnte ein wenig weiter runter, man
         nehme etwas zwischen 1n0 und 2n2.
      */
	}

	Endpoint_SelectEndpoint(PrevEndpoint);
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);

	/* Sample reload timer initialization */
	TIMSK0  = (1 << OCIE0A);
	OCR0A   = ((F_CPU / 8 / CurrentAudioSampleFrequency) - 1);
	TCCR0A  = (1 << WGM01);  // CTC mode
	TCCR0B  = (1 << CS01);   // Prescaler :8
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	/* Stop the sample reload timer */
	TCCR0B = 0;

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= Audio_Device_ConfigureEndpoints(&Microphone_Audio_Interface);

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	Audio_Device_ProcessControlRequest(&Microphone_Audio_Interface);
}

/** Audio class driver callback for the setting and retrieval of streaming endpoint properties. This callback must be implemented
 *  in the user application to handle property manipulations on streaming audio endpoints.
 *
 *  When the DataLength parameter is NULL, this callback should only indicate whether the specified operation is valid for
 *  the given endpoint index, and should return as fast as possible. When non-NULL, this value may be altered for GET operations
 *  to indicate the size of the retrieved data.
 *
 *  \note The length of the retrieved data stored into the Data buffer on GET operations should not exceed the initial value
 *        of the \c DataLength parameter.
 *
 *  \param[in,out] AudioInterfaceInfo  Pointer to a structure containing an Audio Class configuration and state.
 *  \param[in]     EndpointProperty    Property of the endpoint to get or set, a value from Audio_ClassRequests_t.
 *  \param[in]     EndpointAddress     Address of the streaming endpoint whose property is being referenced.
 *  \param[in]     EndpointControl     Parameter of the endpoint to get or set, a value from Audio_EndpointControls_t.
 *  \param[in,out] DataLength          For SET operations, the length of the parameter data to set. For GET operations, the maximum
 *                                     length of the retrieved data. When NULL, the function should return whether the given property
 *                                     and parameter is valid for the requested endpoint without reading or modifying the Data buffer.
 *  \param[in,out] Data                Pointer to a location where the parameter data is stored for SET operations, or where
 *                                     the retrieved data is to be stored for GET operations.
 *
 *  \return Boolean \c true if the property get/set was successful, \c false otherwise
 */
bool CALLBACK_Audio_Device_GetSetEndpointProperty(USB_ClassInfo_Audio_Device_t* const AudioInterfaceInfo,
                                                  const uint8_t EndpointProperty,
                                                  const uint8_t EndpointAddress,
                                                  const uint8_t EndpointControl,
                                                  uint16_t* const DataLength,
                                                  uint8_t* Data)
{
	/* Check the requested endpoint to see if a supported endpoint is being manipulated */
	if (EndpointAddress == Microphone_Audio_Interface.Config.DataINEndpoint.Address)
	{
		/* Check the requested control to see if a supported control is being manipulated */
		if (EndpointControl == AUDIO_EPCONTROL_SamplingFreq)
		{
			switch (EndpointProperty)
			{
				case AUDIO_REQ_SetCurrent:
					/* Check if we are just testing for a valid property, or actually adjusting it */
					if (DataLength != NULL)
					{
						/* Set the new sampling frequency to the value given by the host */
						CurrentAudioSampleFrequency = (((uint32_t)Data[2] << 16) | ((uint32_t)Data[1] << 8) | (uint32_t)Data[0]);

						/* Adjust sample reload timer to the new frequency */
						OCR0A = ((F_CPU / 8 / CurrentAudioSampleFrequency) - 1);
                  
					}

					return true;
				case AUDIO_REQ_GetCurrent:
					/* Check if we are just testing for a valid property, or actually reading it */
					if (DataLength != NULL)
					{
						*DataLength = 3;

						Data[2] = (CurrentAudioSampleFrequency >> 16);
						Data[1] = (CurrentAudioSampleFrequency >> 8);
						Data[0] = (CurrentAudioSampleFrequency &  0xFF);
					}

					return true;
			}
		}
	}

	return false;
}

/** Audio class driver callback for the setting and retrieval of streaming interface properties. This callback must be implemented
 *  in the user application to handle property manipulations on streaming audio interfaces.
 *
 *  When the DataLength parameter is NULL, this callback should only indicate whether the specified operation is valid for
 *  the given entity and should return as fast as possible. When non-NULL, this value may be altered for GET operations
 *  to indicate the size of the retrieved data.
 *
 *  \note The length of the retrieved data stored into the Data buffer on GET operations should not exceed the initial value
 *        of the \c DataLength parameter.
 *
 *  \param[in,out] AudioInterfaceInfo  Pointer to a structure containing an Audio Class configuration and state.
 *  \param[in]     Property            Property of the interface to get or set, a value from Audio_ClassRequests_t.
 *  \param[in]     EntityAddress       Address of the audio entity whose property is being referenced.
 *  \param[in]     Parameter           Parameter of the entity to get or set, specific to each type of entity (see USB Audio specification).
 *  \param[in,out] DataLength          For SET operations, the length of the parameter data to set. For GET operations, the maximum
 *                                     length of the retrieved data. When NULL, the function should return whether the given property
 *                                     and parameter is valid for the requested endpoint without reading or modifying the Data buffer.
 *  \param[in,out] Data                Pointer to a location where the parameter data is stored for SET operations, or where
 *                                     the retrieved data is to be stored for GET operations.
 *
 *  \return Boolean \c true if the property GET/SET was successful, \c false otherwise
 */
bool CALLBACK_Audio_Device_GetSetInterfaceProperty(USB_ClassInfo_Audio_Device_t* const AudioInterfaceInfo,
                                                   const uint8_t Property,
                                                   const uint8_t EntityAddress,
                                                   const uint16_t Parameter,
                                                   uint16_t* const DataLength,
                                                   uint8_t* Data)
{
	/* No audio interface entities in the device descriptor, thus no properties to get or set. */
	return false;
}
