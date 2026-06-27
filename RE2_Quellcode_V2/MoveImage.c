/* MoveImage @ 0x8009072c  (Ghidra headless, raw MIPS overlay) */

int MoveImage(RECT *rect,int x,int y)

{
  int iVar1;
  
  checkRECT("MoveImage",rect);
  iVar1 = -1;
  if (rect->w != 0) {
    if (rect->h == 0) {
      iVar1 = SYS_OBJ_8F4();
      return iVar1;
    }
    DAT_800b27b4 = y << 0x10 | x & 0xffffU;
    DAT_800b27b0._0_2_ = rect->x;
    DAT_800b27b0._2_2_ = rect->y;
    DAT_800b27b8._0_2_ = rect->w;
    DAT_800b27b8._2_2_ = rect->h;
    iVar1 = (**(code **)(PTR_PTR_800b26f8 + 8))
                      (*(undefined4 *)(PTR_PTR_800b26f8 + 0x18),&DAT_800b27a8,0x14,0);
  }
  return iVar1;
}


