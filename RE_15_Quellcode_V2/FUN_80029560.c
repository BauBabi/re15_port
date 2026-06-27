void FUN_80029560(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)&DAT_800b841c;
  iVar1 = 0x40;
  do {
    *puVar2 = 0;
    iVar1 = iVar1 + -1;
    puVar2 = puVar2 + 1;
  } while (iVar1 != 0);
  DAT_800b851c = &DAT_800b841c;
  return;
}
