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
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(2,0,0),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x03EB,
	.ProductID              = 0x2047,
	.ReleaseNumber          = VERSION_BCD(0,0,2),

	.ManufacturerStrIndex   = STRING_ID_Manufacturer,
	.ProductStrIndex        = STRING_ID_Product,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                   = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize   = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces          = 2,

			.ConfigurationNumber      = 1,
			.ConfigurationStrIndex    = NO_DESCRIPTOR,

			.ConfigAttributes         = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

			.MaxPowerConsumption      = USB_CONFIG_POWER_MA(100)
		},

	.Audio_ControlInterface =
		{
			.Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber          = INTERFACE_ID_AudioControl,
			.AlternateSetting         = 0,

			.TotalEndpoints           = 0,

			.Class                    = AUDIO_CSCP_AudioClass,
			.SubClass                 = AUDIO_CSCP_ControlSubclass,
			.Protocol                 = AUDIO_CSCP_ControlProtocol,

			.InterfaceStrIndex        = NO_DESCRIPTOR
		},

	.Audio_ControlInterface_SPC =
		{
			.Header                   = {.Size = sizeof(USB_Audio_Descriptor_Interface_AC_t), .Type = DTYPE_CSInterface},
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_Header,

			.ACSpecification          = VERSION_BCD(1,0,0),
			.TotalLength              = (sizeof(USB_Audio_Descriptor_Interface_AC_t) +
			                             sizeof(USB_Audio_Descriptor_InputTerminal_t) +
			                             sizeof(USB_Audio_Descriptor_OutputTerminal_t)),

			.InCollection             = 1,
			.InterfaceNumber          = INTERFACE_ID_AudioStream,
		},

	.Audio_InputTerminal =
		{
			.Header                   = {.Size = sizeof(USB_Audio_Descriptor_InputTerminal_t), .Type = DTYPE_CSInterface},
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_InputTerminal,

			.TerminalID               = 0x01,
			.TerminalType             = AUDIO_TERMINAL_IN_MIC,
			.AssociatedOutputTerminal = 0x00,

			.TotalChannels            = 1,
			.ChannelConfig            = 0,

			.ChannelStrIndex          = NO_DESCRIPTOR,
			.TerminalStrIndex         = NO_DESCRIPTOR
		},

	.Audio_OutputTerminal =
		{
			.Header                   = {.Size = sizeof(USB_Audio_Descriptor_OutputTerminal_t), .Type = DTYPE_CSInterface},
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,

			.TerminalID               = 0x02,
			.TerminalType             = AUDIO_TERMINAL_STREAMING,
			.AssociatedInputTerminal  = 0x00,

			.SourceID                 = 0x01,

			.TerminalStrIndex         = NO_DESCRIPTOR
		},

	.Audio_StreamInterface_Alt0 =
		{
			.Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber          = INTERFACE_ID_AudioStream,
			.AlternateSetting         = 0,

			.TotalEndpoints           = 0,

			.Class                    = AUDIO_CSCP_AudioClass,
			.SubClass                 = AUDIO_CSCP_AudioStreamingSubclass,
			.Protocol                 = AUDIO_CSCP_StreamingProtocol,

			.InterfaceStrIndex        = NO_DESCRIPTOR
		},

	.Audio_StreamInterface_Alt1 =
		{
			.Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber          = INTERFACE_ID_AudioStream,
			.AlternateSetting         = 1,

			.TotalEndpoints           = 1,

			.Class                    = AUDIO_CSCP_AudioClass,
			.SubClass                 = AUDIO_CSCP_AudioStreamingSubclass,
			.Protocol                 = AUDIO_CSCP_StreamingProtocol,

			.InterfaceStrIndex        = NO_DESCRIPTOR
		},

	.Audio_StreamInterface_SPC =
		{
			.Header                   = {.Size = sizeof(USB_Audio_Descriptor_Interface_AS_t), .Type = DTYPE_CSInterface},
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_General,

			.TerminalLink             = 0x02,   // relates to this OUTPUT terminal - forwarded to the INPUT terminal from there
         // Unbedingt lesen: audio10.pdf, Seite 105, Example 1: USB Microphone

			.FrameDelay               = 1,      // given in number of USB frames (might be 0 for just a mic)
			.AudioFormat              = 0x0001  // PCM (see frmts10.pdf, page 29, table A.1.1), signed!
		},

	.Audio_AudioFormat =
		{
			.Header                   = {.Size = sizeof(USB_Audio_Descriptor_Format_t) +
			                                     sizeof(ConfigurationDescriptor.Audio_AudioFormatSampleRates),
			                             .Type = DTYPE_CSInterface},
			.Subtype                  = AUDIO_DSUBTYPE_CSInterface_FormatType,

			.FormatType               = 0x01,   // Type I format
			.Channels                 = 0x01,   //  1 channel (mono)

			.SubFrameSize             = 0x02,   //  2 bytes per sample on each channel
			.BitResolution            = 16,     // 16 bits per sample on each channel

			.TotalDiscreteSampleRates = (sizeof(ConfigurationDescriptor.Audio_AudioFormatSampleRates) / sizeof(USB_Audio_SampleFreq_t))
		},

	.Audio_AudioFormatSampleRates =
		{
			AUDIO_SAMPLE_FREQ(8000),
			AUDIO_SAMPLE_FREQ(11025),
			AUDIO_SAMPLE_FREQ(22050),
			AUDIO_SAMPLE_FREQ(44100),
			AUDIO_SAMPLE_FREQ(48000),
		},

	.Audio_StreamEndpoint =
		{
			.Endpoint =
				{
					.Header              = {.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), .Type = DTYPE_Endpoint},

					.EndpointAddress     = AUDIO_STREAM_EPADDR,
					.Attributes          = (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNC | ENDPOINT_USAGE_DATA),
					.EndpointSize        = AUDIO_STREAM_EPSIZE,
					.PollingIntervalMS   = 0x01
				},

			.Refresh                  = 0,
			.SyncEndpointNumber       = 0
		},

	.Audio_StreamEndpoint_SPC =
		{
			.Header                   = {.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t), .Type = DTYPE_CSEndpoint},
			.Subtype                  = AUDIO_DSUBTYPE_CSEndpoint_General,

			.Attributes               = (AUDIO_EP_ACCEPTS_SMALL_PACKETS | AUDIO_EP_SAMPLE_FREQ_CONTROL),

			.LockDelayUnits           = 0x00,
			.LockDelay                = 0x0000
		}
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"Dean Camera");

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"LUFA Audio In Demo");

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case STRING_ID_Language:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case STRING_ID_Manufacturer:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case STRING_ID_Product:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
	}

	*DescriptorAddress = Address;
	return Size;
}

