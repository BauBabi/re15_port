import subprocess, time, os, sys
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
state, out, secs = sys.argv[1:4]
p = subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE])
time.sleep(12)
subprocess.run(["C:/ProgramData/chocolatey/bin/ffmpeg","-hide_banner","-loglevel","error","-y",
    "-f","gdigrab","-framerate","30","-offset_x","0","-offset_y","48","-video_size","600x450",
    "-i","desktop","-t",secs,"-c:v","libx264rgb","-qp","0","-preset","ultrafast",out])
subprocess.run(["taskkill","/IM","duckstation-qt-x64-ReleaseLTCG.exe"],capture_output=True)
for _ in range(40):
    time.sleep(0.5)
    if p.poll() is not None: break
print("exit", p.poll())
