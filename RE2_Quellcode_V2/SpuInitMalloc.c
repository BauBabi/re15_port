/* SpuInitMalloc @ 0x800826bc  (Ghidra headless, raw MIPS overlay) */

long SpuInitMalloc(long num,char *top)

{
  uint uVar1;
  long lVar2;
  
  uVar1 = DAT_800ab244;
  if (num < 1) {
    lVar2 = S_M_INIT_OBJ_4C();
    return lVar2;
  }
  top[0] = '\x10';
  top[1] = '\x10';
  top[2] = '\0';
  top[3] = '@';
  DAT_800ab6f0 = 0;
  DAT_800ab6ec = num;
  DAT_800ab6f4 = top;
  *(int *)(top + 4) = (0x10000 << (uVar1 & 0x1f)) + -0x1010;
  return num;
}


