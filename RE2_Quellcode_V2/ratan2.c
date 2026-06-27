/* ratan2 @ 0x8008f31c  (Ghidra headless, raw MIPS overlay) */

long ratan2(long y,long x)

{
  bool bVar1;
  bool bVar2;
  long lVar3;
  int iVar4;
  
  bVar1 = x < 0;
  if (bVar1) {
    x = -x;
  }
  bVar2 = y < 0;
  if (bVar2) {
    y = -y;
  }
  if ((x == 0) && (y == 0)) {
    lVar3 = RATAN_OBJ_178();
    return lVar3;
  }
  if (x <= y) {
    iVar4 = y >> 10;
    if ((x & 0x7fe00000U) != 0) {
      if (iVar4 == 0) {
        trap(0x1c00);
      }
      if ((iVar4 == -1) && (x == 0x80000000)) {
        trap(0x1800);
      }
      lVar3 = RATAN_OBJ_148(x / iVar4);
      return lVar3;
    }
    if (y == 0) {
      trap(0x1c00);
    }
    if ((y == 0xffffffff) && (x << 10 == -0x80000000)) {
      trap(0x1800);
    }
    iVar4 = 0x400 - *(short *)(&DAT_800b1eac + ((x << 10) / y) * 2);
    if (bVar1) {
      iVar4 = 0x800 - iVar4;
    }
    if (bVar2) {
      iVar4 = -iVar4;
    }
    return iVar4;
  }
  iVar4 = x >> 10;
  if ((y & 0x7fe00000U) != 0) {
    if (iVar4 == 0) {
      trap(0x1c00);
    }
    if ((iVar4 == -1) && (y == 0x80000000)) {
      trap(0x1800);
    }
    lVar3 = RATAN_OBJ_BC(y / iVar4);
    return lVar3;
  }
  if (x == 0) {
    trap(0x1c00);
  }
  if ((x == 0xffffffff) && ((y & 0x3fffffU) == 0x200000)) {
    trap(0x1800);
  }
  lVar3 = RATAN_OBJ_15C();
  return lVar3;
}


