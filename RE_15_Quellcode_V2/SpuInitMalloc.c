long SpuInitMalloc(long num,char *top)

{
  uint uVar1;
  long lVar2;
  
  uVar1 = DAT_80076d80;
  if (num < 1) {
    lVar2 = S_M_INIT_OBJ_4C();
    return lVar2;
  }
  top[0] = '\x10';
  top[1] = '\x10';
  top[2] = '\0';
  top[3] = '@';
  DAT_80076db8 = 0;
  DAT_80076db4 = num;
  DAT_80076dbc = top;
  *(int *)(top + 4) = (0x10000 << (uVar1 & 0x1f)) + -0x1010;
  return num;
}
