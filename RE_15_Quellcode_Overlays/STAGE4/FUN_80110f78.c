/* FUN_80110f78 @ 0x80110f78  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f78(void)

{
  int in_v1;
  
  if (in_v1 == 2) {
    func_0x80037edc(0,10);
    func_0x80037edc(5,0x32);
    func_0x80037edc(7,0x32);
    _DAT_800aca58 = 7;
    return;
  }
  if ((((bRam00000090 & 0xf0) * 0x10 - (int)sRam0000006a) + 0x200 & 0xfff) < 0x400) {
    bRam0000009f = bRam00000090;
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


