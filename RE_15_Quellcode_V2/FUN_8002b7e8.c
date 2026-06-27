byte FUN_8002b7e8(undefined4 param_1,undefined2 param_2)

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
        bVar1 = FUN_8002b5d0(puVar2,param_1,param_2);
        bVar4 = bVar4 | bVar1;
      }
      puVar2 = puVar2 + 0x7d;
    } while (cVar3 != '\0');
  }
  bVar1 = FUN_8002b5d0(&DAT_800aca54,param_1,param_2);
  return bVar4 | bVar1;
}
