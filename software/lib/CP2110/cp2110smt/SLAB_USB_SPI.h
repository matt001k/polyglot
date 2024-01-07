//-----------------------------------------------------------------------------
// SLAB_USB_SPI.h
//-----------------------------------------------------------------------------
// Copyright 2012-2017 Silicon Laboratories Inc.
// http://www.silabs.com
//-----------------------------------------------------------------------------

/// @file SLAB_USB_SPI.h
/// This file defines the API of the CP2130 Interface Library

#ifndef HOST_LIB_SLABUSBSPI_INCLUDE_SLAB_USB_SPI_H_INCLUDED_9QK7BKA9FX
#define HOST_LIB_SLABUSBSPI_INCLUDE_SLAB_USB_SPI_H_INCLUDED_9QK7BKA9FX

/// @addtogroup slabusbspi CP2130 SPI interface Library
///
/// @{

#include	"silabs_defs.h"
#include	"silabs_sal.h"
#include	"Types.h"

#ifdef _WIN32
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SI_USB_XP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SI_USB_XP_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SLAB_USB_SPI_EXPORTS
#define SLAB_USB_SPI_API
#else
#define SLAB_USB_SPI_API     __declspec(dllimport)
#pragma comment (lib, "SLAB_USB_SPI.lib")
#endif // USB_SPI_EXPORTS
#else // !_WIN32
#define SLAB_USB_SPI_API
#define WINAPI
#endif // !_WIN32


/////////////////////////////////////////////////////////////////////////////
// Definitions - Windows Definitions
/////////////////////////////////////////////////////////////////////////////
/// @name Windows Definitions
/// @{

#define MAX_PATH            260
#define CALLBACK            __stdcall
#ifndef WINAPI
#define WINAPI              __stdcall
#endif
/// @}

/////////////////////////////////////////////////////////////////////////////
// Variable types
/////////////////////////////////////////////////////////////////////////////
/// CP213X_DEVICE opaque object
typedef void*                       CP213x_DEVICE;	// TODO: casting as a void * prevents the compiler from being able to help us find type-mismatches

/////////////////////////////////////////////////////////////////////////////
// Definitions specific to USB SPI code
/////////////////////////////////////////////////////////////////////////////
/// @name Definitions specific to USB SPI code
/// @{

#define BULK_PACKETSIZE             64
#define EP_BUFSIZE                  BULK_PACKETSIZE
#define CMD_SIZE                    8
#define CMD_TIMEOUT_MS              1000        // Timeout for completion of SPI command (i.e. first packet)
#define STRING_DESCRIPTOR_SIZE      256
#define SIZE_PIN_CONFIG             20


// Maximum block size for WriteRead
#define WRITEREAD_MAX_BLOCKSIZE     256         // Matches size of firmware Read buffer

// Number of GPIO/CS pins
#define    CP213x_NUM_GPIO          11

// Chip Select Mode:  0: Idle, 1: Active, 2: Active; all other channels idle
#define CSMODE_IDLE                 0
#define CSMODE_ACTIVE               1
#define CSMODE_ACTIVE_OTHERS_IDLE   2

// Mode definitions for GPIO/CS/SpecialFunction pins
#define GPIO_MODE_INPUT                 0
#define GPIO_MODE_OUTPUT_OD             1
#define GPIO_MODE_OUTPUT_PP             2
#define GPIO_MODE_CHIPSELECT            3       // Chip select mode

#define GPIO_MODE__RTR_ACTLOW           4
#define GPIO_MODE__RTR_ACTHIGH          5

#define GPIO_MODE__EVTCNTR_RISING       4
#define GPIO_MODE__EVTCNTR_FALLING      5
#define GPIO_MODE__EVTCNTR_NEGPULSE     6
#define GPIO_MODE__EVTCNTR_POSPULSE     7

#define GPIO_MODE__CLK_OUT              4
#define GPIO_MODE__SPI_ACTIVITY         4
#define GPIO_MODE__SUSPEND              4
#define GPIO_MODE__SUSPENDBAR           4

//  End of GPIO Mode definitions


// String Buffer Definitions
#define SHORT_STRING_LEN            60
#define LONG_STRING_LEN_1           61
#define LONG_STRING_LEN_2           63
/// @}

