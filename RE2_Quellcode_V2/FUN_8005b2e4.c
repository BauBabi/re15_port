/* FUN_8005b2e4 @ 0x8005b2e4  (Ghidra headless, raw MIPS overlay) */

void FUN_8005b2e4(undefined1 param_1,undefined1 param_2)

{
  if ((DAT_800cfb74 & 0x2000) == 0) {
    DAT_800dbb70 = 1;
    DAT_800df340 = param_2;
    DAT_800dfadf = param_1;
    if (DAT_800d75c0 == '\x01') {
      FUN_8007a120((int)DAT_800d75c2,0x7f,param_1);
    }
    if (DAT_800d75c8 == '\x01') {
      FUN_8007a120((int)DAT_800d75ca,0x7f,DAT_800dfadf);
    }
    if (DAT_800d75d0 == '\x01') {
      FUN_8007a120((int)DAT_800d75d2,0x7f,DAT_800dfadf);
    }
  }
  return;
}


