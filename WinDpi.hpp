#pragma once

#ifndef _INC_WINDOWS
    #include <windows.h>
#endif

#ifndef DEFAULT_DPI
    #define DEFAULT_DPI 96
#endif

#ifndef AutoFreeLibrary
    struct AutoFreeLibrary
    {
        HINSTANCE m_hInst;
        AutoFreeLibrary(HINSTANCE hInst) : m_hInst(hInst)
        {
        }
        ~AutoFreeLibrary()
        {
            FreeLibrary(m_hInst);
        }
        operator HINSTANCE() const
        {
            return m_hInst;
        }
    };
    #define AutoFreeLibrary AutoFreeLibrary
#endif

struct WinDpi
{
    HWND m_hwnd;
    UINT m_uDpiX, m_uDpiY;
    INT m_nDpiType;
    AutoFreeLibrary m_hSHCORE;
    typedef HRESULT (WINAPI *FN_GetDpiForMonitor)(HMONITOR, INT, UINT *, UINT *);
    typedef HRESULT (WINAPI *FN_SetProcessDpiAwareness)(INT value);
    typedef BOOL (WINAPI* FN_EnableNonClientDpiScaling)(HWND);
    FN_GetDpiForMonitor m_pGetDpiForMonitor;
    FN_SetProcessDpiAwareness m_pSetProcessDpiAwareness;

    WinDpi(HWND hwnd, INT nAware = 1 /* PROCESS_SYSTEM_DPI_AWARE */, INT nDpiType = 0)
        : m_hwnd(hwnd)
        , m_uDpiX(DEFAULT_DPI)
        , m_uDpiY(DEFAULT_DPI)
        , m_hSHCORE(::LoadLibraryW(L"SHCORE"))
        , m_pGetDpiForMonitor(reinterpret_cast<FN_GetDpiForMonitor>(::GetProcAddress(m_hSHCORE, "GetDpiForMonitor")))
        , m_pSetProcessDpiAwareness(reinterpret_cast<FN_SetProcessDpiAwareness>(::GetProcAddress(m_hSHCORE, "SetProcessDpiAwareness")))
    {
        if (nAware > 0)
            SetDpiAware(nAware);
        if (m_pGetDpiForMonitor && nAware == 2 /* PROCESS_PER_MONITOR_DPI_AWARE */)
        {
            if (HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST))
            {
                HRESULT hr = (*m_pGetDpiForMonitor)(hMonitor, nDpiType, &m_uDpiX, &m_uDpiY);
                if (SUCCEEDED(hr))
                    return;
            }
        }
        if (HDC hDC = ::GetDC(hwnd))
        {
            m_uDpiX = ::GetDeviceCaps(hDC, LOGPIXELSX);
            m_uDpiY = ::GetDeviceCaps(hDC, LOGPIXELSY);
            ::ReleaseDC(hwnd, hDC);
            return;
        }
        m_uDpiX = m_uDpiY = DEFAULT_DPI;
    }
    UINT GetDpi() const
    {
        return m_uDpiY;
    }
    UINT GetDpiX() const
    {
        return m_uDpiX;
    }
    UINT GetDpiY() const
    {
        return m_uDpiY;
    }
    INT PointToPixel(INT pt) const
    {
        return ::MulDiv(pt, m_uDpiY, 72);
    }
    INT PixelToPoint(INT px) const
    {
        return ::MulDiv(px, 72, m_uDpiY);
    }
    BOOL SetDpiAware(INT nAware = 1 /* PROCESS_SYSTEM_DPI_AWARE */)
    {
        if (m_pSetProcessDpiAwareness)
            return SUCCEEDED((*m_pSetProcessDpiAwareness)(nAware));
        return FALSE;
    }
};
