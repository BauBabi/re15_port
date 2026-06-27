void FUN_8001a4c0(void)

{
  bool bVar1;
  byte bVar2;
  
  bVar2 = 0x13;
  do {
    (&DAT_800acc2c)[(uint)bVar2 * 0x7d] = 0;
    bVar1 = bVar2 != 0;
    bVar2 = bVar2 - 1;
  } while (bVar1);
  return;
}
