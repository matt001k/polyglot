/////////////////////////////////////////////////////////////////////////////
// SLABHIDtoUART.h
// For SLABHIDtoUART.dll
// and Silicon Labs CP2110/CP2114 HID to UART
/////////////////////////////////////////////////////////////////////////////
#ifndef SLAB_HID_TO_UART_H
#define SLAB_HID_TO_UART_H

/// @addtogroup slabhiduart CP211X Serial over HID Library
///
/// @{

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include	"silabs_defs.h"
#include	"silabs_sal.h"
#include "Types.h"

/////////////////////////////////////////////////////////////////////////////
// Tool-chain-dependent hacks
/////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HID_TO_UART_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HID_TO_UART_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#if defined(HID_TO_UART_EXPORTS)
#define HID_TO_UART_API
#else
#define HID_TO_UART_API __declspec(dllimport)
#pragma comment(lib, "SLABHIDtoUART.lib")
#endif // defined(HID_TO_UART_EXPORTS)
#else // !_WIN32
#define HID_TO_UART_API
#define WINAPI
#endif // !_WIN32


/////////////////////////////////////////////////////////////////////////////
// Return Code Definitions
/////////////////////////////////////////////////////////////////////////////

#if !defined(USE_LEGACY_HID_UART_STATUS)
/// API Status Return Codes
typedef enum _HID_UART_STATUS {
	HID_UART_SUCCESS					= SILABS_STATUS_SUCCESS
	, HID_UART_DEVICE_NOT_FOUND			= 0x01
	, HID_UART_INVALID_HANDLE			= 0x02
	, HID_UART_INVALID_DEVICE_OBJECT	= 0x03
	, HID_UART_INVALID_PARAMETER		= 0x04
	, HID_UART_INVALID_REQUEST_LENGTH	= 0x05

	, HID_UART_READ_ERROR				= 0x10
	, HID_UART_WRITE_ERROR				= 0x11
	, HID_UART_READ_TIMED_OUT			= 0x12
	, HID_UART_WRITE_TIMED_OUT			= 0x13
	, HID_UART_DEVICE_IO_FAILED			= 0x14
	, HID_UART_DEVICE_ACCESS_ERROR		= 0x15
	, HID_UART_DEVICE_NOT_SUPPORTED		= 0x16
	, HID_UART_INVALID_CONFIG_VERSION	= 0x17

	, HID_UART_UNKNOWN_ERROR			= SILABS_STATUS_UNKNOWN_ERROR
} HID_UART_STATUS, *PHID_UART_STATUS;
#else // defined(USE_LEGACY_HID_UART_STATUS)

// HID_UART_STATUS
typedef int HID_UART_STATUS;

// API Status Return Codes
#define HID_UART_SUCCESS							SILABS_STATUS_SUCCESS
#define	HID_UART_DEVICE_NOT_FOUND					0x01
#define HID_UART_INVALID_HANDLE						0x02
#define	HID_UART_INVALID_DEVICE_OBJECT				0x03
#define	HID_UART_INVALID_PARAMETER					0x04
#define	HID_UART_INVALID_REQUEST_LENGTH				0x05

#define	HID_UART_READ_ERROR							0x10
#define	HID_UART_WRITE_ERROR						0x11
#define	HID_UART_READ_TIMED_OUT						0x12
#define	HID_UART_WRITE_TIMED_OUT					0x13
#define	HID_UART_DEVICE_IO_FAILED					0x14
#define HID_UART_DEVICE_ACCESS_ERROR				0x15
#define HID_UART_DEVICE_NOT_SUPPORTED				0x16
#define HID_UART_INVALID_CONFIG_VERSION             0x17

#define HID_UART_UNKNOWN_ERROR						SILABS_STATUS_UNKNOWN_ERROR
#endif // defined(USE_LEGACY_HID_UART_STATUS)

/////////////////////////////////////////////////////////////////////////////
// String Definitions
/////////////////////////////////////////////////////////////////////////////

/// Product String option Types
/// @defgroup GetStringOptions
/// @{
#define HID_UART_GET_VID_STR						0x01
#define HID_UART_GET_PID_STR						0x02
#define HID_UART_GET_PATH_STR						0x03
#define HID_UART_GET_SERIAL_STR						0x04
#define HID_UART_GET_MANUFACTURER_STR				0x05
#define HID_UART_GET_PRODUCT_STR					0x06
/// @}

