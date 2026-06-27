/* FUN_8004fa28 @ 0x8004fa28  (Ghidra headless, raw MIPS overlay) */

int FUN_8004fa28(int *param_1,int param_2,int param_3,short *param_4)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  int in_t0;
  int iVar4;
  
  uVar3 = ((ushort)param_4[5] & 0x30) >> 4;
  if (uVar3 == 1) {
    sVar1 = *param_4;
    iVar2 = *param_1;
LAB_8004fadc:
    iVar2 = iVar2 - sVar1;
    iVar4 = (int)((ulonglong)((longlong)iVar2 * -0xd4629b7) >> 0x20);
  }
  else if (uVar3 < 2) {
    if (uVar3 != 0) {
      param_2 = param_2 - param_3;
      goto LAB_8004fb14;
    }
    iVar2 = ((int)*param_4 + (uint)(ushort)param_4[2]) - *param_1;
    iVar4 = (int)((ulonglong)((longlong)iVar2 * -0xd4629b7) >> 0x20);
  }
  else {
    if (uVar3 != 2) {
      if (uVar3 != 3) {
        param_2 = param_2 - param_3;
        goto LAB_8004fb14;
      }
      sVar1 = param_4[1];
      iVar2 = param_1[2];
      goto LAB_8004fadc;
    }
    iVar2 = ((int)param_4[1] + (uint)(ushort)param_4[3]) - param_1[2];
    iVar4 = (int)((ulonglong)((longlong)iVar2 * -0xd4629b7) >> 0x20);
  }
  in_t0 = ((iVar4 + iVar2 >> 9) - (iVar2 >> 0x1f)) * 0x1c2;
  param_2 = param_2 - param_3;
LAB_8004fb14:
  if (param_2 < in_t0) {
    in_t0 = param_2;
  }
  if (in_t0 < 0) {
    in_t0 = 0;
  }
  return in_t0;
}