////////////////////////////////////////////////////////////////////////////////
// SPI Command and Subcommand definitions
////////////////////////////////////////////////////////////////////////////////

/// @name SPI Data Transfer command and subcommands
/// @{
#define CMD_TRANSFER_DATA           0x0000
    // Subcommand definitions for TRANSFER_DATA command
    #define SUBCMD_READ             0
    #define SUBCMD_WRITE            1
    #define SUBCMD_WRITEREAD        2
    #define SUBCMD_RTREAD           4
    #define SUBCMD_MSB_RELEASE_BUS  0x80
/// @}
_Check_return_
_Success_(return == TRUE)
static inline BOOL IsValidSubCmd(_In_ const BYTE SubCmd) {return (SUBCMD_READ == SubCmd) || (SUBCMD_WRITE == SubCmd) || (SUBCMD_WRITEREAD == SubCmd) || (SUBCMD_RTREAD == SubCmd);}

/// @name SPI Control Word bitfield definitions
/// @{
// Bits 7-6: Not assigned

// Bit 5: Clock phase (CPHA)
#define SPICTL_CPHA_SHIFT           5
#define SPICTL_CPHA_MASK            0x20
#define SPICTL_CPHA_LEADING_EDGE    0
#define SPICTL_CPHA_TRAILING_EDGE   1

// Bit 4: Clock polarity (CPOL)
#define SPICTL_CPOL_SHIFT           4
#define SPICTL_CPOL_MASK            0x10
#define SPICTL_CPOL_ACTIVE_HIGH     0
#define SPICTL_CPOL_ACTIVE_LOW      1

// Bit 3: Chip Select Mode
#define SPICTL_CSMODE_SHIFT         3
#define SPICTL_CSMODE_MASK          0x08
#define SPICTL_CSMODE_OPENDRAIN     0
#define SPICTL_CSMODE_PUSHPULL      1

// Bit 2-0: Clock rate
#define SPICTL_CLKRATE_SHIFT        0
#define SPICTL_CLKRATE_MASK         0x07
#define SPICTL_CLKRATE_12M          0       // 12 MHz
#define SPICTL_CLKRATE_6M           1       // 6 MHz
#define SPICTL_CLKRATE_3M           2       // 3 MHz
#define SPICTL_CLKRATE_1M5          3       // 1.5 MHz
#define SPICTL_CLKRATE_750K         4       // 750 kHz
#define SPICTL_CLKRATE_375K         5       // 375 kHz
#define SPICTL_CLKRATE_187K5        6       // 187.5 kHz
#define SPICTL_CLKRATE_93K75        7       // 93.75 kHz
/// @}

/////////////////////////////////////////////////////////////////////////////
// Error Definitions
/////////////////////////////////////////////////////////////////////////////

/// @name Return values for BOOL-returning functions
/// @{
#define bRESULT_PASS    TRUE
#define bRESULT_FAIL    FALSE
/// @}

