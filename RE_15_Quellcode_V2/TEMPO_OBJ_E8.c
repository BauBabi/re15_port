void TEMPO_OBJ_E8(undefined4 param_1,short param_2,int param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  
  uVar3 = (uint)((int)*(short *)(param_4 + 0x4a) * *(int *)(param_4 + 0x8c) * 10) /
          (uint)(DAT_800bb4bc * 0x3c);
  if (DAT_800bb4bc * 0x3c == 0) {
    trap(0x1c00);
  }
  *(short *)(param_4 + 0x70) = (short)uVar3;
  if ((int)(uVar3 << 0x10) < 1) {
    *(undefined2 *)(param_4 + 0x70) = 1;
  }
  if ((*(int *)(param_4 + 0xa0) == 0) || (*(int *)(param_4 + 0x8c) == *(int *)(param_4 + 0xa4))) {
    piVar2 = (int *)((int)&DAT_800bb500 + ((param_3 << 0x10) >> 0xe));
    iVar1 = param_2 * 0xac + *piVar2;
    *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) & 0xffffffbf;
    iVar1 = param_2 * 0xac + *piVar2;
    *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) & 0xffffff7f;
  }
  return;
}
