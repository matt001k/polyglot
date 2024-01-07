// Copyright (c) 2015-2016 by Silicon Laboratories Inc.  All rights reserved.
// The program contained in this listing is proprietary to Silicon Laboratories,
// headquartered in Austin, Texas, U.S.A. and is subject to worldwide copyright
// protection, including protection under the United States Copyright Act of 1976
// as an unpublished work, pursuant to Section 104 and Section 408 of Title XVII
// of the United States code.  Unauthorized copying, adaptation, distribution,
// use, or display is prohibited by this law.

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#pragma comment (lib, "SLABHIDtoUART.lib")
#else
#include "OsDep.h"
#endif
#include "stdio.h"
#include "SLABCP2110.h"
#include "SLABCP2114.h"
#include "util.h"
#include "smt.h"

const WORD g_LockAll = 
        HID_UART_LOCK_PRODUCT_STR_1 |
        HID_UART_LOCK_PRODUCT_STR_2 |
        HID_UART_LOCK_SERIAL_STR |
        HID_UART_LOCK_PIN_CONFIG |
        HID_UART_LOCK_VID |
        HID_UART_LOCK_PID |
        HID_UART_LOCK_POWER |
        HID_UART_LOCK_POWER_MODE |
        HID_UART_LOCK_RELEASE_VERSION |
        HID_UART_LOCK_FLUSH_BUFFERS |
        HID_UART_LOCK_MFG_STR_1 |
        HID_UART_LOCK_MFG_STR_2;

void AbortOnErr( HID_UART_STATUS status, std::string funcName)
{
    if( status != HID_UART_SUCCESS)
    {
        char msg[ 128];
        sprintf( msg, /*SIZEOF_ARRAY( msg),*/ "%s returned 0x%x", funcName.c_str(), status);
        throw CDllErr( msg);
    }
}

