import subprocess, time, os, sys
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
state = sys.argv[1]; out = sys.argv[2]
p = subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE])
time.sleep(15)
subprocess.run(["C:/ProgramData/chocolatey/bin/ffmpeg","-hide_banner","-loglevel","error","-y","-f","gdigrab","-framerate","1","-i","desktop","-frames:v","1",out])
r = subprocess.run(["powershell","-NoProfile","-Command","Get-Process duckstation* | Select-Object Id,MainWindowTitle | Format-List"],capture_output=True,text=True)
print(r.stdout)
subprocess.run(["taskkill","/IM","duckstation-qt-x64-ReleaseLTCG.exe"],capture_output=True)
for _ in range(40):
    time.sleep(0.5)
    if p.poll() is not None: break
print("exit", p.poll())
