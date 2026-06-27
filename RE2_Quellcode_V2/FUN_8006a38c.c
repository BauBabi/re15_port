/* FUN_8006a38c @ 0x8006a38c  (Ghidra headless, raw MIPS overlay) */

void FUN_8006a38c(int param_1)

{
  if (param_1 != 0) {
    if ((DAT_800d5bfc & 1) == 0) {
      if ((&DAT_800d4a3e)[(uint)DAT_800d5bfd * 4] != '\0') {
        DAT_800d5c0c = 2;
        goto LAB_8006a438;
      }
      if ((DAT_800d5bfd & 1) != 0) {
        DAT_800d5c0c = 4;
        goto LAB_8006a438;
      }
    }
    else {
      if ((&DAT_800d4a3e)[(uint)DAT_800d5bfd * 4] != '\0') {
        DAT_800d5c0c = 0xfe;
        goto LAB_8006a438;
      }
      if ((DAT_800d5bfd & 1) == 0) {
        DAT_800d5c0c = 0xfc;
        goto LAB_8006a438;
      }
    }
    DAT_800d5c0c = 0;
LAB_8006a438:
    DAT_800d5c0d = ((DAT_800d5bfd >> 1) - (DAT_800d5bfc >> 1)) * '\x03';
    return;
  }
  if ((DAT_800d5bfc & 1) == 0) {
    if ((&DAT_800d4a3e)[(uint)DAT_800d5bfc * 4] == '\0') {
      if ((DAT_800d5bfd & 1) != 0) {
        DAT_800d5c10 = 0xfc;
        goto LAB_8006a54c;
      }
      goto LAB_8006a544;
    }
  }
  else if ((&DAT_800d4a3e)[(uint)DAT_800d5bfc * 4] == '\0') {
    if ((DAT_800d5bfd & 1) == 0) {
      DAT_800d5c10 = 4;
      goto LAB_8006a54c;
    }
LAB_8006a544:
    DAT_800d5c10 = 0;
    goto LAB_8006a54c;
  }
  if ((DAT_800d5bfd & 1) == 0) {
    DAT_800d5c10 = 2;
  }
  else {
    DAT_800d5c10 = 0xfe;
  }
LAB_8006a54c:
  DAT_800d5c11 = ((DAT_800d5bfc >> 1) - (DAT_800d5bfd >> 1)) * '\x03';
  return;
}


