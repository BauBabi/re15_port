void FGO_02_OBJ_160(undefined4 param_1,short *param_2)

{
  int in_t0;
  int in_t1;
  int in_t2;
  int in_t3;
  int in_t4;
  int in_t5;
  int iVar1;
  
  param_2[3] = (short)(in_t5 * in_t0 >> 0xc);
  param_2[4] = (short)(in_t2 * in_t0 >> 0xc);
  iVar1 = in_t4 * in_t3 >> 0xc;
  *param_2 = (short)(in_t1 * in_t2 >> 0xc) + (short)(iVar1 * in_t5 >> 0xc);
  param_2[1] = (short)(iVar1 * in_t2 >> 0xc) - (short)(in_t1 * in_t5 >> 0xc);
  iVar1 = in_t1 * in_t3 >> 0xc;
  param_2[7] = (short)(in_t4 * in_t5 >> 0xc) + (short)(iVar1 * in_t2 >> 0xc);
  param_2[6] = (short)(iVar1 * in_t5 >> 0xc) - (short)(in_t4 * in_t2 >> 0xc);
  return;
}