// String Lengths
#define HID_UART_DEVICE_STRLEN						260

// HID_UART_DEVICE_STR
typedef char HID_UART_DEVICE_STR[HID_UART_DEVICE_STRLEN];

/////////////////////////////////////////////////////////////////////////////
// UART Definitions
/////////////////////////////////////////////////////////////////////////////

// Error Status
#define HID_UART_PARITY_ERROR						0x01
#define HID_UART_OVERRUN_ERROR						0x02

// Line Break Status
#define HID_UART_LINE_BREAK_INACTIVE				0x00
#define HID_UART_LINE_BREAK_ACTIVE					0x01

// Data Bits
#define HID_UART_FIVE_DATA_BITS						0x00
#define HID_UART_SIX_DATA_BITS						0x01
#define HID_UART_SEVEN_DATA_BITS					0x02
#define HID_UART_EIGHT_DATA_BITS					0x03

// Parity
#define HID_UART_NO_PARITY							0x00
#define HID_UART_ODD_PARITY							0x01
#define HID_UART_EVEN_PARITY						0x02
#define HID_UART_MARK_PARITY						0x03
#define HID_UART_SPACE_PARITY						0x04

// Stop Bits
// Short = 1 stop bit
// Long  = 1.5 stop bits (5 data bits)
//       = 2 stop bits (6-8 data bits)
#define HID_UART_SHORT_STOP_BIT						0x00
#define HID_UART_LONG_STOP_BIT						0x01

// Flow Control
#define HID_UART_NO_FLOW_CONTROL					0x00
#define HID_UART_RTS_CTS_FLOW_CONTROL				0x01

// Read/Write Limits
#define HID_UART_MIN_READ_SIZE						1
#define HID_UART_MAX_READ_SIZE						32768
#define HID_UART_MIN_WRITE_SIZE						1
#define HID_UART_MAX_WRITE_SIZE						4096

/////////////////////////////////////////////////////////////////////////////
// User Customization Definitions
/////////////////////////////////////////////////////////////////////////////

// User-Customizable Field Lock Bitmasks
/// @defgroup HID_UART_LOCK_BITMASKS
/// @{
#define HID_UART_LOCK_PRODUCT_STR_1					0x0001
#define HID_UART_LOCK_PRODUCT_STR_2					0x0002
#define HID_UART_LOCK_SERIAL_STR					0x0004
#define HID_UART_LOCK_PIN_CONFIG					0x0008
#define HID_UART_LOCK_VID							0x0100
#define HID_UART_LOCK_PID							0x0200
#define HID_UART_LOCK_POWER							0x0400
#define HID_UART_LOCK_POWER_MODE					0x0800
#define HID_UART_LOCK_RELEASE_VERSION				0x1000
#define HID_UART_LOCK_FLUSH_BUFFERS					0x2000
#define HID_UART_LOCK_MFG_STR_1						0x4000
#define HID_UART_LOCK_MFG_STR_2						0x8000
/// @}

// Field Lock Bit Values
#define HID_UART_LOCK_UNLOCKED						1
#define HID_UART_LOCK_LOCKED						0

// Power Max Value (500 mA)
#define HID_UART_BUS_POWER_MAX						0xFA

// Power Modes
#define HID_UART_BUS_POWER							0x00
#define HID_UART_SELF_POWER_VREG_DIS				0x01
#define HID_UART_SELF_POWER_VREG_EN					0x02

// Flush Buffers Bitmasks
#define HID_UART_FLUSH_TX_OPEN						0x01
#define HID_UART_FLUSH_TX_CLOSE						0x02
#define HID_UART_FLUSH_RX_OPEN						0x04
#define HID_UART_FLUSH_RX_CLOSE						0x08

// USB Config Bitmasks
#define HID_UART_SET_VID							0x01
#define HID_UART_SET_PID							0x02
#define HID_UART_SET_POWER							0x04
#define HID_UART_SET_POWER_MODE						0x08
#define HID_UART_SET_RELEASE_VERSION				0x10
#define HID_UART_SET_FLUSH_BUFFERS					0x20

