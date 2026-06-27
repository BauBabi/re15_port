void FUN_8001e1c8(uint param_1,undefined2 param_2,int param_3,undefined1 param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  short *psVar4;
  int iVar5;
  int iVar6;
  undefined2 local_50 [2];
  undefined2 local_4c;
  undefined2 local_48;
  undefined2 local_44;
  undefined2 local_40 [2];
  undefined2 local_3c;
  undefined2 local_38;
  undefined2 local_34;
  short local_30 [2];
  short local_2c;
  
  param_1 = param_1 & 0xffff;
  iVar5 = param_1 + 1;
  psVar4 = &DAT_80072d3c;
  iVar3 = 0;
  uVar2 = 0;
  do {
    if (param_3 == 0) {
      if (iVar5 == 0) {
        trap(0x1c00);
      }
      if ((iVar5 == -1) && (*psVar4 * 0x11 == -0x80000000)) {
        trap(0x1800);
      }
      local_30[0] = (short)((*psVar4 * 0x11) / iVar5);
      iVar6 = *(short *)((int)&DAT_80072d44 + iVar3) * 0x11;
      if (iVar5 == 0) {
        trap(0x1c00);
      }
      if ((iVar5 == -1) && (iVar6 == -0x80000000)) {
        trap(0x1800);
      }
      local_2c = (short)(iVar6 / iVar5);
    }
    else {
      iVar6 = (int)*psVar4 * param_1;
      if (iVar6 < 0) {
        iVar6 = iVar6 + 0xf;
      }
      local_30[0] = (short)(iVar6 >> 4);
      iVar6 = (int)*(short *)((int)&DAT_80072d44 + iVar3) * param_1;
      if (iVar6 < 0) {
        iVar6 = iVar6 + 0xf;
      }
      local_2c = (short)(iVar6 >> 4);
    }
    FUN_8004f008(param_2,local_30,local_30);
    psVar4 = psVar4 + 1;
    *(int *)(local_50 + uVar2 * 2) = local_30[0] + 0xa0;
    uVar1 = uVar2 + 1;
    *(int *)(local_40 + uVar2 * 2) = local_2c + 0x6c;
    iVar3 = iVar3 + 2;
    uVar2 = uVar1;
  } while (uVar1 < 4);
  uVar2 = 0;
  iVar3 = 0;
  do {
    iVar5 = iVar3 + DAT_8008f640;
    *(undefined2 *)(iVar5 + 8) = local_50[0];
    *(undefined2 *)(iVar5 + 10) = local_40[0];
    *(undefined2 *)(iVar5 + 0x10) = local_4c;
    *(undefined2 *)(iVar5 + 0x12) = local_3c;
    *(undefined2 *)(iVar5 + 0x18) = local_48;
    *(undefined2 *)(iVar5 + 0x1a) = local_38;
    uVar2 = uVar2 + 1;
    *(undefined2 *)(iVar5 + 0x20) = local_44;
    *(undefined1 *)(iVar5 + 6) = param_4;
    *(undefined1 *)(iVar5 + 5) = param_4;
    *(undefined1 *)(iVar5 + 4) = param_4;
    *(undefined2 *)(iVar5 + 0x22) = local_34;
    iVar3 = iVar3 + 0x28;
  } while (uVar2 < 2);
  return;
}
