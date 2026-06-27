/* FUN_80097348 @ 0x80097348  (Ghidra headless, raw MIPS overlay) */

bool FUN_80097348(int param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = (*DAT_800c39b8)();
  if (iVar1 == 0) {
    *(undefined1 *)(param_1 + 0x46) = 1;
    *(undefined1 **)(param_1 + 0x14) = &LAB_800973b0;
    *(undefined4 *)(param_1 + 0x20) = param_2;
    *(undefined1 **)(param_1 + 0x18) = &LAB_800973cc;
  }
  return iVar1 == 0;
}


