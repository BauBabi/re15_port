import ctypes, sys
from ctypes import wintypes
user32=ctypes.windll.user32
pid=int(sys.argv[1]); res=[]
@ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
def cb(h,lp):
    p=wintypes.DWORD(); user32.GetWindowThreadProcessId(h,ctypes.byref(p))
    if p.value==pid and user32.IsWindowVisible(h):
        n=user32.GetWindowTextLengthW(h); b=ctypes.create_unicode_buffer(n+1); user32.GetWindowTextW(h,b,n+1)
        if b.value.startswith("RE1.5"): res.append(h)
    return True
user32.EnumWindows(cb,0)
if not res: sys.exit(1)
h=res[0]
if len(sys.argv)>2 and sys.argv[2]=="rect":
    r=wintypes.RECT(); user32.GetClientRect(h,ctypes.byref(r)); pt=wintypes.POINT(0,0); user32.ClientToScreen(h,ctypes.byref(pt))
    print(pt.x,pt.y,r.right,r.bottom)
else:
    print("0x%x"%h)
