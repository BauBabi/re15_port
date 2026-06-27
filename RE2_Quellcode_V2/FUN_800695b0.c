/* FUN_800695b0 @ 0x800695b0  (Ghidra headless, raw MIPS overlay) */

char FUN_800695b0(void)

{
  char *pcVar1;
  uint uVar2;
  
  uVar2 = 0;
  if ((((uint)DAT_800d5bfd != (uint)DAT_800d5bfc) &&
      ((&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] != '\0')) &&
     ((byte)(&DAT_800a9e1f)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] * 8] != 0)) {
    pcVar1 = (&PTR_DAT_800a9e20)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] * 2];
    do {
      uVar2 = uVar2 + 1;
      if (*pcVar1 == (&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4]) {
        DAT_800d5c16 = pcVar1[2];
        DAT_800d5c17 = pcVar1[3];
        return pcVar1[1];
      }
      pcVar1 = pcVar1 + 4;
    } while (uVar2 < (byte)(&DAT_800a9e1f)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] * 8])
    ;
  }
  return '\0';
}


