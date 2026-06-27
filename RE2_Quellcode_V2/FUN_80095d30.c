/* FUN_80095d30 @ 0x80095d30  (Ghidra headless, raw MIPS overlay) */

undefined1 FUN_80095d30(void)

{
  byte bVar1;
  int iVar2;
  
  iVar2 = (*DAT_800c39b0)();
  if (((*(uint *)(iVar2 + 0x34) & 0xffff0000) != 0) ||
     (((iVar2 != *(int *)(iVar2 + 0x10) && (*(char *)(iVar2 + 0x38) != '\0')) ||
      (**(char **)(iVar2 + 0x30) != '\0')))) {
    bVar1 = *(byte *)(iVar2 + 0x49);
    if (bVar1 == 3) {
      return 1;
    }
    if (bVar1 < 4) {
      if (bVar1 == 2) {
        return 1;
      }
    }
    else if (bVar1 == 6) {
      return 4;
    }
  }
  return *(undefined1 *)(iVar2 + 0x49);
}


