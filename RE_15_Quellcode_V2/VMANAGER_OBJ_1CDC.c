void VMANAGER_OBJ_1CDC(int param_1,undefined2 param_2,undefined2 param_3)

{
  byte bVar1;
  short in_t0;
  
  *(undefined2 *)((int)&DAT_8008f6ae + (param_1 >> 0xf)) = param_3;
  bVar1 = (&DAT_8008f82c)[in_t0];
  *(undefined2 *)((int)&DAT_8008f6ac + (param_1 >> 0xf)) = param_2;
  (&DAT_8008f82c)[in_t0] = bVar1 | 3;
  return;
}