// USB Config Bit Values
#define HID_UART_SET_IGNORE							0
#define HID_UART_SET_PROGRAM						1

// String Lengths
#define HID_UART_MFG_STRLEN							62
#define HID_UART_PRODUCT_STRLEN						62
#define HID_UART_SERIAL_STRLEN						30

// HID_UART_MFG_STR
typedef char HID_UART_MFG_STR[HID_UART_MFG_STRLEN];

// HID_UART_PRODUCT_STR
typedef char HID_UART_PRODUCT_STR[HID_UART_PRODUCT_STRLEN];

// HID_UART_SERIAL_STR
typedef char HID_UART_SERIAL_STR[HID_UART_SERIAL_STRLEN];

/////////////////////////////////////////////////////////////////////////////
// Pin Definitions
/////////////////////////////////////////////////////////////////////////////

// Pin Config Modes
#define HID_UART_GPIO_MODE_INPUT					0x00
#define HID_UART_GPIO_MODE_OUTPUT_OD				0x01
#define HID_UART_GPIO_MODE_OUTPUT_PP				0x02
#define HID_UART_GPIO_MODE_FUNCTION1				0x03
#define HID_UART_GPIO_MODE_FUNCTION2				0x04

// Suspend Value Bit Values
#define HID_UART_VALUE_SUSPEND_LO					0
#define HID_UART_VALUE_SUSPEND_HI					1

// Suspend Mode Bit Values
#define HID_UART_MODE_SUSPEND_OD					0
#define HID_UART_MODE_SUSPEND_PP					1

// RS485 Active Levels
#define HID_UART_MODE_RS485_ACTIVE_LO				0x00
#define HID_UART_MODE_RS485_ACTIVE_HI				0x01

/////////////////////////////////////////////////////////////////////////////
// Typedefs
/////////////////////////////////////////////////////////////////////////////

typedef void* HID_UART_DEVICE;
typedef unsigned char U8;
typedef unsigned short U16;

/////////////////////////////////////////////////////////////////////////////
// Exported Library Functions
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// HidUart_GetNumDevices
/// @brief Determines the number of devices connected to the system with matching VID/PID
///
/// @param[out] lpdwNumDevices a pointer to a uint32_t location to hold the returned device count
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetNumDevices(_Out_writes_bytes_(sizeof(DWORD)) _Pre_defensive_ LPDWORD lpdwNumDevices, _In_ _Pre_defensive_ const WORD vid, _In_ _Pre_defensive_ const WORD pid);

// HidUart_GetString
/// @brief This function returns a null-terminated vendor ID string, product ID string, serial string, device path
/// string, manufacturer string, or product string for the device specified by an index passed in deviceNum.
/// 
/// @detail The index for the first device is 0 and the last device is the value returned by @ref HidUart_GetNumDevices() - 1.
///
/// @param[in] deviceNum Index of the device for which the string is desired.
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
/// @param[out] deviceString - Variable of type HID_UART_DEVICE_STRING which will contain a NULL terminated
/// ASCII device string on return.The string is 260 bytes on Windows and 512 bytes on macos
/// and Linux.
/// @param[in] options - Determines if deviceString contains a vendor ID string, product ID string, serial string,
/// device path string, manufacturer string, or product string. See @ref GetStringOptions
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetString(_In_ _Pre_defensive_ const DWORD deviceNum, _In_ _Pre_defensive_ const WORD Vid, _In_ _Pre_defensive_ const WORD Pid, char* deviceString, _In_ _Pre_defensive_ const DWORD options);

// HidUart_GetOpenedString
/// @brief This function returns a null-terminated vendor ID string, product ID string, serial string, device path
/// string, manufacturer string, or product string for the device specified by device.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[out] deviceString - Variable of type HID_UART_DEVICE_STRING which will contain a NULL terminated
/// ASCII device string on return.The string is 260 bytes on Windows and 512 bytes on macos
/// and Linux.
/// @param[in] options - Determines if deviceString contains a vendor ID string, product ID string, serial string,
/// device path string, manufacturer string, or product string. See @ref GetStringOptions
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetOpenedString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, char* deviceString, _In_ _Pre_defensive_ const DWORD options);

