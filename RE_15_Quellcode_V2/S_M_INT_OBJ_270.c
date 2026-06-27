void S_M_INT_OBJ_270(void)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  int iVar4;
  int iVar5;
  
  iVar1 = DAT_80076dbc;
  iVar4 = DAT_80076db8 + -1;
  if (-1 < iVar4) {
    puVar3 = (uint *)(iVar4 * 8 + DAT_80076dbc);
    do {
      if ((*puVar3 & 0x80000000) == 0) {
        return;
      }
      *puVar3 = *puVar3 & 0xfffffff | 0x40000000;
      iVar2 = DAT_80076db8 * 8;
      iVar5 = iVar4 + -1;
      DAT_80076db8 = iVar4;
      puVar3[1] = puVar3[1] + *(int *)(iVar2 + iVar1 + 4);
      puVar3 = puVar3 + -2;
      iVar4 = iVar5;
    } while (-1 < iVar5);
  }
  return;
}
