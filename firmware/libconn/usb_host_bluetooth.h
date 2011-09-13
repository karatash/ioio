/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

//
// USB Host Bluetooth dongle Driver (Header File)
//
// The driver passes on the following events to the application event handler
// (defined by the USB_HOST_APP_EVENT_HANDLER macro):
// - EVENT_DETACH - when device is detached.
// - EVENT_SUSPEND - when device is suspended.
// - EVENT_RESUME - when device is resumed.
//
// This driver can be configured to either use transfer events from usb_host.c
// or use a polling mechanism.  If USB_ENABLE_TRANSFER_EVENT is defined, this
// driver will utilize transfer events.  Otherwise, this driver will utilize
// polling.
//
// Since the generic class is performed with interrupt transfers,
// USB_SUPPORT_INTERRUPT_TRANSFERS must be defined.

#ifndef __USBHOSTBLUETOOTH_H__
#define __USBHOSTBLUETOOTH_H__

#include "usb_config.h"
#include "USB/usb_common.h"
#include "usb_host_driver_common.h"

//////////////////////////////////////////////////////////////////////////////////
//// The following code is not intended for the client to use directly.
//////////////////////////////////////////////////////////////////////////////////

// Generic Device Information
// This structure contains information about an attached device, including
// status flags and device identification.
typedef struct {
  USB_DEVICE_ID     ID;              // Identification information about the device
  DWORD             rxLength;        // Number of bytes received in the last IN transfer
  BOOL              initialized;     // Is the driver initialized
  BOOL              txBusy;          // Driver busy transmitting data
  BOOL              rxBusy;          // Driver busy receiving data
  BYTE              bulkinEndpoint;  // Address of the bulk input endpoint
  BYTE              bulkoutEndpoint; // Address of bulk output endpoint
  BYTE              intEndpoint;     // Address of interrupt input endpoint
  BYTE              rxErrorCode;     // Error code of last IN transfer
  BYTE              txErrorCode;     // Error code of last OUT transfer
} BLUETOOTH_DEVICE;

extern BLUETOOTH_DEVICE gc_BluetoothDevData; // Information about the attached device.

////////////////////////////////////////////////////////////////////////////////
// The following two functions are to be put in the driver table and called by
// the USB host layer. Should not be called directly by the client.
////////////////////////////////////////////////////////////////////////////////

BOOL USBHostBluetoothInit ( BYTE address, DWORD flags, BYTE clientDriverID );
BOOL USBHostBluetoothEventHandler ( BYTE address, USB_EVENT event, void *data, DWORD size );


////////////////////////////////////////////////////////////////////////////////
// The following code is the public API of the driver - should be used by the
// client.
////////////////////////////////////////////////////////////////////////////////

// Check whether a device is currently attached.
#define USBHostBluetoothIsDeviceAttached() (gc_BluetoothDevData.initialized)

// Resets the device and restarts all the attachment process.
// Device must be attached.
void USBHostBluetoothdReset();

// Returns the ID of the currently attached device.
// Device must be attached at the time of call, and argument must not be NULL.
void USBHostBluetoothGetDeviceId(USB_DEVICE_ID *pDevID);

// Issue a read request from the device.
// Actual read will be done asynchronously. Client should call
// USBHostBluetoothRxIsComplete() to check for completion and get status code.
// Returns USB_SUCCESS if succeeded.
// Device must be attached.
BYTE USBHostBluetoothRead(void *buffer, DWORD length);

// Check whether the last call to USBHostBluetoothRead has completed.
// In case it is complete, returns TRUE, and the error code and number of bytes
// read are returned.
BOOL USBHostBluetoothRxIsComplete(BYTE *errorCode, DWORD *byteCount);

// Issue a read request to the device.
// Actual write will be done asynchronously. Client should call
// USBHostBluetoothTxIsComplete() to check for completion and get status code.
// Returns USB_SUCCESS if succeeded.
// Device must be attached.
BYTE USBHostBluetoothWrite(const void *buffer, DWORD length);

// Check whether the last call to USBHostBluetoothWrite has completed.
// In case it is complete, returns TRUE, and the error code is returned.
BOOL USBHostBluetoothTxIsComplete(BYTE *errorCode);

// This function must be called periodically by the client to provide context to
// the driver IF NOT working with transfer events (USB_ENABLE_TRANSFER_EVENT)
// It will poll for the status of transfers.
#ifndef USB_ENABLE_TRANSFER_EVENT
void USBHostBluetoothTasks( void );
#endif  // USB_ENABLE_TRANSFER_EVENT


#endif  // __USBHOSTBLUETOOTH_H__