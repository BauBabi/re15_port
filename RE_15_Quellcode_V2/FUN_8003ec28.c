void FUN_8003ec28(void)

{
  byte bVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  
  puVar2 = &DAT_800acc2c;
  uVar3 = 0;
  uVar4 = (uint)DAT_800aca4e;
  DAT_800aca5e = 0xffff;
  if (uVar4 != 0) {
    do {
      if ((*puVar2 & 1) != 0) {
        *(undefined2 *)((int)puVar2 + 10) = 0xffff;
        uVar3 = uVar3 + 1;
      }
      puVar2 = puVar2 + 0x7d;
    } while (uVar3 < uVar4);
  }
  bVar1 = *(byte *)(DAT_800ac778 + 2);
  for (puVar2 = &DAT_800b3f98; puVar2 < &DAT_800b3f98 + (uint)bVar1 * 0x25; puVar2 = puVar2 + 0x25)
  {
    if ((*puVar2 & 1) != 0) {
      *(undefined2 *)((int)puVar2 + 10) = 0xffff;
    }
  }
  return;
}