/// API Status return codes
typedef enum _USB_SPI_STATUS {
/// @name CP2130 General Errors
/// @{

	USB_SPI_ERRCODE_SUCCESS							= SILABS_STATUS_SUCCESS	///< The function returned successfully.
	, USB_SPI_ERRCODE_ALLOC_FAILURE                 =  0x01    ///< Allocation error.
	, USB_SPI_ERRCODE_INVALID_ENUM_VALUE            =  0x02    ///< Invalid enumeration value.
	, USB_SPI_ERRCODE_NULL_POINTER                  =  0x03    ///< Null pointer.
	, USB_SPI_ERRCODE_INVALID_CHANNEL_INDEX         =  0x04    ///< Channel index is not in range.
	, USB_SPI_ERRCODE_INVALID_GPIO_MODE             =  0x05    ///< Mode != INPUT, OUTPUT_OD, or OUTPUT_PP.

	, USB_SPI_ERRCODE_UNKNOWN_ERROR                 =  0xFFFFFFFF  ///< Unknown/unspecified error.  TODO: No, not all-F's, how about 0xFF?
/// @}

//
// API Errors
//
/// @name CP2130 API Errors
/// @{
	, USB_SPI_ERRCODE_INVALID_PARAMETER              = 0x10    ///< Invalid function parameter.

	, USB_SPI_ERRCODE_INVALID_DEVICE_OBJECT          = 0x11    ///< The specified device object pointer is invalid.
/// @}

//
// Device Errors
//
/// @name CP2130 Device Errors
/// @{
	, USB_SPI_ERRCODE_DEVICE_NOT_FOUND               = 0x20	///< The specified USB device could not be found.
	, USB_SPI_ERRCODE_USB_DEVICE_NOT_OPENED          = 0x21	///< The current USB device is not opened.
	, USB_SPI_ERRCODE_INVALID_HANDLE                 = 0x22	///< The handle is invalid.
/// @}

//
// Device Hardware Interface Errors
//
/// @name CP2130 Device Hardware Interface Errors
/// @{

	, USB_SPI_ERRCODE_HWIF_DEVICE_ERROR              = 0x30	///< An error occurred while communicating with the device or while retrieving device information.


	, USB_SPI_ERRCODE_HWIF_TRANSFER_TIMEOUT          = 0x31	///< A control transfer operation timed out.
/// @}

/// @name CP2130 Data Transfer Errors
/// @{

	, USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR         = 0x50
	, USB_SPI_ERRCODE_INVALID_TRANSFER_SIZE          = 0x51

	, USB_SPI_ERRCODE_PIPE_INIT_FAIL                 = 0x60
	, USB_SPI_ERRCODE_PIPE_QUERY_FAIL                = 0x61
	, USB_SPI_ERRCODE_PIPE_WRITE_FAIL                = 0x62
	, USB_SPI_ERRCODE_PIPE_READ_FAIL                 = 0x63
	, USB_SPI_ERRCODE_PIPE_ABORT_FAIL                = 0x64
	, USB_SPI_ERRCODE_PIPE_FLUSH_FAIL                = 0x65
	, USB_SPI_ERRCODE_PIPE_INVALID_ID                = 0x66

	, USB_SPI_ERRCODE_READ_THREAD_CREATE_FAILURE     = 0x70
	, USB_SPI_ERRCODE_READ_THREAD_NOT_RUNNING        = 0x71
	, USB_SPI_ERRCODE_READ_THREAD_START_FAILURE      = 0x72

	, USB_SPI_ERRCODE_DEVICE_RETURNED_TOO_MUCH_DATA  = 0x80

	, USB_SPI_ERRCODE_SYSTEM_ERROR						= 0xFFFFFFFE
/// @}

} USB_SPI_STATUS, *PUSB_SPI_STATUS;

/// @name CP213x Customization Masks
/// @{
// User-Customizable Field Lock Bitmasks
#define CP213x_LOCK_PRODUCT_STR_1           0x0001
#define CP213x_LOCK_PRODUCT_STR_2           0x0002
#define CP213x_LOCK_SERIAL_STR              0x0004
#define CP213x_LOCK_PIN_CONFIG              0x0008

#define CP213x_LOCK_VID                     0x0100
#define CP213x_LOCK_PID                     0x0200
#define CP213x_LOCK_POWER                   0x0400
#define CP213x_LOCK_POWER_MODE              0x0800
#define CP213x_LOCK_RELEASE_VERSION         0x1000
#define CP213x_LOCK_MFG_STR_1               0x2000
#define CP213x_LOCK_MFG_STR_2               0x4000
#define CP213x_LOCK_TRANSFER_PRIORITY       0x8000

// USB Config Bitmasks
#define CP213x_SET_VID                      0x01
#define CP213x_SET_PID                      0x02
#define CP213x_SET_POWER                    0x04
#define CP213x_SET_POWER_MODE               0x08
#define CP213x_SET_RELEASE_VERSION          0x10
#define CP213x_SET_TRANSFER_PRIORITY        0x80

// String Lengths
#define MFG_STRLEN          62
#define PRODUCT_STRLEN      62
#define SERIAL_STRLEN       30
// MFG_STR
typedef char MFG_STR[MFG_STRLEN];
// PRODUCT_STR
typedef char PRODUCT_STR[PRODUCT_STRLEN];
// SERIAL_STR
typedef char SERIAL_STR[SERIAL_STRLEN];

/////////////////////////////////////////////////////////////////////////////
// Enumerations
/////////////////////////////////////////////////////////////////////////////

