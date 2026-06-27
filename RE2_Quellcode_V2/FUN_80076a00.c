/* FUN_80076a00 @ 0x80076a00  (Ghidra headless, raw MIPS overlay) */

void FUN_80076a00(undefined1 *param_1,undefined1 *param_2,int param_3)

{
  undefined1 *puVar1;
  
  puVar1 = param_1 + param_3;
  do {
    *param_1 = *param_2;
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
  } while (param_1 < puVar1);
  return;
}


