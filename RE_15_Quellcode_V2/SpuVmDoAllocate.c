void SpuVmDoAllocate(void)

{
  uint *puVar1;
  int iVar2;
  
  iVar2 = 0;
  puVar1 = &DAT_8008fd28;
  DAT_800b5330 = (short)((int)DAT_800b532e << 3);
  DAT_800b5332 = (ushort)DAT_800b5320 + (ushort)DAT_800b531b * 0x10;
  (&DAT_8008f84a)[DAT_800b532e * 0x1a] = 0x7fff;
  do {
    iVar2 = iVar2 + 1;
    *puVar1 = ~(1 << ((int)DAT_800b532e & 0x1fU)) & *puVar1;
    puVar1 = puVar1 + 1;
  } while (iVar2 < 0x10);
  if ((DAT_800b532c & 1) != 0) {
    VMANAGER_OBJ_86C();
    return;
  }
  (&DAT_8008f6b2)[DAT_800b5330] =
       *(undefined2 *)((((short)DAT_800b532c + -1) / 2) * 0x10 + DAT_800b2b28 + 0xe);
  (&DAT_8008f82c)[DAT_800b532e] = (&DAT_8008f82c)[DAT_800b532e] | 8;
  iVar2 = DAT_800b2b3c;
  (&DAT_8008f6b4)[DAT_800b5330] =
       *(undefined2 *)
        (((uint)DAT_800b531b * 0x10 + (uint)DAT_800b5320) * 0x20 + DAT_800b2b3c + 0x10);
  (&DAT_8008f6b6)[DAT_800b5330] =
       *(short *)(((uint)DAT_800b531b * 0x10 + (uint)DAT_800b5320) * 0x20 + iVar2 + 0x12) +
       DAT_800b2540;
  (&DAT_8008f82c)[DAT_800b532e] = (&DAT_8008f82c)[DAT_800b532e] | 0x30;
  return;
}