enum SET_STATUS {SET_SUCCESS, SET_FAIL, SET_VERIFY_FAIL};

/////////////////////////////////////////////////////////////////////////////
// Pin Definitions
/////////////////////////////////////////////////////////////////////////////

// Pin Config Mode Array Indices
#define CP213x_INDEX_GPIO_0         0
#define CP213x_INDEX_GPIO_1         1
#define CP213x_INDEX_GPIO_2         2
#define CP213x_INDEX_GPIO_3         3
#define CP213x_INDEX_GPIO_4         4
#define CP213x_INDEX_GPIO_5         5
#define CP213x_INDEX_GPIO_6         6
#define CP213x_INDEX_GPIO_7         7
#define CP213x_INDEX_GPIO_8         8
#define CP213x_INDEX_GPIO_9         9
#define CP213x_INDEX_GPIO_10        10

// Pin Bitmasks
#define CP213x_MASK_SCK             0x0001
#define CP213x_MASK_MISO            0x0002
#define CP213x_MASK_MOSI            0x0004
#define CP213x_MASK_GPIO_0          0x0008
#define CP213x_MASK_GPIO_1          0x0010
#define CP213x_MASK_GPIO_2          0x0020
#define CP213x_MASK_GPIO_3          0x0040
#define CP213x_MASK_GPIO_4          0x0080
#define CP213x_MASK_GPIO_5          0x0100
#define CP213x_MASK_GPIO_6          0x0400
#define CP213x_MASK_GPIO_7          0x0800
#define CP213x_MASK_GPIO_8          0x1000
#define CP213x_MASK_GPIO_9          0x2000
#define CP213x_MASK_GPIO_10         0x4000

/// @}

////////////////////////////////////////////////////////////////////////////////
// Structures
////////////////////////////////////////////////////////////////////////////////

/// @name Type and structure definitions
/// @{

/// This struct has the same format as _USB_DEVICE_DESCRIPTOR, as defined in Windows usb100.h.
/// Using this typedef enables portable application code.
typedef struct _DEVICE_DESCRIPTOR
{
    BYTE    bLength;
    BYTE    bDescriptorType;
    WORD    bcdUSB;
    BYTE    bDeviceClass;
    BYTE    bDeviceSubClass;
    BYTE    bDeviceProtocol;
    BYTE    bMaxPacketSize0;
    WORD    idVendor;
    WORD    idProduct;
    WORD    bcdDevice;
    BYTE    iManufacturer;
    BYTE    iProduct;
    BYTE    iSerialNumber;
    BYTE    bNumConfigurations;
} DEVICE_DESCRIPTOR, *PDEVICE_DESCRIPTOR;

/// SPI Command Word
typedef struct _SPI_CMD
{
    WORD   Cmd;
    WORD   SubCmd;
    DWORD  Len;
    DWORD  blockSize;
    DWORD  timeoutMs;
    DWORD  res;
} SPI_CMD, *PSPI_CMD;


// Bitfield masks for delay_mode structure element
#define SPI_INTERBYTE_DELAY_MASK        0x01
#define SPI_CS_POSTASSERT_DELAY_MASK    0x02
#define SPI_CS_PREDEASSERT_DELAY_MASK   0x04
#define SPI_CS_TOGGLE_MASK              0x08

/// @}

/////////////////////////////////////////////////////////////////////////////
// Exported API Functions
/////////////////////////////////////////////////////////////////////////////

/// @name Exported API Functions
/// @{

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	///
	/// @brief what it does in brief
	///
	/// @details what it does in detail
	///
	/// @param[in] hDevice USB Interface Handle
	///
	/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetLibraryVersion (_Out_writes_bytes_opt_(1) BYTE* major, _Out_writes_bytes_opt_(1) BYTE* minor, _Out_writes_bytes_opt_(1) BOOL* release);

//
//  CP2130 Device Management
//
/// @name CP2130 Device Management
/// @{

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetGuid                  ( _Out_ LPGUID guid );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetGuid                  ( LPGUID guid );

#ifdef _WIN32
///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetNumDevices            ( _Out_writes_bytes_(4/*sizeof DWORD*/) DWORD* numDevices );
#else
USB_SPI_STATUS
CP213x_GetNumDevices ( DWORD* numDevices, DWORD VID, DWORD PID );
#endif
    
