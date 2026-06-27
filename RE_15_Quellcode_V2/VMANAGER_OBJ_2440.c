undefined4 VMANAGER_OBJ_2440(undefined4 param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 uVar1;
  short in_t2;
  int iVar2;
  
  iVar2 = (int)in_t2;
  DAT_800b5320 = *(undefined1 *)(&DAT_8008f858 + iVar2 * 0x1a);
  DAT_800b532e = in_t2;
  uVar1 = note2pitch2(param_3,param_2);
  (&DAT_8008f6b0)[iVar2 * 8] = uVar1;
  (&DAT_8008f82c)[iVar2] = (&DAT_8008f82c)[iVar2] | 4;
  return 1;
}
