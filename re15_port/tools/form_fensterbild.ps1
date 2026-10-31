# form_fensterbild.ps1 -Hwnd <dezimal> -Aus <datei.png>
# Fensterinhalt von aussen holen, wenn gdigrab nicht geht: PrintWindow mit PW_RENDERFULLCONTENT (Flag 2) laesst die
# Fensterverwaltung den D3D11-Inhalt selbst zusammensetzen. Kein Readback im Spielprozess, also nicht RE15_AUTOSHOT.
# Gebraucht in getrennten Sitzungen (qwinsta Status Getr.): dort meldet gdigrab "Failed to capture image (error 5)" fuer den
# Desktop und liefert fuer hwnd= schwarze oder unbrauchbare Bilder (gemessen 2026-10-31). Auch PrintWindow lieferte einmal ein
# schwarzes Bild, deshalb prueft form_spiel.sh die Helligkeit und wiederholt.
param([long]$Hwnd, [string]$Aus)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class R41Fb {
  [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint flags);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
  [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
}
"@
$h = [IntPtr]$Hwnd
$w = New-Object R41Fb+RECT
[void][R41Fb]::GetWindowRect($h, [ref]$w)
$c = New-Object R41Fb+RECT
[void][R41Fb]::GetClientRect($h, [ref]$c)
$bw = $w.R - $w.L; $bh = $w.B - $w.T
if ($bw -le 0 -or $bh -le 0) { "Fenster ohne Flaeche"; exit 1 }
$bmp = New-Object System.Drawing.Bitmap($bw, $bh, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$dc = $g.GetHdc()
$ok = [R41Fb]::PrintWindow($h, $dc, 2)
$g.ReleaseHdc($dc); $g.Dispose()
# Nur die Innenflaeche: Rahmen und Titelleiste gehoeren nicht zum Bild des Spiels
$p = New-Object R41Fb+POINT
[void][R41Fb]::ClientToScreen($h, [ref]$p)
$ox = $p.X - $w.L; $oy = $p.Y - $w.T
$innen = $bmp.Clone((New-Object System.Drawing.Rectangle($ox, $oy, $c.R, $c.B)), $bmp.PixelFormat)
$innen.Save($Aus, [System.Drawing.Imaging.ImageFormat]::Png)
$innen.Dispose(); $bmp.Dispose()
"PrintWindow=$ok Fenster=${bw}x${bh} Innen=$($c.R)x$($c.B) Versatz=$ox,$oy"