///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetDevicePath(_In_ const DWORD deviceIndex, _Out_writes_bytes_(MAX_PATH) LPSTR path);

#ifdef _WIN32
///
/// @brief Opens a USB device using a device index and returns a
/// device object pointer which will be used for subsequent access.
///
/// @details
///
/// @param[in] deviceIndex Index of device to open
/// @param[out] device Returns a pointer to an opaque device object @ref CP213x_DEVICE used for subsequent device access.
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval USB_SPI_ERRCODE_INVALID_DEVICE_OBJECT
/// @retval USB_SPI_ERRCODE_INVALID_PARAMETER
///
/// @note ::CP213x_GetNumDevices() must be called prior to calling this function
/// @in order to initialize the library's internal device list.
///
/// @sa ::CP213x_GetNumDevices()
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_OpenByIndex              ( DWORD deviceIndex,
                                  CP213x_DEVICE* phDevice );

    
///
/// @brief Opens a USB device using a specified Device Path and returns a
/// device object pointer which will be used for subsequent access.
///
/// @details
///
/// @param[in] devicePath The specified Device Path
/// @param[out] phDevice Returns a pointer to an opaque device object @ref CP213x_DEVICE used for subsequent device access. 
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_INVALID_DEVICE_OBJECT
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_OpenByDevicePath         ( LPCSTR devicePath,
                                  CP213x_DEVICE* phDevice );

#else
    
USB_SPI_STATUS CP213x_Open(DWORD deviceIndex, CP213x_DEVICE* phDevice, DWORD VID, DWORD PID );

#endif

    
///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetVidPid(_In_ const DWORD deviceIndex, _Out_writes_bytes_(4/*sizeof DWORD*/) WORD* vid, _Out_writes_bytes_(4/*sizeof DWORD*/) WORD* pid );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetOpenedDevicePath      (_In_ const CP213x_DEVICE device, LPSTR path );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetOpenedVidPid(_In_ const CP213x_DEVICE device, _Out_writes_bytes_opt_(2) WORD* vid, _Out_writes_bytes_opt_(2) WORD* pid);

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_Close                    (_In_ const CP213x_DEVICE hDevice );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
SLAB_USB_SPI_API BOOL WINAPI
CP213x_IsOpened                 (_In_ const CP213x_DEVICE hDevice);

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_Reset                    (_In_ const CP213x_DEVICE hDevice );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetDeviceVersion         (_In_ const CP213x_DEVICE hDevice, _Out_writes_bytes_opt_(1) BYTE* majorVersion, _Out_writes_bytes_opt_(1) BYTE* minorVersion );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetDeviceDescriptor      (_In_ const CP213x_DEVICE hDevice,
                                  PDEVICE_DESCRIPTOR pDescriptor );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetStringDescriptor      (_In_ const CP213x_DEVICE hDevice,
                                  BYTE index,
                                  BYTE stringDescriptor[STRING_DESCRIPTOR_SIZE] );

/// @brief Get the USB Device Configuration info
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[out] vid VID of the device
/// @param[out] pid PID of the device
/// @param[out] power Power of the device
/// @param[out] powerMode Power Mode of the device
/// @param[out] releaseVersion Release version of the device
/// @param[out] transferPriority Transfer priority of the device
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE --
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetUsbConfig             (_In_ const CP213x_DEVICE hDevice,
								 _Out_writes_bytes_opt_(2) WORD* vid,
								 _Out_writes_bytes_opt_(2) WORD* pid,
								 _Out_writes_bytes_opt_(1) BYTE* pMaxPower,
								 _Out_writes_bytes_opt_(1) BYTE* pPowerMode,
								 _Out_writes_bytes_opt_(2) WORD* releaseVersion,
								 _Out_writes_bytes_opt_(1) BYTE* transferPriority );

///
/// @brief Set the USB device Configuration info
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[in] vid New VID to be set in the device
/// @param[in] pid New PID to be set in the device
/// @param[in] power New Power to be set in the device
/// @param[in] powerMode New Power Mode to be set in the device
/// @param[in] releaseVersion New Release version to be set in the device
/// @param[in] transferPriority New Transfer priority to be set in the device
/// @param[in] mask Mask that represents which of the settings to write to the device
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
/// 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetUsbConfig             (_In_ const CP213x_DEVICE hDevice,
								 _In_ const WORD vid,
								 _In_ const WORD pid,
								 _In_ const BYTE MaxPower,
								 _In_ const BYTE PowerMode,
								 _In_ const WORD releaseVersion,
								 _In_ const BYTE transferPriority,
								 _In_ const BYTE mask );

