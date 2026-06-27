/* FUN_80049e48 @ 0x80049e48  (Ghidra headless, raw MIPS overlay) */

void FUN_80049e48(void)

{
  byte bVar1;
  undefined2 uVar2;
  undefined1 uVar3;
  ushort uVar4;
  undefined4 *puVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  
  DAT_800cfb70 = 0;
  DAT_800cfddc = 0;
  DAT_800cfbd8 = DAT_800cfbd8 & 0xfff04000;
  DAT_800cfd4c = DAT_800cfd4c & 63999;
  DAT_800ce32c = DAT_800ce328;
  DAT_800ce324 = DAT_800ce328;
  FUN_8005a444();
  puVar5 = &DAT_800cfe18;
  iVar8 = 0x21;
  DAT_800ce334 = &DAT_800cfe1c;
  DAT_800cfbf3 = 0;
  do {
    *puVar5 = &DAT_800d424c;
    iVar8 = iVar8 + -1;
    puVar5 = puVar5 + 1;
  } while (iVar8 != 0);
  DAT_800d031e = 0;
  DAT_800d0322 = 0;
  DAT_800d4224 = &DAT_800d0324;
  uVar9 = 0;
  do {
    uVar9 = uVar9 + 1;
    DAT_800d0324 = 0;
  } while (uVar9 < 0x20);
  if (DAT_800cfc00 != DAT_800d482c) {
    if ((short)DAT_800d482c < 0xc) {
      if ((DAT_800d482c & 1) == 0) {
        iVar8 = FUN_80077360(&DAT_800d4a2c,5);
        if (iVar8 != 0) {
          DAT_800d482c = 8;
        }
        iVar8 = FUN_80077360(&DAT_800d4a2c,0xf);
        uVar4 = 10;
      }
      else {
        iVar8 = FUN_80077360(&DAT_800d4a2c,6);
        uVar4 = 9;
      }
      if (iVar8 != 0) {
        DAT_800d482c = uVar4;
      }
    }
    bVar1 = DAT_800cfc00;
    DAT_800cfc00 = (byte)DAT_800d482c;
    FUN_80068f9c((int)(short)DAT_800d482c);
    FUN_8003baf0(&DAT_800cfbf8);
    uVar3 = DAT_800cfe12;
    uVar2 = DAT_800cfe10;
    DAT_800cfbfc = 0;
    if (DAT_800d482c - 0xe < 2) {
      DAT_800cfe12 = 0;
      DAT_800cfe10 = 0;
      DAT_800d46be = DAT_800cfd4e;
      DAT_800d46b3 = uVar3;
      DAT_800d46b4 = uVar2;
      DAT_800cfd4e = DAT_800cfd5a;
    }
    else if (0xb < bVar1) {
      DAT_800cfd4e = DAT_800d46be;
      DAT_800cfe12 = DAT_800d46b3;
      DAT_800cfe10 = DAT_800d46b4;
    }
    DAT_800d5c0b = 1;
    FUN_80059a00((int)(short)DAT_800d482c,1);
  }
  DAT_800dfad7 = '\0';
  FUN_80031f94(1);
  if ((int)DAT_800d481c != (uint)DAT_800cfb85) {
    DAT_800cfb85 = (byte)DAT_800d481c;
    FUN_80012fb8(((int)DAT_800d481c & 0xffU) + 0xd5,&DAT_801bfa14,1,"FONT TIM");
    DAT_800cfbf0 = 0x1d;
    FUN_80076a40(&DAT_801bfa14);
    FUN_80031f94(1);
    FUN_80029c94();
  }
  DAT_800cfbf0 = 0xd05;
  if (DAT_800d44a0 == '\0') {
    uVar9 = (uint)*(ushort *)((&PTR_DAT_800a7210)[DAT_800d481c] + DAT_800d481e * 2);
  }
  else {
    uVar9 = *(ushort *)((&PTR_DAT_800a7210)[DAT_800d481c] + DAT_800d481e * 2) + 0x7d;
  }
  iVar8 = FUN_80012fb8(uVar9,DAT_800ce324,1,&DAT_800110c8);
  piVar6 = (int *)(DAT_800ce324 + 8);
  do {
    if (*piVar6 != 0) {
      *piVar6 = *piVar6 + DAT_800ce324;
    }
    piVar6 = piVar6 + 1;
  } while (piVar6 < (int *)(DAT_800ce324 + 100));
  uVar9 = 0;
  if (*(char *)(DAT_800ce324 + 1) != '\0') {
    do {
      iVar7 = uVar9 * 0x20 + *(int *)(DAT_800ce324 + 0x24);
      *(int *)(iVar7 + 0x1c) = *(int *)(iVar7 + 0x1c) + DAT_800ce324;
      uVar9 = uVar9 + 1;
    } while (uVar9 < *(byte *)(DAT_800ce324 + 1));
  }
  DAT_800ce32c = DAT_800ce32c + iVar8;
  uVar9 = 0;
  if (*(char *)(DAT_800ce324 + 2) != '\0') {
    do {
      piVar6 = (int *)(uVar9 * 8 + *(int *)(DAT_800ce324 + 0x30));
      *piVar6 = *piVar6 + DAT_800ce324;
      iVar8 = uVar9 * 8 + *(int *)(DAT_800ce324 + 0x30);
      *(int *)(iVar8 + 4) = *(int *)(iVar8 + 4) + DAT_800ce324;
      uVar9 = uVar9 + 1;
    } while (uVar9 < *(byte *)(DAT_800ce324 + 2));
  }
  FUN_8002c7ac((int)DAT_800d4820);
  FUN_8001bba4();
  FUN_80059e54();
  FUN_80052d14();
  FUN_8005a888();
  if (*(int *)(DAT_800ce324 + 0x10) != 0) {
    DAT_800ce32c = *(int *)(DAT_800ce324 + 0x10);
  }
  DAT_800ce330 = &DAT_800cfbf8;
  DAT_800cfbfc = 0;
  FUN_8003bfac();
  FUN_80053528();
  FUN_8005a09c();
  FUN_80018e08();
  FUN_80049a9c();
  FUN_8001aaa0();
  FUN_8004a694();
  FUN_80049ad0();
  while (DAT_800dfad7 != '\0') {
    FUN_80031f94(1);
  }
  FUN_8006931c();
  return;
}


