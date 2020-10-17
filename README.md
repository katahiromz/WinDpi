# WinDpi

Usage:

```cpp
WinDpi windpi(hwnd);
LOGFONTW lf;
ZeroMemory(&lf, sizeof(lf));
lf.lfHeight = windpi.PointToPixel(12);
lstrcpyW(lf.lfFaceName, L"MS UI Gothic");
HFONT hFont = CreateFontIndirectW(&lf);
...
MoveWindow(hwndItem, 0, 0, windpi.PointToPixel(20), windpi.PointToPixel(20), TRUE);
...
```
