void FGO_01_OBJ_64(int param_1,undefined2 *param_2)

{
  short sVar1;
  int in_t0;
  int iVar2;
  int iVar3;
  int in_t3;
  int iVar4;
  short sVar5;
  undefined2 uVar6;
  int iVar7;
  
  if (*(short *)(param_1 + 2) < 0) {
    FGO_01_OBJ_CC();
    return;
  }
  sVar5 = (short)*(int *)(&DAT_800794c4 + ((int)*(short *)(param_1 + 2) & 0xfffU) * 4);
  iVar7 = -(int)sVar5;
  iVar2 = *(int *)(&DAT_800794c4 + ((int)*(short *)(param_1 + 2) & 0xfffU) * 4) >> 0x10;
  sVar1 = *(short *)(param_1 + 4);
  param_2[2] = sVar5;
  param_2[5] = (short)(-(iVar2 * in_t3) >> 0xc);
  uVar6 = (undefined2)(iVar2 * in_t0 >> 0xc);
  if (-1 < sVar1) {
    param_2[8] = uVar6;
    iVar4 = (int)(short)*(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4);
    iVar3 = *(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4) >> 0x10;
    *param_2 = (short)(iVar3 * iVar2 >> 0xc);
    param_2[1] = (short)(-(iVar4 * iVar2) >> 0xc);
    iVar2 = iVar3 * iVar7 >> 0xc;
    param_2[3] = (short)(iVar4 * in_t0 >> 0xc) - (short)(iVar2 * in_t3 >> 0xc);
    param_2[6] = (short)(iVar4 * in_t3 >> 0xc) + (short)(iVar2 * in_t0 >> 0xc);
    iVar7 = iVar4 * iVar7 >> 0xc;
    param_2[4] = (short)(iVar3 * in_t0 >> 0xc) + (short)(iVar7 * in_t3 >> 0xc);
    param_2[7] = (short)(iVar3 * in_t3 >> 0xc) - (short)(iVar7 * in_t0 >> 0xc);
    return;
  }
  param_2[8] = uVar6;
  FGO_01_OBJ_160();
  return;
}
