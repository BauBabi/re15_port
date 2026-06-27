/* DecDCTvlcSize @ 0x80094fd4  (Ghidra headless, raw MIPS overlay) */

int DecDCTvlcSize(int size)

{
  int iVar1;
  
  iVar1 = VLC_OBJ_0;
  if (0 < size + -1) {
    VLC_OBJ_0 = size << 1;
    return iVar1;
  }
  VLC_OBJ_0 = 0xffffff;
  return iVar1;
}


