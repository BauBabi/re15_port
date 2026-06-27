uint FUN_80037250(void)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = 0;
  uVar3 = (int)(DAT_800acbfc + 0x1ffd4cf5) / 500;
  puVar2 = &DAT_800acc2c + (uVar3 & 0xff) * 0x7d;
  cVar1 = DAT_800aca4e;
  if (DAT_800aca4e != '\0') {
    do {
      uVar3 = uVar3 + 1;
      puVar2 = puVar2 + 0x7d;
      if (0x13 < (uVar3 & 0xff)) {
        uVar3 = 0;
        puVar2 = &DAT_800acc2c;
      }
      if ((*puVar2 & 1) != 0) {
        if ((-1 < *(short *)((int)puVar2 + 0x9a)) && ((*(byte *)((int)puVar2 + 9) & 0x60) == 0)) {
          uVar4 = uVar3 + 1;
          break;
        }
        cVar1 = cVar1 + -1;
      }
    } while (cVar1 != '\0');
  }
  uVar4 = uVar4 & 0xff;
  if (uVar4 != 0) {
    DAT_800acbfc = &DAT_800aca38 + uVar4 * 0x7d;
  }
  return uVar4;
}
