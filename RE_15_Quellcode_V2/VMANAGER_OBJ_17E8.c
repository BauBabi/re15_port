void VMANAGER_OBJ_17E8(int param_1,undefined2 param_2,undefined2 param_3,short param_4)

{
  byte bVar1;
  
  *(undefined2 *)((int)&DAT_8008f6ae + (param_1 >> 0xf)) = param_3;
  bVar1 = (&DAT_8008f82c)[param_4];
  *(undefined2 *)((int)&DAT_8008f6ac + (param_1 >> 0xf)) = param_2;
  (&DAT_8008f82c)[param_4] = bVar1 | 3;
  return;
}
