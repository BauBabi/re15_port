/* FUN_80097494 @ 0x80097494  (Ghidra headless, raw MIPS overlay) */

bool FUN_80097494(int param_1,char param_2,undefined1 param_3)

{
  int iVar1;
  
  iVar1 = (*DAT_800c39b8)();
  if (iVar1 == 0) {
    *(undefined1 *)(param_1 + 0x46) = 1;
    *(undefined1 **)(param_1 + 0x14) = &LAB_8009752c;
    *(undefined1 **)(param_1 + 0x18) = &LAB_80097580;
    *(char *)(param_1 + 0x51) = param_2;
    *(undefined1 *)(param_1 + 0x52) = param_3;
    *(bool *)(param_1 + 0x53) = param_2 == *(char *)(param_1 + 0xe4);
  }
  return iVar1 == 0;
}


