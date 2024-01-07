/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <signal.h>
#include "Terminal.h"
#include "SLABHIDDevice.h"
#include "SLABHIDtoUART.h"
#include <unistd.h>

/////////////////////////////////////////////////////////////////////////////
// Definitions
/////////////////////////////////////////////////////////////////////////////

#define SILABS_VID                      0x10c4
#define CP2110_PID                      0xea80
#define CP2114_PID                      0xeab0

#define ERROR_LEVEL_SUCCESS             0
#define ERROR_LEVEL_INVALID_ARG         1
#define ERROR_LEVEL_API_CODE            2

/////////////////////////////////////////////////////////////////////////////
// Namespaces
/////////////////////////////////////////////////////////////////////////////

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// Static Global Variables
/////////////////////////////////////////////////////////////////////////////

// Globals used to store command line arguments
static bool ShowSerialList = false; // -l
static bool UseSerial = false; // -s
static string Serial; // -s SERIAL
static unsigned short Vid = SILABS_VID; // -vid VID
static unsigned short Pid = CP2110_PID; // -pid PID

// The connected HID UART device object
static HID_UART_DEVICE Device;

// Keep track of terminal mode and allow user to leave
// terminal mode using Ctrl-C
static bool InTerminalMode = false;

/////////////////////////////////////////////////////////////////////////////
// Static Global Functions
/////////////////////////////////////////////////////////////////////////////

// Convert error level value to a printable string
static string DecodeErrorLevel(int errorLevel)
{
    string text;
    char temp[100];
    
    switch (errorLevel)
    {
    case ERROR_LEVEL_SUCCESS:           text = "Success"; break;
    case ERROR_LEVEL_INVALID_ARG:       text = "Invalid command line parameter"; break;
    case ERROR_LEVEL_API_CODE:          text = "API Error Code"; break;
    default:
        sprintf(temp, "Error Level (%d)", errorLevel);
        text = temp;
        break;
    }
    
    return text;
}

// Convert HID_UART_STATUS value to a printable string
static string DecodeHidUartStatus(HID_UART_STATUS status)
{
    string text;
    char temp[100];
    
    switch (status)
    {
    case HID_UART_SUCCESS:                      text = "HID_UART_SUCCESS"; break;
    case HID_UART_DEVICE_NOT_FOUND:             text = "HID_UART_DEVICE_NOT_FOUND"; break;
    case HID_UART_INVALID_HANDLE:               text = "HID_UART_INVALID_HANDLE"; break;
    case HID_UART_INVALID_DEVICE_OBJECT:        text = "HID_UART_INVALID_DEVICE_OBJECT"; break;
    case HID_UART_INVALID_PARAMETER:            text = "HID_UART_INVALID_PARAMETER"; break;
    case HID_UART_INVALID_REQUEST_LENGTH:       text = "HID_UART_INVALID_REQUEST_LENGTH"; break;
    case HID_UART_READ_ERROR:                   text = "HID_UART_READ_ERROR"; break;
    case HID_UART_WRITE_ERROR:                  text = "HID_UART_WRITE_ERROR"; break;
    case HID_UART_READ_TIMED_OUT:               text = "HID_UART_READ_TIMED_OUT"; break;
    case HID_UART_WRITE_TIMED_OUT:              text = "HID_UART_WRITE_TIMED_OUT"; break;
    case HID_UART_DEVICE_IO_FAILED:             text = "HID_UART_DEVICE_IO_FAILED"; break;
    case HID_UART_DEVICE_ACCESS_ERROR:          text = "HID_UART_DEVICE_ACCESS_ERROR"; break;
    case HID_UART_DEVICE_NOT_SUPPORTED:         text = "HID_UART_DEVICE_NOT_SUPPORTED"; break;
    case HID_UART_UNKNOWN_ERROR:                text = "HID_UART_UNKNOWN_ERROR"; break;
    default:
        sprintf(temp, "Error Status (0x%02X)", status);
        text = temp;
        break;
    }
    
    return text;
}

