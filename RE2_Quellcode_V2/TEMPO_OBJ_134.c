/* TEMPO_OBJ_134 @ 0x8007f26c  (Ghidra headless, raw MIPS overlay) */

void TEMPO_OBJ_134(undefined4 param_1,short param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  int in_t1;
  uint uVar3;
  
  uVar3 = (uint)((int)*(short *)(param_4 + 0x50) * *(int *)(param_4 + 0x94) * 10) /
          (uint)(DAT_800ea228 * 0x3c);
  if (DAT_800ea228 * 0x3c == 0) {
    trap(0x1c00);
  }
  *(short *)(param_4 + 0x54) = (short)uVar3;
  if ((int)(uVar3 << 0x10) < 1) {
    *(undefined2 *)(param_4 + 0x54) = 1;
  }
  if ((*(int *)(param_4 + 0xa8) == 0) || (*(int *)(param_4 + 0x94) == *(int *)(param_4 + 0xac))) {
    piVar2 = (int *)((int)&DAT_800ea250 + ((in_t1 << 0x10) >> 0xe));
    iVar1 = param_2 * 0xb0 + *piVar2;
    *(uint *)(iVar1 + 0x98) = *(uint *)(iVar1 + 0x98) & 0xffffffbf;
    iVar1 = param_2 * 0xb0 + *piVar2;
    *(uint *)(iVar1 + 0x98) = *(uint *)(iVar1 + 0x98) & 0xffffff7f;
  }
  return;
}