///
/// @brief Get the USB device Manufacturer string
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[out] manufacturingString Pointer to buffer that will be populated with Manufacturer string
/// @param[out] strlen Pointer to BYTE that will be set to the Manufacturer string length
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetManufacturingString   (_In_ const CP213x_DEVICE hDevice, LPSTR manufacturingString, _Out_writes_bytes_opt_(1) BYTE* manufacturingStringLen);

///
/// @brief Set the USB device Manufacturer string
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[in] manufacturingString Buffer containing the USB device Manufacturer string to be set
/// @param[in] strlen The length of the supplied Manufacturer string
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetManufacturingString   (_In_ const CP213x_DEVICE hDevice, LPCSTR manufacturingString, _In_ const BYTE manufacturingStringlen );

///
/// @brief Get the USB device Product string
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[out] productString Buffer that will be populated with the USB device Product string
/// @param[out] strlen Pointer to BYTE that will be set to the Product string length
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetProductString(_In_ const CP213x_DEVICE hDevice, LPSTR productString, _Out_writes_bytes_opt_(1) BYTE* productStringLen);

///
/// @brief Set the USB device Product string
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[in] productString Buffer containing the USB device Product string
/// @param[in] strlen The length of the supplied Product string
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetProductString(_In_ const CP213x_DEVICE hDevice, LPCSTR productString, _In_ const BYTE productStringlen);

///
/// @brief Get the USB device Serial Number string
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[out] serialString Buffer that will be populated with the USB device Serial Number string
/// @param[out] strlen Pointer to BYTE that will be set to the Serial Number string length
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetSerialString          (_In_ const CP213x_DEVICE hDevice, LPSTR serialString, _Out_writes_bytes_opt_(1) BYTE* serialStringlen);

///
/// @brief Set the USB device Serial Number string
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[in]  serialString Buffer containing the USB device Serial Number string
/// @param[in] strlen The length of the supplied Serial Number string
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetSerialString          (_In_ const CP213x_DEVICE hDevice, LPCSTR serialString, _In_ const BYTE serialStringlen );

///
/// @brief Get the USB device Pin Configuration info
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[out] pinConfig Buffer containing the USB device Pin Configuration info
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetPinConfig             (_In_ const CP213x_DEVICE hDevice, BYTE pinConfig[SIZE_PIN_CONFIG] );


///
/// @brief Set the USB device Pin Configuration info
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[out] pinConfig Buffer containing the USB device Pin Configuration info
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval #USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR -- 
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetPinConfig             (_In_ const CP213x_DEVICE hDevice, BYTE pinConfig[SIZE_PIN_CONFIG] );

///
/// @brief Get the Lock values
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[out] lockValue The Lock values
///
/// @returns Returns USB_SPI_ERRCODE_SUCCESS on success, or another @ref USB_SPI_STATUS Return value if there is an error.
/// @retval USB_SPI_ERRCODE_SUCCESS
/// @retval USB_SPI_ERRCODE_INVALID_HANDLE
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetLock(_In_ const CP213x_DEVICE hDevice, _Out_writes_bytes_opt_(2) WORD* lockValue);

///
/// @brief Set the Lock values
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[in] lockValue The Lock values to set
///
/// @returns Returns USB_SPI_ERRCODE_SUCCESS on success, or another @ref USB_SPI_STATUS Return value if there is an error.
/// @retval USB_SPI_ERRCODE_SUCCESS
/// @retval USB_SPI_ERRCODE_INVALID_HANDLE
/// @retval USB_SPI_ERRCODE_CONTROL_TRANSFER_ERROR
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetLock(_In_ const CP213x_DEVICE hDevice, _In_ const WORD lockValue);

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_ReadProm                 (_In_ const CP213x_DEVICE hDevice, BYTE pReadBuf[] );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_WriteProm                (_In_ const CP213x_DEVICE hDevice, BYTE pWriteBuf[] );
/// @}