// Convert Get/SetUartConfig() parity value to a printable string
static string DecodeParity(BYTE parity)
{
    string text;
    
    switch (parity)
    {
    case HID_UART_NO_PARITY:    text = "N"; break;
    case HID_UART_ODD_PARITY:   text = "O"; break;
    case HID_UART_EVEN_PARITY:  text = "E"; break;
    case HID_UART_MARK_PARITY:  text = "M"; break;
    case HID_UART_SPACE_PARITY: text = "S"; break;
    default:                    text = "?"; break;
    }
    
    return text;
}

// Convert string to parity value
static BYTE EncodeParity(string text)
{
    BYTE parity = HID_UART_NO_PARITY;
    
    if (text[0] == 'n' || text[0] == 'N')
        parity = HID_UART_NO_PARITY;
    if (text[0] == 'o' || text[0] == 'O')
        parity = HID_UART_ODD_PARITY;
    if (text[0] == 'e' || text[0] == 'E')
        parity = HID_UART_EVEN_PARITY;
    if (text[0] == 'm' || text[0] == 'M')
        parity = HID_UART_MARK_PARITY;
    if (text[0] == 's' || text[0] == 'S')
        parity = HID_UART_SPACE_PARITY;
    
    return parity;
}

// Convert Get/SetUartConfig() flow control value to a printable string
static string DecodeFlowControl(BYTE flowControl)
{
    string text;
    
    switch (flowControl)
    {
    case HID_UART_NO_FLOW_CONTROL:      text = "None";          break;
    case HID_UART_RTS_CTS_FLOW_CONTROL: text = "RTS/CTS";       break;
    }
    
    return text;
}

// Print a string for the error code and API status
static void PrintError(int level, HID_UART_STATUS status)
{
    string text;

    // Decode a command line error code
    if (level != ERROR_LEVEL_API_CODE)
    {
        text = DecodeErrorLevel(level);
    }
    // Decode a HID_UART_STATUS code
    else
    {
        text = DecodeHidUartStatus(status);
    }
    
    printf("%s\n", text.c_str());
}

// Display the command line argument help text to the console
static void PrintUsage()
{
    BYTE major;
    BYTE minor;
    BOOL release;
    char sLibVersion[100];
    
    HidUart_GetLibraryVersion(&major, &minor, &release);
    
    sprintf(sLibVersion, "%u.%u%s", major, minor, release ? "" : " (Debug)");
    
    printf ("Silicon Labs CP211x HID to UART Example 1.0\n");
    printf("libslabhidtouart.so %s\n", sLibVersion);
    printf("\n");
    printf("Demonstrate the UART and GPIO capabilities of a CP211x device.\n");
    printf("\n");
    printf("hidUartExample [-l] [-vid VID] [-pid PID]\n");
    printf("hidUartExample [-s SERIAL] [-vid VID] [-pid PID]\n");
    printf("\n");
    printf("positional arguments:\n");
    printf("-l               Display a list of all CP211x serial strings\n");
    printf("-vid VID         Specify the USB Vendor ID of the device (default 0x10c4)\n");
    printf("-pid PID         Specify the USB Product ID of the device (default 0xea80)\n");
    printf("-s SERIAL        Connect to a CP211x device by serial string\n");
    printf("\n");
}

// Display a list of all available serial strings
static void PrintSerialList()
{
    HID_UART_STATUS status;
    DWORD numDevices = 0;
    HID_UART_DEVICE_STR serial;
    
    status = HidUart_GetNumDevices(&numDevices, Vid, Pid);
    
    if (status == HID_UART_SUCCESS)
    {
        printf("List of CP211x serial strings\n");
        printf("=============================\n");
        
        for (DWORD i = 0; i < numDevices; i++)
        {
            if (HidUart_GetString(i, Vid, Pid, serial, HID_UART_GET_SERIAL_STR) == HID_UART_SUCCESS)
            {
                printf("%s\n", serial);
            }
        }
    }
}

