/* _SsSndTempo @ 0x8007f138  (Ghidra headless, raw MIPS overlay) */

void _SsSndTempo(int param_1,short param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  
  piVar5 = (int *)((int)&DAT_800ea250 + ((param_1 << 0x10) >> 0xe));
  iVar4 = *piVar5 + param_2 * 0xb0;
  iVar1 = *(int *)(iVar4 + 0xa8) + -1;
  *(int *)(iVar4 + 0xa8) = iVar1;
  if (iVar1 < 0) {
    iVar1 = param_2 * 0xb0 + *piVar5;
    *(uint *)(iVar1 + 0x98) = *(uint *)(iVar1 + 0x98) & 0xffffffbf;
    TEMPO_OBJ_208();
    return;
  }
  iVar3 = (int)*(short *)(iVar4 + 0x4e);
  if (iVar3 < 1) {
    uVar2 = *(uint *)(iVar4 + 0x94);
    if (*(uint *)(iVar4 + 0xac) < uVar2) {
      *(uint *)(iVar4 + 0x94) = uVar2 + iVar3;
      TEMPO_OBJ_128();
      return;
    }
    if (uVar2 < *(uint *)(iVar4 + 0xac)) {
      *(uint *)(iVar4 + 0x94) = uVar2 - iVar3;
      if (*(uint *)(iVar4 + 0xac) < uVar2 - iVar3) {
        *(uint *)(iVar4 + 0x94) = *(uint *)(iVar4 + 0xac);
      }
    }
  }
  else {
    if (iVar3 == 0) {
      trap(0x1c00);
    }
    if ((iVar3 == -1) && (iVar1 == -0x80000000)) {
      trap(0x1800);
    }
    if (iVar1 % iVar3 != 0) {
      return;
    }
    uVar2 = *(uint *)(iVar4 + 0x94);
    iVar1 = uVar2 - 1;
    if ((*(uint *)(iVar4 + 0xac) < uVar2) || (iVar1 = uVar2 + 1, uVar2 < *(uint *)(iVar4 + 0xac))) {
      *(int *)(iVar4 + 0x94) = iVar1;
      TEMPO_OBJ_134();
      return;
    }
  }
  uVar2 = (uint)((int)*(short *)(iVar4 + 0x50) * *(int *)(iVar4 + 0x94) * 10) /
          (uint)(DAT_800ea228 * 0x3c);
  if (DAT_800ea228 * 0x3c == 0) {
    trap(0x1c00);
  }
  *(short *)(iVar4 + 0x54) = (short)uVar2;
  if ((int)(uVar2 << 0x10) < 1) {
    *(undefined2 *)(iVar4 + 0x54) = 1;
  }
  if ((*(int *)(iVar4 + 0xa8) == 0) || (*(int *)(iVar4 + 0x94) == *(int *)(iVar4 + 0xac))) {
    piVar5 = (int *)((int)&DAT_800ea250 + ((param_1 << 0x10) >> 0xe));
    iVar1 = param_2 * 0xb0 + *piVar5;
    *(uint *)(iVar1 + 0x98) = *(uint *)(iVar1 + 0x98) & 0xffffffbf;
    iVar1 = param_2 * 0xb0 + *piVar5;
    *(uint *)(iVar1 + 0x98) = *(uint *)(iVar1 + 0x98) & 0xffffff7f;
  }
  return;
}