// HidUart_GetIndexedString
/// @brief This function returns a null-terminated USB string descriptor for the device specified by an index
/// passed in deviceNum. (Windows/Linux only)
///
/// @param[in] deviceNum Index of the device for which the string is desired.
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
/// @param[in] stringIndex - Specifies the device-specific index of the USB string descriptor to return.
/// @param[out] deviceString - Variable of type @ref HID_UART_DEVICE_STRING which will contain a NULL terminated
/// device descriptor string on return.The string is 260 bytes on Windows and 512 bytes on Linux.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetIndexedString(_In_ _Pre_defensive_ const DWORD deviceNum, _In_ _Pre_defensive_ const WORD vid, _In_ _Pre_defensive_ const WORD pid, _In_ _Pre_defensive_ const DWORD stringIndex, char* deviceString);

// HidUart_GetOpenedIndexedString
/// @brief This function returns a null-terminated USB string descriptor for the device specified by device. (Windows/Linux only)
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] stringIndex - Specifies the device-specific index of the USB string descriptor to return.
/// @param[out] deviceString - Variable of type @ref HID_UART_DEVICE_STRING which will contain a NULL terminated
/// device descriptor string on return.The string is 260 bytes on Windows and 512 bytes on Linux.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetOpenedIndexedString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const DWORD stringIndex, char* deviceString);

// HidUart_GetAttributes
/// @brief This function returns the device vendor ID, product ID, and release number for the device specified
/// by an index passed in deviceNum.
///
/// @param[in] deviceNum Index of the device for which the string is desired.
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
/// @param[out] deviceVid - returns the 2-byte Vendor ID value from the device.
/// @param[out] devicePid - returns the 2-byte Product ID value from the device.
/// @param[out] deviceReleaseNumber - returns the 2-byte USB device release number in binary-coded decimal from the device.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetAttributes(_In_ _Pre_defensive_ const DWORD deviceNum, _In_ _Pre_defensive_ const WORD Vid, _In_ _Pre_defensive_ const WORD Pid, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* deviceVid, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* devicePid, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* deviceReleaseNumber);

// HidUart_GetOpenedAttributes
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[out] deviceVid - returns the 2-byte Vendor ID value from the device.
/// @param[out] devicePid - returns the 2-byte Product ID value from the device.
/// @param[out] deviceReleaseNumber - returns the 2-byte USB device release number in binary-coded decimal from the device.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetOpenedAttributes(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* deviceVid, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* devicePid, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* deviceReleaseNumber);

// HidUart_Open
/// @brief Opens a device using a device number between 0 and @ref HidUart_GetNumDevices() - 1, enables the
/// UART, and returns a device object pointer which will be used for subsequent accesses.
///
/// @param[in] device - Returns a pointer to a HID-to-UART device objectsubsequent accesses to the device.
/// @param[in] deviceNum - Zero-based device index, between 0 and (@ref HidUart_GetNumDevices() - 1).
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @note Be careful when opening a device. Any HID device may be opened by this library. However, if the
/// device is not actually a CP211x, use of this library will cause undesirable results. The best course of
/// action would be to designate a unique VID/PID for CP211x devices only. The application should then
/// filter devices using this VID/PID.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_Open(_Out_ HID_UART_DEVICE* device, _In_ _Pre_defensive_ const DWORD deviceNum, _In_ _Pre_defensive_ const WORD vid, _In_ _Pre_defensive_ const WORD pid);

// HidUart_Close
/// @brief Closes an opened device using the device object pointer provided by @ref HidUart_Open().
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
///
/// @note The device parameter is invalid after calling @ref HidUart_Close(). Set device to NULL after calling @ref HidUart_Close().
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_Close(_In_ _Pre_defensive_ const HID_UART_DEVICE device);

// HidUart_IsOpened
/// @brief Returns the device opened status.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[out] pbIsOpened - Returns TRUE if the device object pointer is valid and the device has been opened using @ref HidUart_Open().
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- pbIsOpened is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_IsOpened(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(BOOL)) _Pre_defensive_ BOOL* pbIsOpened);

// HidUart_SetUartEnable
/// @brief Enables or disables the UART.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] enable - Set to TRUE to enable the UART
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetUartEnable(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const BOOL enable);