//
//  CP2130 SPI Configuration and Transfer Operations
//
/// @name CP2130 SPI Configuration and Transfer Operations
/// @{

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetSpiControlBytes       (_In_ const CP213x_DEVICE hDevice, _Out_writes_bytes_(CP213x_NUM_GPIO) BYTE controlBytes[CP213x_NUM_GPIO] );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetSpiControlByte        (_In_ const CP213x_DEVICE hDevice, _In_ const BYTE channel, _In_ const BYTE controlByte );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetSpiDelay              (_In_ const CP213x_DEVICE hDevice,
								 _In_ const BYTE channel,
                                    BYTE* delayMode,
                                    WORD* interByteDelay,
                                    WORD* postAssertDelay,
                                    WORD* preDeassertDelay );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetSpiDelay              (_In_ const CP213x_DEVICE hDevice,
								 _In_ const BYTE channel,
								 _In_ const BYTE delayMode,
								 _In_ const WORD interByteDelay,
								 _In_ const WORD postAssertDelay,
								 _In_ const WORD preDeassertDelay );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetChipSelect            (_In_ const CP213x_DEVICE hDevice,
                                  WORD* channelCsEnable,
                                  WORD* pinCsEnable );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetChipSelect			(_In_ const CP213x_DEVICE hDevice, _In_ const BYTE channel, _In_ const BYTE mode);

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_TransferWrite			(_In_ const CP213x_DEVICE hDevice,
								BYTE pWriteBuf[],
								_In_ const DWORD length,
								_In_ const BOOL releaseBusAfterTransfer,
								_In_ const DWORD timeoutMs,
								DWORD * pBytesActuallyWritten);

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_TransferWriteRead        (_In_ const CP213x_DEVICE hDevice,
                                  BYTE pWriteBuf[],
                                  BYTE pReadBuf[],
                                  _In_ const DWORD length,
									_In_ const BOOL releaseBusAfterTransfer,
                                  _In_ const DWORD timeoutMs,
                                  DWORD* pBytesActuallyTransferred );

///
/// @brief Perform SPI Read (asynchronous)
///
/// @details The device reads SPI data in 'blockSize' chunks.
/// This function returns immediately.  The application should call @ref CP213x_ReadPoll() periodically to read data.
/// When 'totalSize' bytes have been read, the Read operation is terminated.  To terminate the operation before
/// then, the application should call @ref CP213x_ReadAbort().
///
/// @param[in] hDevice USB Interface Handle
/// @param[in] totalSize Total number of bytes to read
/// @param[in] blockSize Number of bytes to read at a time
/// @param[in] releaseBusAfterTransfer Release buf after transfer is complete
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval USB_SPI_ERRCODE_READ_THREAD_CREATE_FAILURE
/// @retval USB_SPI_ERRCODE_PIPE_ABORT_FAIL
/// @retval USB_SPI_ERRCODE_PIPE_FLUSH_FAIL
/// @retval USB_SPI_ERRCODE_HWIF_TRANSFER_TIMEOUT
/// @retval USB_SPI_ERRCODE_PIPE_WRITE_FAIL
/// @retval USB_SPI_ERRCODE_READ_THREAD_START_FAILURE
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_TransferReadAsync        (_In_ const CP213x_DEVICE hDevice,
                                  DWORD totalSize,
                                  DWORD blockSize,
                                  BOOL releaseBusAfterTransfer );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_TransferReadSync         (_In_ const CP213x_DEVICE hDevice,
                                  BYTE pReadBuf[],
                                  DWORD length,
                                  BOOL releaseBusAfterTransfer,
                                  DWORD timeoutMs,
                                  DWORD* pBytesActuallyRead );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_TransferReadRtrAsync     (_In_ const CP213x_DEVICE hDevice,
                                  DWORD totalSize,
                                  DWORD blockSize,
                                  BOOL releaseBusAfterTransfer );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_TransferReadRtrSync      (_In_ const CP213x_DEVICE hDevice,
                                  BYTE pReadBuf[],
                                  DWORD totalSize,
                                  DWORD blockSize,
                                  BOOL releaseBusAfterTransfer,
                                  DWORD timeoutMs,
                                  DWORD* pBytesActuallyRead );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetRtrState              (_In_ const CP213x_DEVICE hDevice, BYTE* isStopped );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetRtrStop               (_In_ const CP213x_DEVICE hDevice, BYTE stopRtr );

