void FUN_80039b2c(int param_1)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  param_1 = param_1 * 8;
  uVar4 = (uint)DAT_800afbb7;
  iVar5 = uVar4 * 0x6c;
  cVar1 = (&DAT_800b11d1)[param_1];
  *(int *)(&DAT_800af364 + iVar5) = (int)cVar1 << 8;
  *(short *)(&DAT_800af370 + iVar5) = (short)((int)cVar1 << 8);
  cVar1 = (&DAT_800b11d2)[param_1];
  *(int *)(&DAT_800af368 + iVar5) = (int)cVar1 << 8;
  *(short *)(&DAT_800af372 + iVar5) = (short)((int)cVar1 << 8);
  iVar3 = DAT_8008f6a4;
  cVar1 = (&DAT_800b11d3)[param_1];
  *(int *)(&DAT_800af36c + iVar5) = (int)cVar1 << 8;
  *(short *)(&DAT_800af374 + iVar5) = (short)((int)cVar1 << 8);
  bVar2 = (&DAT_800b11d0)[param_1];
  (&DAT_800af3a4)[uVar4 * 0x1b] = &DAT_80072d4c;
  (&DAT_800af340)[uVar4 * 0x1b] = iVar3 + 0xc;
  (&DAT_800af348)[uVar4 * 0x1b] = iVar3 + 0x28;
  *(undefined4 *)(&DAT_800af3a0 + iVar5) = 0x808080;
  *(ushort *)(&DAT_800af39a + iVar5) = (ushort)bVar2 << 4;
  DAT_800ac77c = FUN_80025940(&DAT_800af33c + uVar4 * 0x1b,DAT_800ac77c);
  DAT_800ac77c = FUN_80025a98(&DAT_800af33c + uVar4 * 0x1b);
  DAT_800afbb7 = DAT_800afbb7 + 1;
  return;
}
