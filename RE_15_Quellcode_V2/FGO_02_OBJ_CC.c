void FGO_02_OBJ_CC(int param_1,short *param_2)

{
  short sVar1;
  int in_t0;
  int in_t1;
  int iVar2;
  int in_t3;
  int in_t4;
  int iVar3;
  short in_t6;
  short sVar4;
  int iVar5;
  
  sVar1 = *(short *)(param_1 + 4);
  param_2[5] = in_t6;
  param_2[2] = (short)(in_t4 * in_t0 >> 0xc);
  sVar4 = (short)(in_t1 * in_t0 >> 0xc);
  if (-1 < sVar1) {
    param_2[8] = sVar4;
    iVar3 = (int)(short)*(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4);
    iVar2 = *(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4) >> 0x10;
    param_2[3] = (short)(iVar3 * in_t0 >> 0xc);
    param_2[4] = (short)(iVar2 * in_t0 >> 0xc);
    iVar5 = in_t4 * in_t3 >> 0xc;
    *param_2 = (short)(in_t1 * iVar2 >> 0xc) + (short)(iVar5 * iVar3 >> 0xc);
    param_2[1] = (short)(iVar5 * iVar2 >> 0xc) - (short)(in_t1 * iVar3 >> 0xc);
    iVar5 = in_t1 * in_t3 >> 0xc;
    param_2[7] = (short)(in_t4 * iVar3 >> 0xc) + (short)(iVar5 * iVar2 >> 0xc);
    param_2[6] = (short)(iVar5 * iVar3 >> 0xc) - (short)(in_t4 * iVar2 >> 0xc);
    return;
  }
  param_2[8] = sVar4;
  FGO_02_OBJ_160();
  return;
}
