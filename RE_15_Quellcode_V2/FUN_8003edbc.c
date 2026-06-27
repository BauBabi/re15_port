void FUN_8003edbc(void)

{
  undefined4 *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  puVar1 = &DAT_800ac9b0;
  do {
    *puVar1 = 0;
    uVar2 = uVar2 + 1;
    puVar1 = puVar1 + 1;
  } while (uVar2 < 0x20);
  DAT_800afbb4 = 0;
  return;
}
