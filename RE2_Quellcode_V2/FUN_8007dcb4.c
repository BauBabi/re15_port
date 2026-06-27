/* FUN_8007dcb4 @ 0x8007dcb4  (Ghidra headless, raw MIPS overlay) */

void FUN_8007dcb4(ushort param_1,int param_2)

{
  ushort uVar1;
  int iVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  ushort local_38;
  ushort local_36 [11];
  
  piVar9 = (int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  sVar3 = (short)param_2;
  iVar8 = sVar3 * 0xb0;
  iVar7 = *piVar9 + iVar8;
  iVar2 = *(int *)(iVar7 + 0xa0) + -1;
  *(int *)(iVar7 + 0xa0) = iVar2;
  if (iVar2 < 0) {
    iVar8 = iVar8 + *piVar9;
  }
  else {
    iVar5 = (int)*(short *)(iVar7 + 0x4c);
    if (iVar5 < 1) {
      if (-1 < iVar5) goto LAB_8007e020;
      iVar5 = (uint)*(ushort *)(iVar7 + 0x4a) + iVar5;
      *(short *)(iVar7 + 0x4a) = (short)iVar5;
      if (iVar5 * 0x10000 < 0) goto LAB_8007df7c;
      iVar2 = (int)(short)(param_1 | (ushort)(param_2 << 8));
      FUN_800845e8(iVar2,&local_38,local_36);
      if (((int)((uint)local_38 + (int)*(short *)(iVar7 + 0x4c)) < 1) &&
         ((int)((uint)local_36[0] + (int)*(short *)(iVar7 + 0x4c)) < 1)) {
        FUN_800840ac(iVar2,0,0,1);
        *(uint *)(iVar8 + *piVar9 + 0x98) = *(uint *)(iVar8 + *piVar9 + 0x98) & 0xffffffdf;
      }
      iVar2 = (int)*(short *)(iVar7 + 0x4c);
      if ((*(int *)(iVar7 + 0x9c) - *(int *)(iVar7 + 0xa0)) * -iVar2 < (int)*(short *)(iVar7 + 0x48)
         ) {
        if (local_38 != 0) {
          if (local_36[0] != 0) {
            iVar5 = (int)(short)(param_1 | (ushort)(param_2 << 8));
            uVar4 = (uint)local_38 + iVar2;
            uVar6 = (uint)local_36[0] + iVar2;
            goto LAB_8007df68;
          }
        }
        iVar8 = sVar3 * 0xb0 + *(int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
LAB_8007df3c:
        *(uint *)(iVar8 + 0x98) = *(uint *)(iVar8 + 0x98) & 0xffffffdf;
      }
    }
    else {
      if (iVar5 == 0) {
        trap(0x1c00);
      }
      if ((iVar5 == -1) && (iVar2 == -0x80000000)) {
        trap(0x1800);
      }
      if (iVar2 % iVar5 != 0) goto LAB_8007e020;
      uVar1 = *(short *)(iVar7 + 0x4a) - 1;
      *(ushort *)(iVar7 + 0x4a) = uVar1;
      if ((int)((uint)uVar1 << 0x10) < 0) {
LAB_8007df7c:
        FUN_800840ac((int)(short)(param_1 | (ushort)(param_2 << 8)),0,0,1);
        *(uint *)(iVar8 + *piVar9 + 0x98) = *(uint *)(iVar8 + *piVar9 + 0x98) & 0xffffffdf;
      }
      else {
        iVar5 = (int)(short)(param_1 | (ushort)(param_2 << 8));
        FUN_800845e8(iVar5,&local_38,local_36);
        uVar4 = (uint)local_38;
        if (((int)(uVar4 - (int)*(short *)(iVar7 + 0x4a)) <= (int)(uVar4 - 1)) ||
           ((int)((uint)local_36[0] - (int)*(short *)(iVar7 + 0x4a)) <= (int)(local_36[0] - 1))) {
          if ((uVar4 == 0) || (local_36[0] == 0)) {
            iVar8 = iVar8 + *piVar9;
            goto LAB_8007df3c;
          }
          uVar4 = uVar4 - 1;
          uVar6 = local_36[0] - 1;
LAB_8007df68:
          FUN_800840ac(iVar5,uVar4 & 0xffff,uVar6 & 0xffff,1);
        }
      }
    }
    if ((*(int *)(iVar7 + 0xa0) != 0) && (0 < *(short *)(iVar7 + 0x4a))) goto LAB_8007e020;
    iVar8 = sVar3 * 0xb0 + *(int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  }
  *(uint *)(iVar8 + 0x98) = *(uint *)(iVar8 + 0x98) & 0xffffffdf;
LAB_8007e020:
  FUN_800845e8((int)(short)(param_1 | (ushort)(param_2 << 8)),iVar7 + 0x5c,iVar7 + 0x5e);
  return;
}


