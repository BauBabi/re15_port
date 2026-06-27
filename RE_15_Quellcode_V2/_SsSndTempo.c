void _SsSndTempo(int param_1,short param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  int iVar5;
  
  iVar5 = param_2 * 0xac + *(int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
  uVar3 = (uint)*(short *)(iVar5 + 0x44);
  uVar1 = *(int *)(iVar5 + 0xa0) - 1;
  *(uint *)(iVar5 + 0xa0) = uVar1;
  if ((int)uVar3 < 1) {
    uVar1 = *(uint *)(iVar5 + 0x8c);
    if (*(uint *)(iVar5 + 0xa4) < uVar1) {
      *(uint *)(iVar5 + 0x8c) = uVar3 + uVar1;
      TEMPO_OBJ_DC();
      return;
    }
    if (uVar1 < *(uint *)(iVar5 + 0xa4)) {
      *(uint *)(iVar5 + 0x8c) = uVar1 - uVar3;
      if (*(uint *)(iVar5 + 0xa4) < uVar1 - uVar3) {
        *(uint *)(iVar5 + 0x8c) = *(uint *)(iVar5 + 0xa4);
      }
    }
  }
  else {
    if (uVar3 == 0) {
      trap(0x1c00);
    }
    if (uVar1 % uVar3 != 0) {
      return;
    }
    uVar1 = *(uint *)(iVar5 + 0x8c);
    iVar2 = uVar1 - 1;
    if ((*(uint *)(iVar5 + 0xa4) < uVar1) || (iVar2 = uVar1 + 1, uVar1 < *(uint *)(iVar5 + 0xa4))) {
      *(int *)(iVar5 + 0x8c) = iVar2;
      TEMPO_OBJ_E8();
      return;
    }
  }
  uVar1 = (uint)((int)*(short *)(iVar5 + 0x4a) * *(int *)(iVar5 + 0x8c) * 10) /
          (uint)(DAT_800bb4bc * 0x3c);
  if (DAT_800bb4bc * 0x3c == 0) {
    trap(0x1c00);
  }
  *(short *)(iVar5 + 0x70) = (short)uVar1;
  if ((int)(uVar1 << 0x10) < 1) {
    *(undefined2 *)(iVar5 + 0x70) = 1;
  }
  if ((*(int *)(iVar5 + 0xa0) == 0) || (*(int *)(iVar5 + 0x8c) == *(int *)(iVar5 + 0xa4))) {
    piVar4 = (int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
    iVar5 = param_2 * 0xac + *piVar4;
    *(uint *)(iVar5 + 0x90) = *(uint *)(iVar5 + 0x90) & 0xffffffbf;
    iVar5 = param_2 * 0xac + *piVar4;
    *(uint *)(iVar5 + 0x90) = *(uint *)(iVar5 + 0x90) & 0xffffff7f;
  }
  return;
}
