# fenster.ps1 <pid> : sichtbares Hauptfenster GENAU dieses Prozesses -> "hwnd x y w h" (Client-Bereich, physische Pixel)
param([int]$ProcId)
Add-Type @"
using System; using System.Runtime.InteropServices; using System.Text;
public class W {
  public delegate bool EnumProc(IntPtr h, IntPtr l);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc f, IntPtr l);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
  [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
}
"@
[W]::SetProcessDPIAware() | Out-Null
$script:found = $null
$cb = [W+EnumProc]{ param($h, $l)
  $p = 0; [W]::GetWindowThreadProcessId($h, [ref]$p) | Out-Null
  if ($p -eq $ProcId -and [W]::IsWindowVisible($h)) {
    $r = New-Object W+RECT; [W]::GetClientRect($h, [ref]$r) | Out-Null
    if ($r.R -gt 100) { $script:found = $h; return $false }
  }
  return $true }
[W]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
if ($script:found) {
  $r = New-Object W+RECT; [W]::GetClientRect($script:found, [ref]$r) | Out-Null
  $pt = New-Object W+POINT; [W]::ClientToScreen($script:found, [ref]$pt) | Out-Null
  "0x{0:x} {1} {2} {3} {4}" -f $script:found.ToInt64(), $pt.X, $pt.Y, ($r.R - $r.L), ($r.B - $r.T)
}
