void FUN_80021bbc(void)

{
  undefined2 uVar1;
  
  if (DAT_800b5457 != '\x02') {
    DAT_800b0fe8 = DAT_800b0fe4;
    DAT_800b0fe4 = (ushort)DAT_800afbb5;
    DAT_800ac790 = FUN_80014324(DAT_800afbb5);
    if ((DAT_800aca38 & 0x100000) == 0) {
      FUN_800392d4();
    }
    if ((DAT_800aca38 & 0x20000000) == 0) {
      DAT_800be571 = 2;
      DAT_800be574 = (uint)*(ushort *)((short)DAT_800b0fe4 * 2 + _DAT_800b0fe2 * 0x20);
      DAT_800be572 = *(short *)((&PTR_DAT_8007438c)[DAT_800b0fe0] + _DAT_800b0fe2 * 2) - 1;
      DAT_800be57c = (uint)(byte)(&DAT_8006f442)[(uint)DAT_800be572 * 8] * 0x10000 +
                     (uint)*(ushort *)(&DAT_8006f440 + (uint)DAT_800be572 * 8) +
                     (short)DAT_800b0fe4 * 0x20;
      FUN_80013c50(&DAT_80190000,0,&DAT_80010708);
      DAT_800be571 = 0;
    }
    else {
      DAT_800aca38 = DAT_800aca38 & 0xdfffffff;
    }
    if (*(int *)(&DAT_8018fffc + *(ushort *)((short)DAT_800b0fe4 * 2 + _DAT_800b0fe2 * 0x20)) != 0)
    {
      FUN_800c47e8(&UNK_80190004 +
                   *(int *)(&DAT_8018fff8 +
                           *(ushort *)((short)DAT_800b0fe4 * 2 + _DAT_800b0fe2 * 0x20)),
                   *(undefined4 *)
                    (&DAT_80190000 +
                    *(int *)(&DAT_8018fff8 +
                            *(ushort *)((short)DAT_800b0fe4 * 2 + _DAT_800b0fe2 * 0x20))),
                   &DAT_801a5800);
      uVar1 = _DAT_800aca4c;
      _DAT_800aca4c = 0x15;
      FUN_8004ee78(&DAT_801a5800);
      _DAT_800aca4c = uVar1;
    }
  }
  DAT_80072f2e = -(ushort)(DAT_800aca34 != '\0') & 0xf0;
  FUN_80053a8c(&DAT_80072f2c,&DAT_80190000,&DAT_80199e00,&DAT_80198000,0);
  StoreImage((RECT *)&DAT_80072f2c,(u_long *)&DAT_80198000);
  FUN_80066c30(*(ushort *)((short)DAT_800b0fe4 * 0x20 + *(int *)(DAT_800ac778 + 0x24) + 2) >> 7);
  FUN_80053ca4((short)DAT_800b0fe4 * 0x20 + *(int *)(DAT_800ac778 + 0x24));
  DAT_800b5457 = 0;
  return;
}
