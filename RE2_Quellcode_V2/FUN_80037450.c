/* FUN_80037450 @ 0x80037450  (Ghidra headless, raw MIPS overlay) */

bool FUN_80037450(int param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  short local_18 [2];
  short local_14;
  
  local_14 = 0;
  local_18[0] = (short)(((uint)*(ushort *)(param_1 + 0x9a) * 0x78) / 100) + 0x80;
  FUN_80077384((int)*(short *)(param_1 + 0x76),local_18,local_18);
  iVar4 = *(ushort *)(param_2 + 0x90) - 0x80;
  iVar5 = *(ushort *)(param_2 + 0x92) - 0x80;
  iVar3 = *(int *)(param_2 + 0x38) - *(int *)(param_1 + 0x38);
  iVar2 = *(int *)(param_2 + 0x40) - *(int *)(param_1 + 0x40);
  if (((uint)(iVar3 + iVar4) <= (uint)(iVar4 * 2)) ||
     (bVar1 = false, (uint)(iVar2 + iVar5) <= (uint)(iVar5 * 2))) {
    if (iVar3 < 0) {
      iVar3 = -iVar3;
    }
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    if (iVar2 < iVar3) {
      iVar2 = (int)(short)(local_18[0] + (short)*(undefined4 *)(param_1 + 0x38)) -
              *(int *)(param_2 + 0x84);
      iVar3 = *(int *)(param_1 + 0x40) - *(int *)(param_2 + 0x8c);
      iVar5 = iVar5 + (uint)*(ushort *)(param_1 + 0x90);
    }
    else {
      iVar2 = *(int *)(param_1 + 0x38) - *(int *)(param_2 + 0x84);
      iVar3 = (int)(short)(local_14 + (short)*(undefined4 *)(param_1 + 0x40)) -
              *(int *)(param_2 + 0x8c);
      iVar4 = iVar4 + (uint)*(ushort *)(param_1 + 0x90);
    }
    bVar1 = false;
    if ((uint)(iVar2 + iVar4) <= (uint)(iVar4 << 1)) {
      bVar1 = (uint)(iVar3 + iVar5) <= (uint)(iVar5 << 1);
    }
  }
  return bVar1;
}


