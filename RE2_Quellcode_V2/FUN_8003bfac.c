/* FUN_8003bfac @ 0x8003bfac  (Ghidra headless, raw MIPS overlay) */

void FUN_8003bfac(int param_1)

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  
  if (DAT_800cfbdc < 0) {
    return;
  }
  DAT_800cfbd8 = DAT_800cfbd8 & 0xffffff7f;
  DAT_800cfbf6 = DAT_800cfbf6 & 0xffe0;
  if ((*(byte *)(param_1 + 0x1d3) & 0x7f) != 0) {
    *(byte *)(param_1 + 0x1d3) = *(byte *)(param_1 + 0x1d3) - 1;
  }
  if (*(char *)(param_1 + 8) != '\r') goto LAB_8003c12c;
  iVar5 = (int)*(short *)(param_1 + 0x162);
  iVar3 = (int)*(short *)(param_1 + 0x156) << 0xf;
  if (iVar5 == 0) {
    trap(0x1c00);
  }
  if ((iVar5 == -1) && (iVar3 == -0x80000000)) {
    trap(0x1800);
  }
  iVar6 = *(int *)(param_1 + 0x198);
  uVar4 = (uint)(iVar3 / iVar5) >> 8;
  *(uint *)(iVar6 + 0x70) = (uVar4 << 0x10 | uVar4 << 8) + 0x80;
  if (*(short *)(param_1 + 0x156) < 0) {
    *(undefined4 *)(iVar6 + 0x70) = 0x80;
  }
  if (DAT_800cfe10 == 0) {
    if ((DAT_800a4005 != 0) && (bVar2 = DAT_800a4005 - 1, (*(byte *)(param_1 + 0x14d) & 1) == 0))
    goto LAB_8003c10c;
  }
  else {
    if ((*(byte *)(param_1 + 0x14d) & 1) == 0) {
      DAT_800a4005 = DAT_800a4005 + 1;
    }
    bVar2 = 0x40;
    if (0x40 < DAT_800a4005) {
LAB_8003c10c:
      DAT_800a4005 = bVar2;
    }
  }
  *(uint *)(iVar6 + 0x70) = *(int *)(iVar6 + 0x70) + (uint)DAT_800a4005 * 0x10000;
LAB_8003c12c:
  if ((DAT_800cfe10 != 0) && (bVar1 = DAT_800cfe12 == '\0', DAT_800cfe12 = DAT_800cfe12 + -1, bVar1)
     ) {
    DAT_800cfe12 = '\x1e';
    if (1 < *(short *)(param_1 + 0x156)) {
      *(short *)(param_1 + 0x156) = *(short *)(param_1 + 0x156) + -1;
    }
  }
  (*(code *)(&PTR_LAB_800a4030)[*(byte *)(param_1 + 4)])
            (param_1,*(undefined4 *)(param_1 + 0x108),*(undefined4 *)(param_1 + 0x17c));
  FUN_8003db38(param_1,7000,0x5dc);
  FUN_800177c0(param_1,(int)*(short *)(param_1 + 0x76));
  if ((*(ushort *)(param_1 + 0x10e) & 0xfff) == 0xc) {
    FUN_80046304(param_1);
  }
  FUN_800428cc(param_1);
  FUN_800429c0(param_1);
  FUN_80042644(param_1);
  return;
}


