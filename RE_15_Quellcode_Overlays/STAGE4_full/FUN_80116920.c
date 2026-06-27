/* FUN_80116920 @ 0x80116920  (Ghidra headless, raw MIPS overlay) */

void FUN_80116920(uint *param_1)

{
  uint *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  DAT_800acc0c = 1;
  *(undefined1 *)((int)param_1 + 0x93) = 3;
  *(undefined1 *)(param_1 + 0x6e) = 0;
  *(undefined1 *)((int)param_1 + 0x1b9) = 0;
  *(undefined2 *)((int)param_1 + 0x9a) = 100;
  puVar1 = param_1;
  do {
    puVar1[0x74] = 0;
    uVar2 = uVar2 + 1;
    puVar1 = puVar1 + 1;
  } while (uVar2 < 8);
  param_1[1] = 1;
  *(undefined1 *)(param_1 + 0x25) = 0;
  *(undefined1 *)((int)param_1 + 0x95) = 0;
  *(undefined1 *)((int)param_1 + 0x8f) = 0;
  *param_1 = *param_1 & 0x1fffffff;
  func_0x8001f314(param_1[0x21],param_1[0x5b],0,0x200);
  return;
}


