/* LoadClut2 @ 0x8008f690  (Ghidra headless, raw MIPS overlay) */

u_short LoadClut2(u_long *clut,int x,int y)

{
  u_short uVar1;
  RECT local_18;
  
  local_18.w = 0x10;
  local_18.x = (short)x;
  local_18.y = (short)y;
  local_18.h = 1;
  LoadImage(&local_18,clut);
  uVar1 = GetClut(x,y);
  return uVar1;
}