// Connect to the CP211x device:
// - If using -s SERIAL, then connect to the first device with a matching
//   serial string, VID, and PID
// - Otherwise, connect to the first device with matching VID and PID
static HID_UART_STATUS ConnectToDevice()
{
    HID_UART_STATUS status;
    
    DWORD numDevices;
    
    status = HidUart_GetNumDevices(&numDevices, Vid, Pid);
    
    if (status != HID_UART_SUCCESS)
        return status;
    
    if (UseSerial)
    {
        bool found = false;
        DWORD index = 0;
        
        for (DWORD i = 0; i < numDevices; i++)
        {
            HID_UART_DEVICE_STR serial;
            
            if (HidUart_GetString(i, Vid, Pid, serial, HID_UART_GET_SERIAL_STR) == HID_UART_SUCCESS)
            {
                if (Serial == serial)
                {
                    found = true;
                    index = i;
                }
            }
        }
        
        if (!found)
            return HID_UART_DEVICE_NOT_FOUND;
        
        status = HidUart_Open(&Device, index, Vid, Pid);
    }
    else
    {
        status = HidUart_Open(&Device, 0, Vid, Pid);
    }
    
    // Set read timeouts for quickest response time
    if (status == HID_UART_SUCCESS)
    {
        status = HidUart_SetTimeouts(Device, 0, 1000);
    }
    
    return status;
}

static HID_UART_STATUS DisconnectFromDevice()
{
    return HidUart_Close(Device);
}

