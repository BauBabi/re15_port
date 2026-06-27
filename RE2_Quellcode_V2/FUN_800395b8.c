/* FUN_800395b8 @ 0x800395b8  (Ghidra headless, raw MIPS overlay) */

void FUN_800395b8(int param_1,int param_2,int param_3,undefined2 param_4)

{
  undefined *puVar1;
  uint uVar2;
  int iVar3;
  
  uVar2 = FUN_800397e4(&DAT_800eac28);
  if ((uVar2 < 0x20) && (param_1 != 0)) {
    PTR_DAT_800a2690 = (undefined *)FUN_800397b8(&DAT_800eac28);
    iVar3 = (param_3 - param_2) * 0x80;
    if (param_1 == 0) {
      trap(0x1c00);
    }
    if ((param_1 == -1) && (iVar3 == -0x80000000)) {
      trap(0x1800);
    }
    *PTR_DAT_800a2690 = 1;
    puVar1 = PTR_DAT_800a2690;
    *(short *)(PTR_DAT_800a2690 + 4) = (short)param_1;
    *(undefined2 *)(puVar1 + 2) = param_4;
    *(short *)(puVar1 + 8) = (short)(param_2 << 7);
    *(short *)(puVar1 + 6) = (short)(iVar3 / param_1);
  }
  return;
}


