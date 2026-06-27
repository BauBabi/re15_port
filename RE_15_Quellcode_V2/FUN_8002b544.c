byte FUN_8002b544(void)

{
  byte bVar1;
  uint *puVar2;
  char cVar3;
  byte bVar4;
  
  bVar4 = 0;
  puVar2 = &DAT_800acc2c;
  cVar3 = DAT_800aca4e;
  if (DAT_800aca4e != '\0') {
    do {
      if ((*puVar2 & 1) != 0) {
        cVar3 = cVar3 + -1;
        if (DAT_800ac784 != puVar2) {
          bVar1 = FUN_8002aec4(puVar2);
          bVar4 = bVar4 | bVar1;
        }
      }
      puVar2 = puVar2 + 0x7d;
    } while (cVar3 != '\0');
  }
  return bVar4;
}
