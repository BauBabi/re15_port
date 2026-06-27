/* FUN_8003947c @ 0x8003947c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003947c(int param_1,undefined2 param_2)

{
  undefined *puVar1;
  undefined *puVar2;
  uint uVar3;
  
  uVar3 = FUN_800397e4(&DAT_800eaad8);
  if ((uVar3 < 0x20) && (param_1 != 0)) {
    PTR_DAT_800a268c = (undefined *)FUN_800397b8(&DAT_800eaad8);
    *PTR_DAT_800a268c = 1;
    puVar1 = PTR_DAT_800a268c;
    PTR_DAT_800a268c[1] = param_1 != 0;
    puVar2 = PTR_DAT_800a268c;
    *(short *)(puVar1 + 4) = (short)param_1;
    *(undefined2 *)(puVar2 + 2) = param_2;
    *(undefined2 *)(puVar2 + 6) = 0;
  }
  return;
}