// Initialize the argument variables based on the switches specified
static bool ParseArguments(const vector<string>& args)
{
    // Ignore the first argument, which is the exe path
    for (size_t i = 1; i < args.size(); i++)
    {
        string arg = args[i];
        
        if (arg == "-l")
        {
            ShowSerialList = true;
        }
        else if (arg == "-s")
        {
            // Need at least one more argument to specify the serial string
            if (i >= args.size() - 1)
            {
                return false;
            }
            
            UseSerial = true;
            Serial = args[i + 1];
            i++;
        }
        else if (arg == "-vid")
        {
            // Need at least one more argument to specify the VID
            if (i >= args.size() - 1)
            {
                return false;
            }
            
            Vid = strtoul(args[i + 1].c_str(), NULL, 0);
            i++;
        }
        else if (arg == "-pid")
        {
            // Need at least one more argument to specify the PID
            if (i >= args.size() - 1)
            {
                return false;
            }
            
            Pid = strtoul(args[i + 1].c_str(), NULL, 0);
            i++;
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

static void PrintMainMenu()
{
    printf("\n");
    printf("Main Menu\n");
    printf("=========\n");
    printf("1. Display device information\n");
    printf("2. Get UART Config\n");
    printf("3. Set UART Config\n");
    printf("4. UART Terminal\n");
    printf("5. Read GPIO Latch\n");
    printf("6. Write GPIO Latch\n");
    printf("7. Exit\n");
}

static void DeviceInfo()
{
    HID_UART_STATUS status;
    
    WORD vid, pid, releaseNumber;
    HID_UART_DEVICE_STR serial;
    HID_UART_DEVICE_STR path;
    BYTE partNumber, version;
    HID_UART_DEVICE_STR mfr;
    HID_UART_DEVICE_STR product;
    
    printf("\n");
    printf("Device Information\n");
    printf("==================\n");
    
    status = HidUart_GetOpenedAttributes(Device, &vid, &pid, &releaseNumber);
    if (status == HID_UART_SUCCESS)
        printf("VID: 0x%04X PID: 0x%04X Device Release Number: 0x%04X\n", vid, pid, releaseNumber);
    else
        printf("VID: PID: Device Release Number: %s\n", DecodeHidUartStatus(status).c_str());
    
    status = HidUart_GetOpenedString(Device, serial, HID_UART_GET_SERIAL_STR);
    if (status == HID_UART_SUCCESS)
        printf("Serial: %s\n", serial);
    else
        printf("Serial: %s\n", DecodeHidUartStatus(status).c_str());
    
    status = HidUart_GetOpenedString(Device, path, HID_UART_GET_PATH_STR);
    if (status == HID_UART_SUCCESS)
        printf("Path: %s\n", path);
    else
        printf("Path: %s", DecodeHidUartStatus(status).c_str());
    
    status = HidUart_GetPartNumber(Device, &partNumber, &version);
    if (status == HID_UART_SUCCESS)
        printf("Part Number: %u Version %u\n", partNumber, version);
    else
        printf("Part Number: Version: %s\n", DecodeHidUartStatus(status).c_str());
    
    status = HidUart_GetOpenedString(Device, mfr, HID_UART_GET_MANUFACTURER_STR);
    if (status == HID_UART_SUCCESS)
        printf("Manufacturer: %s\n", mfr);
    else
        printf("Manufacturer: %s\n", DecodeHidUartStatus(status).c_str());
    
    status = HidUart_GetOpenedString(Device, product, HID_UART_GET_PRODUCT_STR);
    if (status == HID_UART_SUCCESS)
        printf("Product: %s\n", product);
    else
        printf("Product: %s\n", DecodeHidUartStatus(status).c_str());
    
    printf("\n");
}

static void GetUartConfig()
{
    HID_UART_STATUS status;
    DWORD baudRate;
    BYTE dataBits;
    BYTE parity;
    BYTE stopBits;
    BYTE flowControl;
    
    printf("\n");
    printf("Get UART Configuration\n");
    printf("======================\n");
    
    status = HidUart_GetUartConfig(Device, &baudRate, &dataBits, &parity, &stopBits, &flowControl);
    if (status == HID_UART_SUCCESS)
    {
        printf("Baud Rate: %u\n", (unsigned int)baudRate);
        printf("Data Bits: %u\n", dataBits + 5);
        printf("Parity: %s\n", DecodeParity(parity).c_str());
        printf("Stop Bits: %s\n", stopBits == HID_UART_SHORT_STOP_BIT ? "1" : "1.5/2");
        printf("Flow Control: %s\n", DecodeFlowControl(flowControl).c_str());
    }
    else
    {
        printf("Error retrieving UART config: %s\n", DecodeHidUartStatus(status).c_str());
    }
}

static void SetUartConfig()
{
    HID_UART_STATUS status;
    DWORD baudRate;
    BYTE dataBits;
    BYTE parity;
    BYTE stopBits;
    BYTE flowControl;
    char str[100];
    unsigned int temp;
    
    printf("\n");
    printf("Set UART Configuration\n");
    printf("======================\n");

    printf("Baud Rate: ");
    scanf("%u", &temp);
    baudRate = temp;
    
    printf("Data Bits (5 - 8): ");
    scanf("%u", &temp);
    dataBits = temp - 5;
    
    printf("Parity (N, O, E, M, S): ");
    scanf("%s", str);
    parity = EncodeParity(str);
    
    printf("Stop Bits (1 = 1 stop bit, 2 = 1.5/2 stop bits): ");
    scanf("%u", &temp);
    stopBits = temp - 1;
    
    printf("Flow Control (0 - None, 1 = RTS/CTS): ");
    scanf("%u", &temp);
    flowControl = temp;
    
    status = HidUart_SetUartConfig(Device, baudRate, dataBits, parity, stopBits, flowControl);
    if (status != HID_UART_SUCCESS)
        printf("Failed setting UART config: %s\n", DecodeHidUartStatus(status).c_str());
    
    printf("\n");
}

static void UartTerminal()
{
    HID_UART_STATUS status;
    BYTE rx[HID_UART_MAX_READ_SIZE];
    BYTE tx;
    DWORD numBytesRead;
    DWORD numBytesWritten;

    // Entered terminal mode
    InTerminalMode = true;

    // Don't wait for newline to read from stdin
    // Disable stdin echo
    setblocking(false);
    setecho(false);

    printf("\n");
    printf("UART Terminal (Ctrl-C Returns to Main Menu)\n");
    printf("===========================================\n");
    printf("\n");
    
    while (1)
    {
        // Read as much UART data as possible
        status = HidUart_Read(Device, rx, HID_UART_MAX_READ_SIZE, &numBytesRead);
        
        // Read at least 1 byte
        if ((status == HID_UART_SUCCESS || status == HID_UART_READ_TIMED_OUT) &&
             numBytesRead > 0)
        {
            // Output the data read
            for (DWORD i = 0; i < numBytesRead; i++)
            {
                putchar(rx[i]);
            }
            fflush(stdout);
        }
        
        // Check for Ctrl-C
        if (!InTerminalMode)
            break;

        // Check for user input without blocking
        if (kbhit())
        {
            // Check for Ctrl-C
            if (!InTerminalMode)
                break;
            
            // Get the character typed
            tx = getchar();
            
            // Transmit the character across the UART
            status = HidUart_Write(Device, &tx, 1, &numBytesWritten);
        }
        
        // Check for Ctrl-C
        if (!InTerminalMode)
            break;
        
        // Sleep 1 ms to keep CPU usage down
        usleep(1000);
        
        // Check for Ctrl-C
        if (!InTerminalMode)
            break;
    }
    
    // Reset terminal settings back to normal
    setblocking(true);
    setecho(true);
}

static void ReadLatch()
{
    HID_UART_STATUS status;
    WORD latchValue;
    
    printf("\n");
    printf("Read GPIO Latch\n");
    printf("===============\n");
    
    status = HidUart_ReadLatch(Device, &latchValue);
    if (status == HID_UART_SUCCESS)
        printf("Value: 0x%04X\n", latchValue);
    else
        printf("Value: %s\n", DecodeHidUartStatus(status).c_str());
    
    printf("\n");
}

static void WriteLatch()
{
    HID_UART_STATUS status;
    
    WORD latchValue;
    WORD latchMask;
    
    printf("\n");
    printf("Write GPIO Latch\n");
    printf("================\n");
    
    printf("Value (2-byte hex): ");
    scanf("%hx", &latchValue);
    
    printf("Mask (2-byte hex): ");
    scanf("%hx", &latchMask);
    
    status = HidUart_WriteLatch(Device, latchValue, latchMask);
    if (status != HID_UART_SUCCESS)
        printf("Failed writing latch: %s\n", DecodeHidUartStatus(status).c_str());
    
    printf("\n");
}

// Main Menu System:
// Present the user with several options to retrieve device information,
// configure the UART, read/write GPIO, and send/receive UART data
static void MainMenu()
{
    while (1)
    {
        char input[MAX_PATH];
        int sel;
        
        // Display the menu options
        PrintMainMenu();

        // Retrieve all text input until a newline or eof is
        // encountered
        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            // Parse the text input for a number value
            if (sscanf(input, "%d", &sel) == 1)
            {
                switch (sel)
                {
                case 1: DeviceInfo(); break;
                case 2: GetUartConfig(); break;
                case 3: SetUartConfig(); break;
                case 4: UartTerminal(); break;
                case 5: ReadLatch(); break;
                case 6: WriteLatch(); break;
                case 7: return;
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////

// Handle interactive attention signal generated by the application user
// (ctrl-c)
void SigintHandler(int param)
{
    // If in terminal mode, then break out of terminal mode
    if (InTerminalMode)
    {
        InTerminalMode = false;
    }
    // Otherwise close the program
    else
    {
        DisconnectFromDevice();
        exit(ERROR_LEVEL_SUCCESS);
    }
}

int main(int argc, char* argv[])
{
    // Register for SIGINT events
    signal(SIGINT, SigintHandler);
    
    HID_UART_STATUS status = HID_UART_SUCCESS;
    
    // Convert the arguments to a list of strings
    vector<string> args(argv, argv + argc);
    
    // The first argument is the path to the executable
    if (args.size() == 0)
    {
        PrintError(ERROR_LEVEL_INVALID_ARG, status);
        return ERROR_LEVEL_INVALID_ARG;
    }
    
    // If the /? or -? switch is found
    if (find(args.begin(), args.end(), "/?") != args.end() ||
        find(args.begin(), args.end(), "-?") != args.end())
    {
        PrintUsage();
        return ERROR_LEVEL_SUCCESS;
    }
    
    // Parse the command line arguments
    if (!ParseArguments(args))
    {
        PrintError(ERROR_LEVEL_INVALID_ARG, status);
        return ERROR_LEVEL_INVALID_ARG;
    }
    
    // Display list of device serial strings
    if (ShowSerialList)
    {
        PrintSerialList();
        return ERROR_LEVEL_SUCCESS;
    }
    
    // Connect to the first device or the device
    // matching the specified VID, PID, and serial string
    status = ConnectToDevice();
    
    if (status != HID_UART_SUCCESS)
    {
        PrintError(ERROR_LEVEL_API_CODE, status);
        return ERROR_LEVEL_API_CODE;
    }

    // Start the menu system
    MainMenu();
    
    DisconnectFromDevice();
    
    return ERROR_LEVEL_SUCCESS;
}
