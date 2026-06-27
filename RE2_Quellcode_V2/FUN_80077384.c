/* FUN_80077384 @ 0x80077384  (Ghidra headless, raw MIPS overlay) */

void FUN_80077384(undefined2 param_1,SVECTOR *param_2,SVECTOR *param_3)

{
  DAT_800ab20a = param_1;
  RotMatrix((SVECTOR *)&DAT_800ab208,(MATRIX *)&DAT_800c3be0);
  ApplyMatrixSV((MATRIX *)&DAT_800c3be0,param_2,param_3);
  return;
}


