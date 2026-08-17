# =============================================================================
# RE1.5 Port - Fenster-Diagnose
# =============================================================================
# Startet re15_pc.exe und listet ALLE Fenster des Prozesses (auch unsichtbare,
# minimierte und solche ausserhalb des Bildschirms) samt Position/Groesse.
# Hintergrund: "Prozess laeuft, aber kein Fenster" kann dreierlei bedeuten -
#   (a) es gibt gar kein Fenster (Haengen vor SDL_CreateWindow),
#   (b) es gibt eins, aber es ist unsichtbar/minimiert,
#   (c) es gibt eins, es liegt aber ausserhalb des sichtbaren Desktops.
# Nur (a) ist ein Engine-Haenger; (b)/(c) sind Fenster-/Positions-Probleme.
#
# Aufruf:  powershell -ExecutionPolicy Bypass -File tools\diag_window.ps1
#          powershell -ExecutionPolicy Bypass -File tools\diag_window.ps1 -Exe "C:\pfad\re15_pc.exe"
# =============================================================================
param(
    [string]$Exe = "",
    [int]$WaitSeconds = 10
)

$ErrorActionPreference = "Continue"

if (-not $Exe) {
    $cands = @(
        "$PSScriptRoot\..\re15_port\build\platform\pc\re15_pc.exe",
        "$PSScriptRoot\..\release\win_out\re15_pc.exe"
    )
    foreach ($c in $cands) { if (Test-Path $c) { $Exe = (Resolve-Path $c).Path; break } }
}
if (-not $Exe -or -not (Test-Path $Exe)) { Write-Output "FEHLER: exe nicht gefunden. -Exe <pfad> angeben."; exit 1 }

$dir = Split-Path -Parent $Exe
Write-Output "== EXE:  $Exe"
Write-Output "== CWD:  $dir"
Write-Output ("== Datum der exe: " + (Get-Item $Exe).LastWriteTime)

$sig = @'
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
public class WinEnum {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc cb, IntPtr p);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr h);
    [DllImport("user32.dll")] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern int GetClassNameW(IntPtr h, StringBuilder s, int n);
    public delegate bool EnumWindowsProc(IntPtr h, IntPtr p);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
    public static List<string> ForPid(uint want) {
        var res = new List<string>();
        EnumWindows((h, p) => {
            uint pid; GetWindowThreadProcessId(h, out pid);
            if (pid != want) return true;
            var t = new StringBuilder(256); GetWindowTextW(h, t, 256);
            var c = new StringBuilder(256); GetClassNameW(h, c, 256);
            RECT r; GetWindowRect(h, out r);
            res.Add(string.Format("hwnd=0x{0:X}  class='{1}'  titel='{2}'  sichtbar={3} minimiert={4}  rect=({5},{6})-({7},{8}) groesse={9}x{10}",
                h.ToInt64(), c, t, IsWindowVisible(h), IsIconic(h), r.L, r.T, r.R, r.B, r.R - r.L, r.B - r.T));
            return true;
        }, IntPtr.Zero);
        return res;
    }
}
'@
Add-Type -TypeDefinition $sig -ErrorAction SilentlyContinue

Get-Process re15_pc -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Start-Sleep -Milliseconds 400
Remove-Item (Join-Path $dir "debug.log") -ErrorAction SilentlyContinue

$p = Start-Process -FilePath $Exe -WorkingDirectory $dir -PassThru
Write-Output ("== gestartet, PID " + $p.Id + " - warte $WaitSeconds s ...")
Start-Sleep -Seconds $WaitSeconds
$p.Refresh()

Write-Output ""
Write-Output ("PROZESS: lebt=" + (-not $p.HasExited) + "  Threads=" + $p.Threads.Count + "  CPU=" + $p.CPU + "  MainWindowHandle=" + $p.MainWindowHandle)

Write-Output ""
Write-Output "FENSTER DES PROZESSES (auch unsichtbare):"
$wins = [WinEnum]::ForPid([uint32]$p.Id)
if ($wins.Count -eq 0) { Write-Output "  KEINE - der Prozess hat kein einziges Fenster erzeugt." }
else { foreach ($w in $wins) { Write-Output "  $w" } }

Write-Output ""
Write-Output "BILDSCHIRME:"
Add-Type -AssemblyName System.Windows.Forms
foreach ($s in [System.Windows.Forms.Screen]::AllScreens) {
    Write-Output ("  " + $s.DeviceName + "  bounds=" + $s.Bounds.ToString() + "  primaer=" + $s.Primary)
}

Write-Output ""
Write-Output "DEBUG.LOG (letzte 25 Zeilen):"
$log = Join-Path $dir "debug.log"
if (Test-Path $log) { Get-Content $log -Tail 25 | ForEach-Object { Write-Output "  $_" } }
else { Write-Output "  (keine debug.log geschrieben!)" }

Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue
Write-Output ""
Write-Output "== fertig (Prozess beendet)"