// HidUart_GetUartEnable
/// @brief Returns the UART enable status.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[out] enable - Returns TRUE if the UART is enabled.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetUartEnable(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(BOOL)) _Pre_defensive_ BOOL* enable);

// HidUart_Read
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_Read(_In_ _Pre_defensive_ const HID_UART_DEVICE device, BYTE* buffer, _In_ _Pre_defensive_ const DWORD numBytesToRead, _Out_writes_bytes_(sizeof(DWORD)) _Pre_defensive_ DWORD* numBytesRead);

// HidUart_Write
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_Write(_In_ _Pre_defensive_ const HID_UART_DEVICE device, BYTE* buffer, _In_ _Pre_defensive_ const DWORD numBytesToWrite, _Out_writes_bytes_(sizeof(DWORD)) _Pre_defensive_ DWORD* numBytesWritten);

// HidUart_FlushBuffers
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_FlushBuffers(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const BOOL flushTransmit, _In_ _Pre_defensive_ const BOOL flushReceive);

// HidUart_CancelIo
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_CancelIo(_In_ _Pre_defensive_ const HID_UART_DEVICE device);

// HidUart_SetTimeouts
/// @brief Sets the read and write timeouts. Timeouts are used for HidUart_Read() and HidUart_Write(). The
/// default value for timeouts is 1000 ms, but timeouts can be set to wait for any number of milliseconds
/// between 0 and 0xFFFFFFFF.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetTimeouts(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const DWORD readTimeout, _In_ _Pre_defensive_ const DWORD writeTimeout);

// HidUart_GetTimeouts
/// @brief Returns the current read and write timeouts specified in milliseconds
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[out] readTimeout - the @ref HidUart_Read() operation timeout in milliseconds.
/// @param[out] writeTimeout - the @ref HidUart_Write() operation timeout in milliseconds.
///
/// @note Read and write timeouts are maintained for each device but are not persistent across @ref HidUart_Open() / @ref HidUart_Close().
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- readTimeout and/or writeTimeout is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetTimeouts(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(DWORD)) _Pre_defensive_ DWORD* readTimeout, _Out_writes_bytes_(sizeof(DWORD)) _Pre_defensive_ DWORD* writeTimeout);

// HidUart_GetUARTStatus
/// @brief Returns the number of bytes held in the device receive and transmit FIFO. Returns the parity/error
/// status and line break status.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[out] transmitFifoSize - Returns the number of bytes currently held in the device transmit FIFO.
/// @param[out] receiveFifoSize - Returns the number of bytes currently held in the device receive FIFO.
/// @param[out] errorStatus - Returns an error status bitmap describing parity and overrun errors function clears the errors.
/// @param[out] lineBreakStatus - Returns 0x01 if line break is currently active and 0x00 otherwise.
///
/// @note The transmitFifoSize and receiveFifoSize only apply to data held in the device FIFOs; they do not include
/// data queued in the HID driver or interface library
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- transmitFifoSize, and/or receiveFifoSize, and/or errorStatus, and/or lineBreakStatus is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetUartStatus(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* transmitFifoSize, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* receiveFifoSize, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* errorStatus, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* lineBreakStatus);

// HidUart_SetUARTConfig
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetUartConfig(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const DWORD baudRate, _In_ _Pre_defensive_ const BYTE dataBits, _In_ _Pre_defensive_ const BYTE parity, _In_ _Pre_defensive_ const BYTE stopBits, _In_ _Pre_defensive_ const BYTE flowControl);

// HidUart_GetUARTConfig
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetUartConfig(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(DWORD)) _Pre_defensive_ DWORD* baudRate, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* dataBits, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* parity, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* stopBits, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* flowControl);

// HidUart_StartBreak
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_StartBreak(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const BYTE duration);

// HidUart_StopBreak
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_StopBreak(_In_ _Pre_defensive_ const HID_UART_DEVICE device);

// HidUart_Reset
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_Reset(_In_ _Pre_defensive_ const HID_UART_DEVICE device);

// HidUart_ReadLatch
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_ReadLatch(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* latchValue);

// HidUart_WriteLatch
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_WriteLatch(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const WORD latchValue, _In_ _Pre_defensive_ const WORD latchMask);

