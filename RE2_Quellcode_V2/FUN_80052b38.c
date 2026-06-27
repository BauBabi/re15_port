/* FUN_80052b38 @ 0x80052b38  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80052b38(void)

{
  bool bVar1;
  char cVar2;
  uint *puVar3;
  char *pcVar4;
  char *pcVar5;
  undefined4 *puVar6;
  
  if (_DAT_800d8cd0 != 0) {
    bVar1 = false;
    pcVar5 = "\x03";
    DAT_800d424b = '\0';
    if (DAT_800a7400 != -1) {
      pcVar4 = "";
      cVar2 = DAT_800a7400;
      do {
        if ((cVar2 == DAT_800d8cd0) && ((DAT_800d8cd1 == '\0' || (DAT_800d8cd1 == *pcVar4)))) {
LAB_80052c00:
          puVar6 = &DAT_800cfe18;
          if (DAT_800ce334 == &DAT_800cfe18) {
            return;
          }
          goto LAB_80052c2c;
        }
        if ((*pcVar4 == DAT_800d8cd0) && ((DAT_800d8cd1 == '\0' || (DAT_800d8cd1 == cVar2)))) {
          bVar1 = true;
          goto LAB_80052c00;
        }
        pcVar4 = pcVar4 + 2;
        pcVar5 = pcVar5 + 2;
        cVar2 = *pcVar5;
        DAT_800d424b = DAT_800d424b + '\x01';
      } while (cVar2 != -1);
    }
  }
  DAT_800d424b = 0xff;
  return;
LAB_80052c2c:
  puVar3 = (uint *)*puVar6;
  if ((*puVar3 & 1) != 0) {
    if ((int)*(char *)((int)puVar3 + 0x1fa) == (uint)DAT_800d8cd0) {
      if (bVar1) {
LAB_80052c84:
        *puVar3 = *puVar3 | 0x2000;
      }
    }
    else if (((int)*(char *)((int)puVar3 + 0x1fa) == (uint)DAT_800d8cd1) && (!bVar1))
    goto LAB_80052c84;
  }
  puVar6 = puVar6 + 1;
  if (puVar6 == DAT_800ce334) {
    return;
  }
  goto LAB_80052c2c;
}


