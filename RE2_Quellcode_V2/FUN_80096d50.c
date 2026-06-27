/* FUN_80096d50 @ 0x80096d50  (Ghidra headless, raw MIPS overlay) */

void FUN_80096d50(int param_1)

{
  byte bVar1;
  
  bVar1 = *(byte *)(param_1 + 0x46);
  if (bVar1 == 3) {
    FUN_80097614(param_1,*(undefined1 *)(param_1 + 0xe4));
  }
  else if (bVar1 < 4) {
    if (bVar1 == 2) {
      FUN_80097600();
    }
  }
  else if (bVar1 == 4) {
    FUN_80097654(param_1,*(undefined1 *)(param_1 + 0x47));
  }
  return;
}


