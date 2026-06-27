/* FUN_80041b20 @ 0x80041b20  (Ghidra headless, raw MIPS overlay) */

int FUN_80041b20(int param_1,uint param_2,int *param_3,int *param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int in_t1;
  int iVar4;
  
  iVar1 = 0;
  uVar3 = param_2 & 1;
  while (uVar3 == 0) {
    param_2 = (param_2 & 0xff) >> 1;
    iVar1 = iVar1 + 1;
    uVar3 = param_2 & 1;
  }
  switch(iVar1) {
  case 0:
    in_t1 = ((int)*(short *)(param_1 + 0x10) - (int)*(short *)(param_1 + 0x12)) / 3;
    *param_3 = (int)*(short *)(param_1 + 0x10) + in_t1 * -2;
    iVar1 = (int)*(short *)(param_1 + 0x12) + in_t1 * -2;
    break;
  case 1:
    in_t1 = ((int)*(short *)(param_1 + 0x10) - (int)*(short *)(param_1 + 0x12)) / 3;
    *param_3 = *(short *)(param_1 + 0x10) - in_t1;
    iVar1 = *(short *)(param_1 + 0x12) - in_t1;
    break;
  case 2:
    iVar2 = (int)*(short *)(param_1 + 0x10) - (int)*(short *)(param_1 + 0x12);
    *param_3 = (int)*(short *)(param_1 + 0x10);
    iVar1 = (int)*(short *)(param_1 + 0x12);
    iVar4 = iVar2 / 3 + (iVar2 >> 0x1f);
    goto LAB_80041c50;
  case 3:
    iVar2 = (int)*(short *)(param_1 + 0xc) - (int)*(short *)(param_1 + 0xe);
    *param_3 = (int)*(short *)(param_1 + 0xc);
    iVar1 = (int)*(short *)(param_1 + 0xe);
    iVar4 = iVar2 / 3 + (iVar2 >> 0x1f);
    goto LAB_80041c50;
  case 4:
    iVar2 = (int)*(short *)(param_1 + 8) - (int)*(short *)(param_1 + 10);
    iVar4 = iVar2 / 3 + (iVar2 >> 0x1f);
    *param_3 = (int)*(short *)(param_1 + 8);
    iVar1 = (int)*(short *)(param_1 + 10);
LAB_80041c50:
    *param_4 = iVar1;
    return iVar4 - (iVar2 >> 0x1f);
  case 5:
    in_t1 = ((int)*(short *)(param_1 + 8) - (int)*(short *)(param_1 + 10)) / 3;
    *param_3 = *(short *)(param_1 + 8) + in_t1;
    iVar1 = *(short *)(param_1 + 10) + in_t1;
    break;
  case 6:
    in_t1 = ((int)*(short *)(param_1 + 8) - (int)*(short *)(param_1 + 10)) / 3;
    *param_3 = (int)*(short *)(param_1 + 8) + in_t1 * 2;
    iVar1 = (int)*(short *)(param_1 + 10) + in_t1 * 2;
    break;
  default:
    goto switchD_80041b6c_default;
  }
  *param_4 = iVar1;
switchD_80041b6c_default:
  return in_t1;
}


