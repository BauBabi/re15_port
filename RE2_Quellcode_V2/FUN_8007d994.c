/* FUN_8007d994 @ 0x8007d994  (Ghidra headless, raw MIPS overlay) */

void FUN_8007d994(ushort param_1,int param_2)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  ushort local_38;
  ushort local_36 [11];
  
  piVar8 = (int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar7 = (short)param_2 * 0xb0;
  iVar6 = *piVar8 + iVar7;
  iVar2 = *(int *)(iVar6 + 0xa0) + -1;
  *(int *)(iVar6 + 0xa0) = iVar2;
  if (iVar2 < 0) {
    iVar7 = iVar7 + *piVar8;
  }
  else {
    iVar4 = (int)*(short *)(iVar6 + 0x4c);
    if (iVar4 < 1) {
      if (-1 < iVar4) goto LAB_8007dc74;
      iVar4 = (uint)*(ushort *)(iVar6 + 0x4a) + iVar4;
      *(short *)(iVar6 + 0x4a) = (short)iVar4;
      if (iVar4 * 0x10000 < 0) goto LAB_8007dbd0;
      iVar2 = (int)(short)(param_1 | (ushort)(param_2 << 8));
      FUN_800845e8(iVar2,&local_38,local_36);
      if ((0x7e < (int)((uint)local_38 - (int)*(short *)(iVar6 + 0x4c))) &&
         (0x7e < (int)((uint)local_36[0] - (int)*(short *)(iVar6 + 0x4c)))) {
        FUN_800840ac(iVar2,0x7f,0x7f,1);
        *(uint *)(iVar7 + *piVar8 + 0x98) = *(uint *)(iVar7 + *piVar8 + 0x98) & 0xffffffef;
      }
      iVar2 = (int)*(short *)(iVar6 + 0x4c);
      if ((*(int *)(iVar6 + 0x9c) - *(int *)(iVar6 + 0xa0)) * -iVar2 < (int)*(short *)(iVar6 + 0x48)
         ) {
        iVar7 = (int)(short)(param_1 | (ushort)(param_2 << 8));
        uVar3 = (uint)local_38 - iVar2;
        uVar5 = (uint)local_36[0] - iVar2;
        goto LAB_8007dbc0;
      }
    }
    else {
      if (iVar4 == 0) {
        trap(0x1c00);
      }
      if ((iVar4 == -1) && (iVar2 == -0x80000000)) {
        trap(0x1800);
      }
      if (iVar2 % iVar4 != 0) goto LAB_8007dc74;
      uVar1 = *(short *)(iVar6 + 0x4a) - 1;
      *(ushort *)(iVar6 + 0x4a) = uVar1;
      if ((int)((uint)uVar1 << 0x10) < 0) {
LAB_8007dbd0:
        FUN_800840ac((int)(short)(param_1 | (ushort)(param_2 << 8)),0x7f,0x7f,1);
        *(uint *)(iVar7 + *piVar8 + 0x98) = *(uint *)(iVar7 + *piVar8 + 0x98) & 0xffffffef;
      }
      else {
        iVar7 = (int)(short)(param_1 | (ushort)(param_2 << 8));
        FUN_800845e8(iVar7,&local_38,local_36);
        uVar3 = (uint)local_38;
        if ((int)(uVar3 + 1) <= (int)(uVar3 + (int)*(short *)(iVar6 + 0x4a))) {
          uVar3 = uVar3 + 1;
          uVar5 = local_36[0] + 1;
LAB_8007dbc0:
          FUN_800840ac(iVar7,uVar3 & 0xffff,uVar5 & 0xffff,1);
        }
      }
    }
    if ((*(int *)(iVar6 + 0xa0) != 0) && (0 < *(short *)(iVar6 + 0x4a))) goto LAB_8007dc74;
    iVar7 = (short)param_2 * 0xb0 +
            *(int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  }
  *(uint *)(iVar7 + 0x98) = *(uint *)(iVar7 + 0x98) & 0xffffffef;
LAB_8007dc74:
  FUN_800845e8((int)(short)(param_1 | (ushort)(param_2 << 8)),iVar6 + 0x5c,iVar6 + 0x5e);
  return;
}


