/* FUN_8002bdf4 @ 0x8002bdf4  (Ghidra headless, raw MIPS overlay) */

void FUN_8002bdf4(void)

{
  undefined1 uVar1;
  undefined1 uVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  uVar2 = DAT_800cbc21;
  uVar1 = DAT_800cbc20;
  DAT_800cbc21 = 0;
  DAT_800cbc20 = 0;
  if (DAT_800dfc1b != '\x02') {
    DAT_800d4824 = DAT_800d4820;
    DAT_800d4820 = (ushort)DAT_800cfbf2;
    DAT_800ce338 = FUN_8002c7dc(DAT_800cfbf2);
    if ((DAT_800cfb74 & 0x100000) == 0) {
      FUN_80049b1c();
    }
    iVar7 = DAT_800cfae8;
    if ((DAT_800cfb74 & 0x20000000) == 0) {
      DAT_800d5302 = 2;
      iVar6 = (int)(short)DAT_800d4820;
      iVar5 = (int)DAT_800d481e;
      DAT_800d5308 = (uint)*(ushort *)(iVar6 * 2 + iVar5 * 0x20 + DAT_800cfae8);
      DAT_800d531c = *(short *)((&PTR_DAT_800a7210)[DAT_800d481c] + iVar5 * 2) - 0x23a;
      iVar4 = (uint)DAT_800d531c * 8;
      DAT_800d531e = *(undefined1 *)(iVar5 * 0x10 + DAT_800ce540 + iVar6);
      DAT_800d5314 = (uint)(byte)(&DAT_800988aa)[iVar4] * 0x10000 +
                     (uint)*(ushort *)(&DAT_800988a8 + iVar4) + iVar6 * 0x20;
      FUN_80012fb8((uint)DAT_800d531c,&DAT_80190000,2,&DAT_80010a10);
      DAT_800d5302 = 0;
    }
    else {
      DAT_800cfb74 = DAT_800cfb74 & 0xdfffffff;
    }
    if (*(int *)(&DAT_8018fffc + *(ushort *)((short)DAT_800d4820 * 2 + DAT_800d481e * 0x20 + iVar7))
        != 0) {
      FUN_80010000(&DAT_801a5800);
      iVar7 = *(int *)(&DAT_8018fff8 +
                      *(ushort *)((short)DAT_800d4820 * 2 + DAT_800d481e * 0x20 + iVar7));
      do {
        FUN_80010778(0x1f800000,&DAT_80190000 + iVar7,0x400);
        iVar7 = iVar7 + 0x400;
        iVar4 = FUN_80010084();
        uVar3 = DAT_800cfbf0;
      } while (iVar4 == 0);
      DAT_800cfbf0 = 0x15;
      FUN_80076a40(&DAT_801a5800);
      DAT_800cfbf0 = uVar3;
    }
  }
  iVar7 = 0;
  DAT_8009dc20 = 0;
  DAT_8009dc22 = *(undefined2 *)(DAT_800dfc10 + 10);
  DAT_800dfd4c._2_2_ = DAT_8009dc22;
  DecDCTReset(0);
  DecDCToutCallback((func *)0x0);
  DecDCTvlc((u_long *)&DAT_80190000,(u_long *)&DAT_80199e00);
  DecDCTin((u_long *)&DAT_80199e00,0);
  do {
    DecDCTout(&DAT_80198000,0x780);
    DecDCToutSync(0);
    LoadImage((RECT *)&DAT_8009dc20,&DAT_80198000);
    iVar7 = iVar7 + 0x10;
    DAT_8009dc20 = DAT_8009dc20 + 0x10;
  } while (iVar7 < 0x140);
  StoreImage((RECT *)&DAT_800dfd4c,&DAT_80198000);
  FUN_8008de24(*(ushort *)((short)DAT_800d4820 * 0x20 + *(int *)(DAT_800ce324 + 0x24) + 2) >> 7);
  iVar7 = *(int *)(DAT_800ce324 + 0x24) + (short)DAT_800d4820 * 0x20;
  FUN_80076cb0(iVar7 + 4,iVar7 + 0x10);
  DAT_800cbc20 = uVar1;
  DAT_800cbc21 = uVar2;
  return;
}