///
/// @brief Read bytes received during an asynchronous Read or ReadRTR operation
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
/// @param[in] pReadBuf Buffer of data to read
/// @param[in] maxLength Maximum number of bytes to read
/// @param[out] pBytesActuallyRead Number of bytes actually read
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval USB_SPI_ERRCODE_NULL_POINTER
/// @retval USB_SPI_ERRCODE_READ_THREAD_NOT_RUNNING
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_ReadPoll                 (_In_ const CP213x_DEVICE hDevice,
                                  BYTE pReadBuf[],
                                  DWORD maxLength,
                                  DWORD* pBytesActuallyRead );

///
/// @brief Abort an asynchronous Read or ReadRTR operation
///
/// @details
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
/// @retval USB_SPI_ERRCODE_NULL_POINTER
///
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_ReadAbort                (_In_ const CP213x_DEVICE hDevice );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_AbortInputPipe           (_In_ const CP213x_DEVICE hDevice );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_AbortOutputPipe          (_In_ const CP213x_DEVICE hDevice );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_FlushInputPipe           (_In_ const CP213x_DEVICE hDevice );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_FlushOutputPipe          (_In_ const CP213x_DEVICE hDevice );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetFifoFullThreshold     (_In_ const CP213x_DEVICE hDevice, BYTE* pFifoFullThreshold );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetFifoFullThreshold     (_In_ const CP213x_DEVICE hDevice, _In_ const BYTE fifoFullThreshold );

/// @}

//
//  CP2130 GPIO and Auxiliary-Function Pins
//
/// @name CP2130 GPIO and Auxiliary-Function Pins
/// @{

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetGpioModeAndLevel(_In_ const CP213x_DEVICE hDevice, _In_ _In_range_(0, CP213x_NUM_GPIO - 1) const BYTE channel, _Out_writes_bytes_opt_(1) BYTE* mode, _Out_writes_bytes_opt_(1) BYTE* level);

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetGpioModeAndLevel      (_In_ const CP213x_DEVICE hDevice, _In_ _In_range_(0, CP213x_NUM_GPIO-1) const BYTE channel, const BYTE mode, const BYTE level );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetGpioValues            (_In_ const CP213x_DEVICE hDevice, _Out_writes_bytes_opt_(2) WORD* gpioValues );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetGpioValues            (_In_ const CP213x_DEVICE hDevice, _In_ const WORD mask, _In_ const WORD gpioValues );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetEventCounter          (_In_ const CP213x_DEVICE hDevice, _Out_writes_bytes_opt_(1) BYTE* mode, _Out_writes_bytes_opt_(2) WORD* eventCount );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetEventCounter          (_In_ const CP213x_DEVICE hDevice, _In_ const BYTE mode, _In_ const WORD eventCount );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_GetClockDivider          (_In_ const CP213x_DEVICE hDevice, _Out_writes_bytes_opt_(1) BYTE* clockDivider );

///
/// @brief what it does in brief
///
/// @details what it does in detail
///
/// @param[in] hDevice USB Interface Handle
///
/// @returns Returns #USB_SPI_ERRCODE_SUCCESS on success, or another #USB_SPI_STATUS Return value if there is an error.
/// @retval #USB_SPI_ERRCODE_SUCCESS -- success
/// @retval #USB_SPI_ERRCODE_INVALID_HANDLE -- 
_Check_return_
_Ret_range_(USB_SPI_ERRCODE_SUCCESS, MAXLONG)
_Success_(return == USB_SPI_ERRCODE_SUCCESS)
SLAB_USB_SPI_API USB_SPI_STATUS WINAPI
CP213x_SetClockDivider          (_In_ const CP213x_DEVICE hDevice, _In_ const BYTE clockDivider );

/// @}

#ifdef __cplusplus
}
#endif // __cplusplus

/// @}

#endif // HOST_LIB_SLABUSBSPI_INCLUDE_SLAB_USB_SPI_H_INCLUDED_9QK7BKA9FX

/////////////////////////////////////////////////////////////////////////////
// End of file
/////////////////////////////////////////////////////////////////////////////
