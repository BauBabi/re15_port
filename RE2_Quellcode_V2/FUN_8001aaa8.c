/* FUN_8001aaa8 @ 0x8001aaa8  (Ghidra headless, raw MIPS overlay) */

int FUN_8001aaa8(undefined4 param_1,int param_2,int *param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  
  uVar2 = (uint)*(byte *)(param_2 + 8);
  if (uVar2 - 0x50 < 0xb) {
    if ((*(byte *)(param_2 + 8) & 1) == 0) {
      iVar1 = FUN_80077360(&DAT_800d4a2c,5);
      if (iVar1 != 0) {
        uVar2 = 0x58;
      }
      iVar1 = FUN_80077360(&DAT_800d4a2c,0xf);
      iVar3 = uVar2 - 0x10;
      if (iVar1 == 0) goto LAB_8001ab48;
      uVar2 = 0x5a;
    }
    else {
      iVar1 = FUN_80077360(&DAT_800d4a2c,6);
      iVar3 = uVar2 - 0x10;
      if (iVar1 == 0) goto LAB_8001ab48;
      uVar2 = 0x59;
    }
  }
  iVar3 = uVar2 - 0x10;
LAB_8001ab48:
  FUN_8001b934(iVar3 << 2 | 2,param_3);
  *(undefined1 *)(param_2 + 0x105) = DAT_800cfbf0._1_1_;
  *(undefined1 *)(param_2 + 0x104) = (undefined1)DAT_800cfbf0;
  FUN_80076a40(param_3);
  FUN_80031f94(1);
  iVar1 = FUN_8001b934(iVar3 << 2 | 3,param_3);
  piVar4 = (int *)((int)param_3 + *param_3);
  *(int *)(param_2 + 0x1bc) = (int)param_3 + *piVar4;
  *(int *)(param_2 + 0x17c) = (int)param_3 + piVar4[1];
  *(int *)(param_2 + 0x108) = (int)param_3 + piVar4[2];
  *(int *)(param_2 + 0x184) = (int)param_3 + piVar4[3];
  *(int *)(param_2 + 0x180) = (int)param_3 + piVar4[4];
  *(int *)(param_2 + 0x18c) = (int)param_3 + piVar4[5];
  *(int *)(param_2 + 0x188) = (int)param_3 + piVar4[6];
  iVar3 = piVar4[7];
  *(undefined4 *)(param_2 + 0x7c) = 0x808080;
  *(int *)(param_2 + 0x14) = (int)param_3 + iVar3;
  if ((*(byte *)(param_2 + 0x1cf) & 0x80) != 0) {
    FUN_80010778((int)param_3 + piVar4[3],(int)param_3 + piVar4[7],iVar1 - piVar4[7]);
    *(int *)(param_2 + 0x14) = (int)param_3 + piVar4[3];
  }
  if ((*(byte *)(param_2 + 0x1cf) & 0x40) != 0) {
    FUN_80010778((int)param_3 + piVar4[1],(int)param_3 + piVar4[3],iVar1 - piVar4[3]);
    iVar1 = piVar4[3] - piVar4[1] >> 2;
    *(int *)(param_2 + 0x180) = *(int *)(param_2 + 0x180) + iVar1 * -4;
    *(int *)(param_2 + 0x14) = *(int *)(param_2 + 0x14) + iVar1 * -4;
    *(int *)(param_2 + 0x184) = *(int *)(param_2 + 0x184) + iVar1 * -4;
    *(int *)(param_2 + 0x188) = *(int *)(param_2 + 0x188) + iVar1 * -4;
    *(int *)(param_2 + 0x18c) = *(int *)(param_2 + 0x18c) + iVar1 * -4;
    *(undefined4 *)(param_2 + 0x17c) = DAT_800cfd74;
    *(undefined4 *)(param_2 + 0x108) = DAT_800cfd00;
  }
  uVar2 = FUN_80076b60(0,*(undefined4 *)(param_2 + 0x14),*(undefined1 *)(param_2 + 0x104),
                       *(undefined1 *)(param_2 + 0x105));
  iVar1 = *(int *)(param_2 + 0x14);
  *(int *)(param_2 + 0x14) = iVar1 + 0xc;
  *(int *)(param_2 + 0x1c) = iVar1 + 0x28;
  return *(int *)(param_2 + 0x14) + ((uVar2 & 0xfffffffc) - 0xc);
}


