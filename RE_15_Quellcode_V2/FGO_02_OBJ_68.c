void FGO_02_OBJ_68(int param_1,short *param_2)

{
  short sVar1;
  int in_t0;
  int iVar2;
  int iVar3;
  int in_t3;
  int iVar4;
  int iVar5;
  short in_t6;
  short sVar6;
  int iVar7;
  
  if (*(short *)(param_1 + 2) < 0) {
    FGO_02_OBJ_CC();
    return;
  }
  iVar4 = (int)(short)*(int *)(&DAT_800794c4 + ((int)*(short *)(param_1 + 2) & 0xfffU) * 4);
  iVar2 = *(int *)(&DAT_800794c4 + ((int)*(short *)(param_1 + 2) & 0xfffU) * 4) >> 0x10;
  sVar1 = *(short *)(param_1 + 4);
  param_2[5] = in_t6;
  param_2[2] = (short)(iVar4 * in_t0 >> 0xc);
  sVar6 = (short)(iVar2 * in_t0 >> 0xc);
  if (-1 < sVar1) {
    param_2[8] = sVar6;
    iVar5 = (int)(short)*(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4);
    iVar3 = *(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4) >> 0x10;
    param_2[3] = (short)(iVar5 * in_t0 >> 0xc);
    param_2[4] = (short)(iVar3 * in_t0 >> 0xc);
    iVar7 = iVar4 * in_t3 >> 0xc;
    *param_2 = (short)(iVar2 * iVar3 >> 0xc) + (short)(iVar7 * iVar5 >> 0xc);
    param_2[1] = (short)(iVar7 * iVar3 >> 0xc) - (short)(iVar2 * iVar5 >> 0xc);
    iVar2 = iVar2 * in_t3 >> 0xc;
    param_2[7] = (short)(iVar4 * iVar5 >> 0xc) + (short)(iVar2 * iVar3 >> 0xc);
    param_2[6] = (short)(iVar2 * iVar5 >> 0xc) - (short)(iVar4 * iVar3 >> 0xc);
    return;
  }
  param_2[8] = sVar6;
  FGO_02_OBJ_160();
  return;
}
