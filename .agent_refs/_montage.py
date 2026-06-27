import sys, glob, os
from PIL import Image
clip = sys.argv[1]
d = f"c:/workspace/git/reAi/.agent_refs/chanB/clip{clip}"
files = sorted(glob.glob(d+"/*.png"))
# pick up to 6 evenly spaced
if len(files) > 6:
    idx = [int(i*(len(files)-1)/5) for i in range(6)]
    files = [files[i] for i in idx]
ims = [Image.open(f) for f in files]
w = max(i.width for i in ims); h = max(i.height for i in ims)
n = len(ims)
mont = Image.new("RGB",(w*n, h),(0,0,0))
for k,im in enumerate(ims):
    mont.paste(im,(k*w,0))
out = f"c:/workspace/git/reAi/.agent_refs/chanB_montage_clip{clip}.png"
mont.save(out)
print(out, mont.size, "frames:", [os.path.basename(f) for f in files])
