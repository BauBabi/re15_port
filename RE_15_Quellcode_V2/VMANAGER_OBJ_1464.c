void VMANAGER_OBJ_1464(int param_1)

{
  int iVar1;
  int iVar2;
  short in_t1;
  short in_t2;
  short in_t3;
  
  iVar2 = (param_1 >> 0x10) - (int)in_t1;
  iVar1 = (int)in_t2;
  if (iVar1 == 0) {
    trap(0x1c00);
  }
  if ((iVar1 == -1) && (iVar2 == -0x80000000)) {
    trap(0x1800);
  }
  (&DAT_8008f864)[in_t3 * 0x1a] = 0;
  (&DAT_8008f862)[in_t3 * 0x1a] = (short)(iVar2 / iVar1);
  return;
}