//---------------------------------------------------------------------------------
DWORD LibSpecificNumDevices( const CVidPid &oldVidPid, const CVidPid &newVidPid)
{
    DWORD newDevCnt;
    AbortOnErr( HidUart_GetNumDevices( &newDevCnt, newVidPid.m_Vid, newVidPid.m_Pid ), "HidUart_GetNumDevices");
    DWORD oldDevCnt = 0;
    if( oldVidPid.m_Vid != newVidPid.m_Vid || oldVidPid.m_Pid != newVidPid.m_Pid)
    {
        AbortOnErr( HidUart_GetNumDevices( &oldDevCnt, oldVidPid.m_Vid, oldVidPid.m_Pid ), "HidUart_GetNumDevices");
    }
    return newDevCnt + oldDevCnt;
}
//---------------------------------------------------------------------------------
class CCP2110Dev
{
public:
    CCP2110Dev( const CVidPid &FilterVidPid, DWORD devIndex);
    ~CCP2110Dev();
    HID_UART_DEVICE   handle() const { return m_H; }
    bool              isLocked() const;
    void              lock() const;
    void              reset() const;
    CDevType          getDevType() const;
    CVidPid           getVidPid() const;
    BYTE              getPowerMode() const;
    BYTE              getMaxPower() const;
    WORD              getDevVer() const;
    BYTE              getFlushBuf() const;
    std::vector<BYTE> getSerNum( bool isAscii) const;
    std::vector<BYTE> getManufacturer( bool isAscii) const;
    std::vector<BYTE> getProduct( bool isAscii) const;
    void              setSerNum( const std::vector<BYTE> &str, bool isAscii) const;
    void              setManufacturer( const std::vector<BYTE> &str, bool isAscii) const;
    void              setProduct( const std::vector<BYTE> &str, bool isAscii) const;
private:
    HID_UART_DEVICE m_H;
};
CCP2110Dev::CCP2110Dev( const CVidPid &FilterVidPid, DWORD devIndex)
{
    AbortOnErr( HidUart_Open( &m_H, devIndex, FilterVidPid.m_Vid, FilterVidPid.m_Pid), "HidUart_Open");
}
CCP2110Dev::~CCP2110Dev()
{
    HID_UART_STATUS status = HidUart_Close( m_H);
    if( status != HID_UART_SUCCESS)
    {
        std::cerr << "HidUart_Close failed\n";
    }
}
bool CCP2110Dev::isLocked() const
{
    WORD lock;
    AbortOnErr( HidUart_GetLock( m_H, &lock), "HidUart_GetLock");
    return (lock & g_LockAll) == 0;
}
void CCP2110Dev::lock() const
{
    AbortOnErr( HidUart_SetLock( m_H, g_LockAll), "HidUart_SetLock");
}
void  CCP2110Dev::reset() const
{
    AbortOnErr( HidUart_Reset( m_H), "HidUart_Reset");
}
CDevType CCP2110Dev::getDevType() const
{
    BYTE partNum;
    BYTE version;
    AbortOnErr( HidUart_GetPartNumber( m_H, &partNum, &version ), "HidUart_GetPartNumber");
    return CDevType( partNum);
}
CVidPid CCP2110Dev::getVidPid() const
{
    WORD vid; WORD pid; BYTE maxPower; BYTE powerMode; WORD devVer; BYTE flushBuffers;
    AbortOnErr( HidUart_GetUsbConfig( m_H, &vid, &pid, &maxPower, &powerMode, &devVer, &flushBuffers), "HidUart_GetUsbConfig");
    return CVidPid( vid, pid);
}
BYTE CCP2110Dev::getPowerMode() const
{
    WORD vid; WORD pid; BYTE maxPower; BYTE powerMode; WORD devVer; BYTE flushBuffers;
    AbortOnErr( HidUart_GetUsbConfig( m_H, &vid, &pid, &maxPower, &powerMode, &devVer, &flushBuffers), "HidUart_GetUsbConfig");
    return powerMode;
}
BYTE CCP2110Dev::getMaxPower() const
{
    WORD vid; WORD pid; BYTE maxPower; BYTE powerMode; WORD devVer; BYTE flushBuffers;
    AbortOnErr( HidUart_GetUsbConfig( m_H, &vid, &pid, &maxPower, &powerMode, &devVer, &flushBuffers), "HidUart_GetUsbConfig");
    return maxPower;
}
WORD CCP2110Dev::getDevVer() const
{
    WORD vid; WORD pid; BYTE maxPower; BYTE powerMode; WORD devVer; BYTE flushBuffers;
    AbortOnErr( HidUart_GetUsbConfig( m_H, &vid, &pid, &maxPower, &powerMode, &devVer, &flushBuffers), "HidUart_GetUsbConfig");
    return devVer;
}
BYTE CCP2110Dev::getFlushBuf() const
{
    WORD vid; WORD pid; BYTE maxPower; BYTE powerMode; WORD devVer; BYTE flushBuffers;
    AbortOnErr( HidUart_GetUsbConfig( m_H, &vid, &pid, &maxPower, &powerMode, &devVer, &flushBuffers), "HidUart_GetUsbConfig");
    return flushBuffers;
}
std::vector<BYTE> CCP2110Dev::getSerNum( bool) const
{
    std::vector<BYTE> str( MAX_UCHAR);
    BYTE CchStr = 0;
    AbortOnErr( HidUart_GetSerialString( m_H, reinterpret_cast<char *>( str.data()), &CchStr), "HidUart_GetSerialString");
    str.resize( CchStr);
    return str;
}
std::vector<BYTE> CCP2110Dev::getManufacturer( bool) const
{
    std::vector<BYTE> str( MAX_UCHAR);
    BYTE CchStr = 0;
    AbortOnErr( HidUart_GetManufacturingString( m_H, reinterpret_cast<char *>( str.data()), &CchStr), "HidUart_GetManufacturingString");
    str.resize( CchStr);
    return str;
}
std::vector<BYTE> CCP2110Dev::getProduct( bool) const
{
    std::vector<BYTE> str( MAX_UCHAR);
    BYTE CchStr = 0;
    AbortOnErr( HidUart_GetProductString( m_H, reinterpret_cast<char *>( str.data()), &CchStr), "HidUart_GetProductString");
    str.resize( CchStr);
    return str;
}
void CCP2110Dev::setSerNum( const std::vector<BYTE> &str, bool) const
{
    AbortOnErr( HidUart_SetSerialString( m_H, reinterpret_cast<char *>( const_cast<BYTE *>( str.data())), static_cast<BYTE>( str.size())), "HidUart_SetSerialString");
}
void CCP2110Dev::setManufacturer( const std::vector<BYTE> &str, bool) const
{
    AbortOnErr( HidUart_SetManufacturingString( m_H, reinterpret_cast<char *>( const_cast<BYTE *>( str.data())), static_cast<BYTE>( str.size())), "HidUart_SetManufacturingString");
}
void CCP2110Dev::setProduct( const std::vector<BYTE> &str, bool) const
{
    AbortOnErr( HidUart_SetProductString( m_H, reinterpret_cast<char *>( const_cast<BYTE *>( str.data())), static_cast<BYTE>( str.size())), "HidUart_SetProductString");
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
struct CCP2110_4Parms : public CDevParms<CCP2110Dev>
{
    CCP2110_4Parms()
        : m_ManufStr( false /*supportsUnicode*/)
    {
        m_FlushBufCfgSpecified = false;
    }
    virtual void readParm( const std::string &parmName);
    void program( const CCP2110Dev &dev, const std::vector<BYTE> *pSerNum) const;
    void verify( const CCP2110Dev &dev, CSerNumSet &serNumSet) const;
protected:
    bool m_FlushBufCfgSpecified;
    BYTE m_FlushBufCfg;
    CManufacturerString<CCP2110Dev>  m_ManufStr;
};
void CCP2110_4Parms::readParm( const std::string &parmName)
{
    if( m_ManufStr.readParm( parmName))
    {
        return;
    }
    if( parmName == "FlushBufferConfig")
    {
        setSpecified( m_FlushBufCfgSpecified, parmName);
        m_FlushBufCfg = readUcharParm();
        readKeyword( "}"); // end of parameter list
        return;
    }
    CDevParms::readParm( parmName);
}
void CCP2110_4Parms::program( const CCP2110Dev &dev, const std::vector<BYTE> *pSerNum) const
{
    CDevParms::program( dev, pSerNum);
    m_ManufStr.program( dev);

    BYTE mask = 0;
    if (m_VidPidSpecified)
    {
        mask |= HID_UART_SET_VID;
        mask |= HID_UART_SET_PID;
    }
    if (m_PowerModeSpecified)
    {
        mask |= HID_UART_SET_POWER_MODE;
    }
    if (m_MaxPowerSpecified)
    {
        mask |= HID_UART_SET_POWER;
    }
    if (m_DevVerSpecified)
    {
        mask |= HID_UART_SET_RELEASE_VERSION;
    }
    if (m_FlushBufCfgSpecified)
    {
        mask |= HID_UART_SET_FLUSH_BUFFERS;
    }
    AbortOnErr(HidUart_SetUsbConfig(dev.handle(), m_Vid, m_Pid, m_MaxPower, m_PowerMode, m_DevVer, m_FlushBufCfg, mask), "HidUart_SetUsbConfig");
}
void CCP2110_4Parms::verify( const CCP2110Dev &dev, CSerNumSet &serNumSet) const
{
    CDevParms::verify( dev, serNumSet);
    m_ManufStr.verify( dev);
    if( m_FlushBufCfgSpecified)
    {
        if( m_FlushBufCfg != dev.getFlushBuf())
        {
            throw CCustErr( "Failed FlushBufferConfig verification");
        }
    }
}
//---------------------------------------------------------------------------------
struct CCP2110Parms : public CCP2110_4Parms
{
    CCP2110Parms()
    {
        m_PinCfgSpecified = false;
    }
    virtual void readParm( const std::string &parmName);
    void program( const CCP2110Dev &dev, const std::vector<BYTE> *pSerNum) const;
    void verify( const CCP2110Dev &dev, CSerNumSet &serNumSet) const;
private:
    bool              m_PinCfgSpecified;
    std::vector<BYTE> m_PinCfg;
    BOOL              m_useSuspendValues;
    WORD              m_suspendValue;
    WORD              m_suspendMode;
    BYTE              m_rs485Level;
    BYTE              m_clkDiv;
};
void CCP2110Parms::readParm( const std::string &parmName)
{
    if( parmName == "PinConfig")
    {
        setSpecified( m_PinCfgSpecified, parmName);
        readByteArrayParmExact( m_PinCfg, CP2110_PIN_CONFIG_SIZE);
        m_useSuspendValues  = static_cast<BOOL>( readUlongParm());
        m_suspendValue      = readUshortParm();
        m_suspendMode       = readUshortParm();
        m_rs485Level        = readUcharParm();
        m_clkDiv            = readUcharParm();
        readKeyword( "}"); // end of parameter list
        return;
    }
    CCP2110_4Parms::readParm( parmName);
}
void CCP2110Parms::program( const CCP2110Dev &dev, const std::vector<BYTE> *pSerNum) const
{
    CCP2110_4Parms::program( dev, pSerNum);
    if( m_PinCfgSpecified)
    {
        AbortOnErr( HidUart_SetPinConfig( dev.handle(), const_cast<BYTE*>( m_PinCfg.data()), m_useSuspendValues,
                                          m_suspendValue, m_suspendMode, m_rs485Level, m_clkDiv), "HidUart_SetPinConfig");
    }
}
void CCP2110Parms::verify( const CCP2110Dev &dev, CSerNumSet &serNumSet) const
{
    CCP2110_4Parms::verify( dev, serNumSet);
    if( m_PinCfgSpecified)
    {
        std::vector<BYTE> pinConfig( m_PinCfg.size());
        BOOL useSuspendValues; WORD suspendValue; WORD suspendMode; BYTE rs485Level; BYTE clkDiv;
        AbortOnErr( HidUart_GetPinConfig( dev.handle(), pinConfig.data(), &useSuspendValues,
                                          &suspendValue, &suspendMode, &rs485Level, &clkDiv), "HidUart_GetPinConfig");
        if( m_PinCfg           != pinConfig ||
            m_useSuspendValues != useSuspendValues ||
            m_suspendValue     != suspendValue ||
            m_suspendMode      != suspendMode ||
            m_rs485Level       != rs485Level ||
            m_clkDiv           != clkDiv)
        {
            throw CCustErr( "Failed PinConfig verification");
        }
    }
}
//---------------------------------------------------------------------------------
struct CCP2114Parms : public CCP2110_4Parms
{
    CCP2114Parms()
    {
        m_PinCfgSpecified = false;
        m_RamCfgSpecified = false;
        m_OtpSpecified    = false;
    }
    virtual void readParm( const std::string &parmName);
    void program( const CCP2110Dev &dev, const std::vector<BYTE> *pSerNum) const;
    void verify( const CCP2110Dev &dev, CSerNumSet &serNumSet) const;
private:
    bool              m_PinCfgSpecified;
    std::vector<BYTE> m_PinCfg;
    BOOL              m_useSuspendValues;
    WORD              m_suspendValue;
    WORD              m_suspendMode;
    BYTE              m_clkDiv;
    bool                      m_RamCfgSpecified;
    CP2114_RAM_CONFIG_STRUCT  m_RamCfg;
    bool                      m_OtpSpecified;
    UINT                      m_cp2114Address;
    std::vector<BYTE>         m_Otp;
};
void CCP2114Parms::readParm( const std::string &parmName)
{
    if( parmName == "PinConfig")
    {
        setSpecified( m_PinCfgSpecified, parmName);
        readByteArrayParmExact( m_PinCfg, CP2114_PIN_CONFIG_SIZE);
        m_useSuspendValues  = static_cast<BOOL>( readUlongParm());
        m_suspendValue      = readUshortParm();
        m_suspendMode       = readUshortParm();
        m_clkDiv            = readUcharParm();
        readKeyword( "}"); // end of parameter list
        return;
    }
    if( parmName == "RamConfig")
    {
        setSpecified( m_RamCfgSpecified, parmName);
        m_RamCfg.Length   = readUshortParm();
        const DWORD CeConfigData   = SIZEOF_ARRAY( m_RamCfg.configData);
	    std::vector<BYTE> configData( CeConfigData);
        readByteArrayParmExact( configData, CeConfigData);
        for( DWORD i = 0; i < CeConfigData; i++)
        {
            m_RamCfg.configData[ i] = configData[ i];
        }
        readKeyword( "}"); // end of parameter list
        return;
    }
    if( parmName == "OTP")
    {
        setSpecified( m_OtpSpecified, parmName);
        m_cp2114Address  = readUlongParm();
        readByteArrayParm( m_Otp, MAX_ULONG);
        readKeyword( "}"); // end of parameter list
        return;
    }
    CCP2110_4Parms::readParm( parmName);
}
void CCP2114Parms::program( const CCP2110Dev &dev, const std::vector<BYTE> *pSerNum) const
{
    CCP2110_4Parms::program( dev, pSerNum);
    if( m_PinCfgSpecified)
    {
        AbortOnErr( CP2114_SetPinConfig( dev.handle(), const_cast<BYTE*>( m_PinCfg.data()), m_useSuspendValues,
                                         m_suspendValue, m_suspendMode, m_clkDiv), "CP2114_SetPinConfig");
    }
    if( m_RamCfgSpecified)
    {
        AbortOnErr( CP2114_SetRamConfig( dev.handle(), const_cast<CP2114_RAM_CONFIG_STRUCT*>( &m_RamCfg)), "CP2114_SetRamConfig");
    }
    if( m_OtpSpecified)
    {
        AbortOnErr( CP2114_WriteOTP( dev.handle(), m_cp2114Address, const_cast<BYTE*>( m_Otp.data()), static_cast<UINT>( m_Otp.size())), "CP2114_WriteOTP");
    }
}
void CCP2114Parms::verify( const CCP2110Dev &dev, CSerNumSet &serNumSet) const
{
    CCP2110_4Parms::verify( dev, serNumSet);
    if( m_PinCfgSpecified)
    {
        std::vector<BYTE> pinConfig( m_PinCfg.size());
        BOOL useSuspendValues; WORD suspendValue; WORD suspendMode; BYTE clkDiv;
        AbortOnErr( CP2114_GetPinConfig( dev.handle(), pinConfig.data(), &useSuspendValues,
                                         &suspendValue, &suspendMode, &clkDiv), "CP2114_GetPinConfig");
        if( m_PinCfg           != pinConfig ||
            m_useSuspendValues != useSuspendValues ||
            m_suspendValue     != suspendValue ||
            m_suspendMode      != suspendMode ||
            m_clkDiv           != clkDiv)
        {
            throw CCustErr( "Failed PinConfig verification");
        }
    }
    if( m_RamCfgSpecified)
    {
        CP2114_RAM_CONFIG_STRUCT RamCfg;
        AbortOnErr( CP2114_GetRamConfig( dev.handle(), &RamCfg), "CP2114_GetRamConfig");
        if( m_RamCfg.Length != RamCfg.Length ||
            memcmp( &m_RamCfg.configData[ 0], &RamCfg.configData[ 0], m_RamCfg.Length))
        {
            throw CCustErr( "Failed RamConfig verification");
        }
    }
    if( m_OtpSpecified)
    {
        std::vector<BYTE> Otp( m_Otp.size());
        AbortOnErr( CP2114_ReadOTP( dev.handle(), m_cp2114Address, Otp.data(), static_cast<UINT>( m_Otp.size())), "CP2114_ReadOTP");
        if( m_Otp != Otp)
        {
            throw CCustErr( "Failed OTP verification");
        }
    }
}
//---------------------------------------------------------------------------------
void LibSpecificMain( const CDevType &devType, const CVidPid &vidPid, int argc, const char * argv[])
{
    if( devType.Value() == 0xa)
    {
        DevSpecificMain<CCP2110Dev,CCP2110Parms> ( devType, vidPid, argc, argv);
    }
    else if( devType.Value() == 0xe)
    {
        DevSpecificMain<CCP2110Dev,CCP2114Parms> ( devType, vidPid, argc, argv);
    }
    else
    {
        throw CSyntErr( "Unsupported PartNum");
    }
}
