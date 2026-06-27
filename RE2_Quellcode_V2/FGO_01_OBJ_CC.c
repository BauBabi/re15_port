/* FGO_01_OBJ_CC @ 0x8008e2c0  (Ghidra headless, raw MIPS overlay) */

void FGO_01_OBJ_CC(int param_1,undefined2 *param_2)

{
  short sVar1;
  int in_t0;
  int in_t1;
  int iVar2;
  int in_t3;
  int in_t4;
  int iVar3;
  undefined2 in_t6;
  undefined2 uVar4;
  int iVar5;
  
  sVar1 = *(short *)(param_1 + 4);
  param_2[2] = in_t6;
  param_2[5] = (short)(-(in_t1 * in_t3) >> 0xc);
  uVar4 = (undefined2)(in_t1 * in_t0 >> 0xc);
  if (-1 < sVar1) {
    param_2[8] = uVar4;
    iVar3 = (int)(short)*(int *)(&DAT_800adeac + ((int)sVar1 & 0xfffU) * 4);
    iVar2 = *(int *)(&DAT_800adeac + ((int)sVar1 & 0xfffU) * 4) >> 0x10;
    *param_2 = (short)(iVar2 * in_t1 >> 0xc);
    param_2[1] = (short)(-(iVar3 * in_t1) >> 0xc);
    iVar5 = iVar2 * in_t4 >> 0xc;
    param_2[3] = (short)(iVar3 * in_t0 >> 0xc) - (short)(iVar5 * in_t3 >> 0xc);
    param_2[6] = (short)(iVar3 * in_t3 >> 0xc) + (short)(iVar5 * in_t0 >> 0xc);
    iVar3 = iVar3 * in_t4 >> 0xc;
    param_2[4] = (short)(iVar2 * in_t0 >> 0xc) + (short)(iVar3 * in_t3 >> 0xc);
    param_2[7] = (short)(iVar2 * in_t3 >> 0xc) - (short)(iVar3 * in_t0 >> 0xc);
    return;
  }
  param_2[8] = uVar4;
  FGO_01_OBJ_160();
  return;
}


