void FUN_8001d600(void)

{
  int iVar1;
  
  if (DAT_800ac9a8 == (short *)0x0) {
    FUN_80021634(2,0);
    DAT_800b0fe6 = _DAT_800b0fe2;
    _DAT_800b0fe2 = (ushort)(byte)(&DAT_800bbe5f)[DAT_800bbe5e];
    iVar1 = (uint)(byte)(&DAT_800bbe5f)[DAT_800bbe5e] * 0x1a;
    DAT_800aca88 = (int)*(short *)(&DAT_800c263c + iVar1 + (uint)DAT_800bbe5e * 0x4fa);
    DAT_800aca94 = *(short *)(&DAT_800c263c + iVar1 + (uint)DAT_800bbe5e * 0x4fa);
    DAT_800aca90 = (int)*(short *)(&DAT_800c263e + iVar1 + (uint)DAT_800bbe5e * 0x4fa);
    DAT_800aca98 = *(short *)(&DAT_800c263e + iVar1 + (uint)DAT_800bbe5e * 0x4fa);
    DAT_800acad6 = (&DAT_800c2640)[iVar1 + (uint)DAT_800bbe5e * 0x4fa];
    DAT_800aca8c = (uint)DAT_800acad6 * -0x708;
    DAT_800aca96 = (short)DAT_800aca8c;
    DAT_800acc0e = (short)DAT_800aca8c;
    if (DAT_800b0fe0 != DAT_800bbe5e) {
      DAT_800b0fe0 = (ushort)DAT_800bbe5e;
      FUN_80039a30();
    }
    DAT_800afbb5 = 0;
    DAT_800b0fe4 = 0;
  }
  else {
    FUN_80021634(2,0);
    FUN_800171f4();
    FUN_80029a98(1,&LAB_80016188);
    do {
      FUN_80029ac8(1);
    } while ((DAT_800aca38 & 0x10000) != 0);
    DAT_800aca88 = (int)*DAT_800ac9a8;
    DAT_800aca94 = *DAT_800ac9a8;
    DAT_800aca8c = (int)DAT_800ac9a8[1];
    DAT_800aca96 = DAT_800ac9a8[1];
    DAT_800aca90 = (int)DAT_800ac9a8[2];
    DAT_800aca98 = DAT_800ac9a8[2];
    DAT_800acabe = DAT_800ac9a8[3];
    DAT_800acad6 = *(byte *)((int)DAT_800ac9a8 + 0xb);
    DAT_800acc0e = (ushort)DAT_800acad6 * -0x708;
    DAT_800afbb5 = *(byte *)(DAT_800ac9a8 + 5);
    DAT_800b0fe6 = _DAT_800b0fe2;
    DAT_800b0fe4 = (ushort)DAT_800afbb5;
    _DAT_800b0fe2 = (ushort)*(byte *)((int)DAT_800ac9a8 + 9);
    if (DAT_800b0fe0 != *(byte *)(DAT_800ac9a8 + 4)) {
      DAT_800b0fe0 = (ushort)*(byte *)(DAT_800ac9a8 + 4);
      FUN_80039a30();
    }
  }
  FUN_800396fc();
  if ((DAT_800aca38 & 0x2000000) != 0) {
    DAT_800aca38 = DAT_800aca38 | 0x20000000;
    DAT_800be571 = 2;
    DAT_800be574 = (uint)*(ushort *)((short)DAT_800b0fe4 * 2 + (short)_DAT_800b0fe2 * 0x20);
    DAT_800be572 = *(short *)((&PTR_DAT_8007438c)[(short)DAT_800b0fe0] + (short)_DAT_800b0fe2 * 2) -
                   1;
    DAT_800be57c = (uint)(byte)(&DAT_8006f442)[(uint)DAT_800be572 * 8] * 0x10000 +
                   (uint)*(ushort *)(&DAT_8006f440 + (uint)DAT_800be572 * 8) +
                   (short)DAT_800b0fe4 * 0x20;
    FUN_80013c50(&DAT_80190000,2,&DAT_800106b0);
    DAT_800be571 = 0;
  }
  iVar1 = FUN_80061fc0(0xffffffff);
  DAT_80072ec0 = iVar1 - DAT_80072ec0;
  while ((DAT_800aca38 & 0x2000000) != 0) {
    FUN_80029ac8(1);
  }
  FUN_80021634(0,0);
  DAT_800b5457 = 1;
  FUN_800444b0();
  FUN_80029ac8(1);
  DAT_80072ec4 = 0x3c;
  return;
}