// HidUart_GetPartNumber
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetPartNumber(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* partNumber, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* version);

// HidUart_GetLibraryVersion
/// @brief tbd
///
/// @param[out] lpdwNumDevices a pointer to a uint32_t location to hold the returned device count
/// @param[out] Vid is the 2-byte Vendor ID value.
/// @param[out] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetLibraryVersion(_Out_writes_bytes_(1) BYTE* major, _Out_writes_bytes_(1) BYTE* minor, _Out_writes_bytes_(sizeof(BOOL)) BOOL* release);

// HidUart_GetHidLibraryVersion
/// @brief tbd
///
/// @param[out] lpdwNumDevices a pointer to a uint32_t location to hold the returned device count
/// @param[out] Vid is the 2-byte Vendor ID value.
/// @param[out] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetHidLibraryVersion(_Out_writes_bytes_(1) BYTE* major, _Out_writes_bytes_(1) BYTE* minor, _Out_writes_bytes_(sizeof(BOOL)) BOOL* release);

// HidUart_GetHidGuid
/// @brief tbd
///
/// @param[out] lpdwNumDevices a pointer to a uint32_t location to hold the returned device count
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetHidGuid(_Out_ void* guid);

// HidUart_SetLock
/// @brief Permanently locks/disables device customization
///
/// @detail When this function is successfully called, the specified fields are fully locked and cannot be further
/// customized.The user customization functions can be called and may return HID_UART_SUCCESS
/// even though the device was not programmed.Call the function’s corresponding get function to verify
/// that customization was successful.Each field is stored in one time programmable memory(OTP) and
/// can only be customized once.After a field is customized, the corresponding lock bits are set to 0.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] lock - Bitmask specifying which fields can be customized/programmed and which fields are already customized.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetLock(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ _Pre_defensive_ const WORD lock);

// HidUart_GetLock
/// @brief Returns the device customization lock status.
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[out] lock - Returns a bitmask specifying which fields are locked. See @ref HID_UART_LOCK_BITMASKS.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lock is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetLock(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* lock);

// HidUart_SetUsbConfig
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetUsbConfig(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _In_ const WORD vid, _In_ const WORD pid, _In_ const BYTE power, _In_ const BYTE powerMode, _In_ const WORD releaseVersion, _In_ const BYTE flushBuffers, _In_ const BYTE mask);

// HidUart_GetUsbConfig
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetUsbConfig(_In_ _Pre_defensive_ const HID_UART_DEVICE device, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* vid, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* pid, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* power, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* powerMode, _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ WORD* releaseVersion, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* flushBuffers);

// HidUart_SetManufacturingString
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetManufacturingString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, char* manufacturingString, _In_ _Pre_defensive_ const BYTE strlen);

// HidUart_GetManufacturingString
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetManufacturingString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, char* manufacturingString, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* strlen);

// HidUart_SetProductString
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetProductString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, char* productString, _In_ _Pre_defensive_ const BYTE strlen);

// HidUart_GetProductString
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetProductString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, char* productString, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* strlen);

// HidUart_SetSerialString
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_SetSerialString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, char* serialString, _In_ _Pre_defensive_ const BYTE strlen);

// HidUart_GetSerialString
/// @brief tbd
///
/// @param[in] device is the Device object pointer as returned by @ref HidUart_Open().
/// @param[in] Vid is the 2-byte Vendor ID value.
/// @param[in] Pid is the 2-byte Product ID value.
///
/// @returns Returns #HID_UART_SUCCESS on success, or another @ref HID_UART_STATUS Return value if there is an error.
///	@retval	#HID_UART_SUCCESS -- success
///	@retval	#HID_UART_INVALID_DEVICE_OBJECT -- device is not a valid, recognized Device object
///	@retval	#HID_UART_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(HID_UART_SUCCESS, HID_UART_UNKNOWN_ERROR)
_Success_(return == HID_UART_SUCCESS)
HID_TO_UART_API HID_UART_STATUS WINAPI
HidUart_GetSerialString(_In_ _Pre_defensive_ const HID_UART_DEVICE device, char* serialString, _Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ BYTE* strlen);

#ifdef __cplusplus
}
#endif // __cplusplus

/// @}

#endif // SLAB_HID_TO_UART_H
