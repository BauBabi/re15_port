/* FUN_80037d48 @ 0x80037d48  (Ghidra headless, raw MIPS overlay) */

bool FUN_80037d48(int param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  short local_18 [2];
  short local_14;
  
  local_14 = 0;
  local_18[0] = (short)(((uint)*(ushort *)(param_1 + 0x9a) * 0x87) / 100) + 0x80;
  FUN_80077384((int)*(short *)(param_1 + 0x76),local_18,local_18);
  iVar3 = *(int *)(param_2 + 0x38) - *(int *)(param_1 + 0x38);
  iVar2 = *(int *)(param_2 + 0x40) - *(int *)(param_1 + 0x40);
  iVar5 = *(ushort *)(param_2 + 0x90) - 0x80;
  iVar6 = *(ushort *)(param_2 + 0x92) - 0x80;
  if (((uint)(iVar3 + iVar5) <= (uint)(iVar5 * 2)) ||
     (bVar1 = false, (uint)(iVar2 + iVar6) <= (uint)(iVar6 * 2))) {
    if (iVar3 < 0) {
      iVar3 = -iVar3;
    }
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    if (iVar2 < iVar3) {
      iVar4 = (int)(short)(local_18[0] + (short)*(undefined4 *)(param_1 + 0x38));
      iVar3 = *(int *)(param_2 + 0x84);
      iVar2 = *(int *)(param_1 + 0x40);
    }
    else {
      iVar4 = *(int *)(param_1 + 0x38);
      iVar3 = *(int *)(param_2 + 0x84);
      iVar2 = (int)(short)(local_14 + (short)*(undefined4 *)(param_1 + 0x40));
    }
    bVar1 = false;
    if ((uint)((iVar4 - iVar3) + iVar5) <= (uint)(iVar5 * 2)) {
      bVar1 = (uint)((iVar2 - *(int *)(param_2 + 0x8c)) + iVar6) <= (uint)(iVar6 * 2);
    }
  }
  return bVar1;
}


