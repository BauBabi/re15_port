/* FUN_80039514 @ 0x80039514  (Ghidra headless, raw MIPS overlay) */

void FUN_80039514(int param_1,undefined1 param_2,undefined2 param_3)

{
  undefined *puVar1;
  undefined *puVar2;
  uint uVar3;
  
  uVar3 = FUN_800397e4(&DAT_800eac28);
  if ((uVar3 < 0x20) && (param_1 != 0)) {
    PTR_DAT_800a2690 = (undefined *)FUN_800397b8(&DAT_800eac28);
    *PTR_DAT_800a2690 = 1;
    puVar1 = PTR_DAT_800a2690;
    PTR_DAT_800a2690[1] = param_2;
    puVar2 = PTR_DAT_800a2690;
    *(short *)(puVar1 + 4) = (short)param_1;
    *(undefined2 *)(puVar2 + 2) = param_3;
    *(undefined2 *)(puVar2 + 6) = 0;
  }
  return;
}


