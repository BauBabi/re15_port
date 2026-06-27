void FGO_06_OBJ_64(undefined4 param_1,short *param_2)

{
  short sVar1;
  short sVar2;
  short sVar3;
  int in_t0;
  int in_t1;
  
  sVar1 = *param_2;
  sVar2 = param_2[1];
  sVar3 = param_2[2];
  *param_2 = (short)(in_t0 * sVar1 - in_t1 * param_2[3] >> 0xc);
  param_2[1] = (short)(in_t0 * sVar2 - in_t1 * param_2[4] >> 0xc);
  param_2[2] = (short)(in_t0 * sVar3 - in_t1 * param_2[5] >> 0xc);
  param_2[3] = (short)(in_t1 * sVar1 + in_t0 * param_2[3] >> 0xc);
  param_2[4] = (short)(in_t1 * sVar2 + in_t0 * param_2[4] >> 0xc);
  param_2[5] = (short)(in_t1 * sVar3 + in_t0 * param_2[5] >> 0xc);
  return;
}
