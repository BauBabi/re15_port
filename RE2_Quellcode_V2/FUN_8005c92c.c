/* FUN_8005c92c @ 0x8005c92c  (Ghidra headless, raw MIPS overlay) */

bool FUN_8005c92c(uint param_1,byte param_2)

{
  bool bVar1;
  
  if ((param_2 & 7) < (byte)(&DAT_800d4ca0)[(param_1 & 0xff) * 2]) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
    if ((&DAT_800d4ca0)[(param_1 & 0xff) * 2] == (param_2 & 7)) {
      bVar1 = (param_2 & 8) != 0;
    }
  }
  return bVar1;
}


