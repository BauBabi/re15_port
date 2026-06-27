undefined4 FUN_8003bc2c(uint param_1)

{
  bool bVar1;
  uint *puVar2;
  uint uVar3;
  byte bVar4;
  
  bVar4 = 3;
  do {
    puVar2 = (uint *)((uint)bVar4 * 4 + 4 + *(int *)(DAT_800ac778 + 0x20));
    for (uVar3 = *puVar2; uVar3 < puVar2[1]; uVar3 = uVar3 + 0xc) {
      if ((param_1 & 0xff) == (int)((uint)*(ushort *)(uVar3 + 10) << 0x10) >> 0x1c) {
        return 1;
      }
    }
    bVar1 = bVar4 != 0;
    bVar4 = bVar4 - 1;
  } while (bVar1);
  return 0;
}
