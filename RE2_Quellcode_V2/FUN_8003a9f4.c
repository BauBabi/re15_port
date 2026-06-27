/* FUN_8003a9f4 @ 0x8003a9f4  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a9f4(void)

{
  undefined2 uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint unaff_s3;
  
  iVar5 = DAT_800ea23c;
  iVar2 = FUN_80039ed4();
  uVar1 = DAT_800d4820;
  iVar4 = DAT_800ce32c;
  if (iVar2 != 0) goto LAB_8003b1e0;
  if (DAT_800a3e84 == '\0') {
    if ((DAT_800a3e80 != 0) && ((DAT_800ce30c & 0x1000) != 0)) {
      DAT_800a3e7c = 1;
    }
  }
  else if ((DAT_800a3e84 == '\x01') && (DAT_800a3e80 != 0)) {
    if ((DAT_800ce306 & 0xf0) != 0) {
      DAT_800a3e7c = 1;
    }
    if ((DAT_800ce306 & 0xf) != 0) {
      DAT_800a3e7c = 1;
    }
  }
  if (DAT_800a3e7c != 0) {
    DAT_800c3b54 = (undefined *)0x0;
    DAT_800c3b50 = (undefined *)0x0;
  }
  unaff_s3 = (uint)DAT_800d4235;
  switch(unaff_s3) {
  case 0:
    DAT_800ea23c = DAT_800ce32c;
    DAT_800ce32c = DAT_800ce32c + 0x53e0;
    FUN_8003b7e8(DAT_800cfb7c >> 4,1);
    DAT_800c3b48 = FUN_800696cc(0x35);
    if ((int)((uint)DAT_800c3b48 << 0x18) < 0) {
      DAT_800c3b48 = FUN_800696cc(0x35);
    }
    DAT_800a3e80 = 0;
    DAT_800a3e7c = 0;
    DAT_800d4235 = 1;
    iVar5 = iVar4;
  case 1:
    DAT_800c3b54 = &DAT_800a3dc8 + DAT_800a3e68;
    FUN_8003a298(1,"DOOR LOCK SERVICE]------------------------------",&DAT_800a3e80);
    if ((DAT_800a3e80 & DAT_800a3e7c) != 0) {
      DAT_800d4235 = 2;
    }
    break;
  case 2:
    iVar4 = FUN_80077360(&DAT_800d4854,0x46);
    if (iVar4 != 0) {
      DAT_800c3b50 = &DAT_800a3dc8 + DAT_800a3e6a;
      DAT_800c3b54 = &DAT_800a3dc8 + DAT_800a3e6c;
      FUN_8003a298(1,"Hall side doors:      |2RELEASED|0]",&DAT_800a3e80);
      uVar3 = DAT_800a3e80 & DAT_800a3e7c;
      goto joined_r0x8003b0e8;
    }
    DAT_800c3b50 = &DAT_800a3dc8 + DAT_800a3e6e;
    DAT_800c3b54 = &DAT_800a3dc8 + DAT_800a3e70;
    FUN_8003a298(1,"Hall side doors:      |2LOCKED|0]",&DAT_800a3e80);
    if ((DAT_800a3e80 & DAT_800a3e7c) != 0) {
      DAT_800d4235 = 3;
    }
    break;
  case 3:
    DAT_800c3b50 = &DAT_800a3dc8 + DAT_800a3e72;
    DAT_800c3b54 = &DAT_800a3dc8 + DAT_800a3e74;
    FUN_8003a298(1,"]The doors can be unlocked]by a |2CARD KEY|0.]",&DAT_800a3e80);
    if ((DAT_800a3e80 & DAT_800a3e7c) != 0) {
      iVar4 = FUN_80077360(&DAT_800d4854,0x46);
      if (iVar4 != 0) goto LAB_8003b1a4;
      DAT_800d4235 = 4;
    }
    break;
  case 4:
    iVar4 = FUN_800696cc(0x35);
    if (-1 < iVar4) {
      DAT_800e873f = 0x35;
      FUN_8002fe38(0,0x100,7,0xff000000);
    }
    DAT_800d4235 = 5;
    break;
  case 5:
    if ((DAT_800e873c & 0x80) == 0) {
      iVar4 = FUN_800696cc(0x35);
      if (iVar4 < 0) {
        DAT_800d4235 = 0xb;
        FUN_8003a298(0,&DAT_80010c78,&DAT_800a3e80);
      }
      else {
        if ((DAT_800e873c & 1) != 0) {
          FUN_8003a298(0,&DAT_80010c78,&DAT_800a3e80);
          FUN_8003a668();
          goto LAB_8003b1a4;
        }
        FUN_8007730c(&DAT_800d4854,0x46);
        iVar4 = FUN_800696cc(0x35);
        iVar4 = iVar4 * 4;
        (&DAT_800d4a3c)[iVar4] = 0;
        (&DAT_800d4a3d)[iVar4] = 0;
        (&DAT_800d4a3e)[iVar4] = 0;
        FUN_80069714();
        DAT_800d4235 = 6;
      }
    }
    break;
  case 6:
    DAT_800c3b54 = &DAT_800a3dc8 + DAT_800a3e76;
    FUN_8003a298(1,"]Checking up ID CARD[025.[025.[025.[025.",&DAT_800a3e80);
    if (DAT_800a3e80 != 0) {
      DAT_800d4235 = 7;
    }
    break;
  case 7:
    if (((DAT_800cfbd8 & 0x40000000) == 0) || (iVar4 = FUN_80077360(&DAT_800d4a34,0x11), iVar4 == 0)
       ) {
      iVar4 = 0;
      DAT_800cfbf2 = 9;
      DAT_800dfc1b = 1;
      do {
        FUN_80031f94(1);
        iVar4 = iVar4 + 1;
      } while (iVar4 < 0x3c);
      FUN_8005ba28(0x2120000,0);
      iVar4 = 0;
      do {
        FUN_80031f94(1);
        iVar4 = iVar4 + 1;
      } while (iVar4 < 0x3c);
    }
    iVar4 = 0;
    DAT_800cfbf2 = 2;
    DAT_800dfc1b = 1;
    do {
      FUN_80031f94(1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < 0x3c);
    FUN_8005ba28(0x2120000,0);
    iVar4 = 0;
    do {
      FUN_80031f94(1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < 0x3c);
    iVar4 = 0;
    if ((DAT_800cfbd8 & 0x40000000) != 0) {
      DAT_800cfbf2 = 0xc;
      DAT_800dfc1b = 1;
      do {
        FUN_80031f94(1);
        iVar4 = iVar4 + 1;
      } while (iVar4 < 0x3c);
      FUN_8005ba28(0x2120000,0);
      iVar4 = 0;
      do {
        FUN_80031f94(1);
        iVar4 = iVar4 + 1;
      } while (iVar4 < 0x3c);
    }
    DAT_800dfc1b = 1;
    DAT_800cfbf2 = (undefined1)uVar1;
    DAT_800d4235 = 8;
    break;
  case 8:
    DAT_800c3b54 = &DAT_800a3dc8 + DAT_800a3e78;
    FUN_8003a298(1,"]OK!]Hall side doors lock released.",&DAT_800a3e80);
    uVar3 = DAT_800a3e80 & DAT_800a3e7c;
joined_r0x8003b0e8:
    if (uVar3 != 0) {
LAB_8003b1a4:
      DAT_800d4235 = 0xb;
    }
    break;
  case 9:
    DAT_800c3b54 = &DAT_800a3dc8 + DAT_800a3e7a;
    FUN_8003a298(1,"press any key]>",&DAT_800a3e80);
    DAT_800a3e84 = '\x01';
    if ((DAT_800a3e80 & DAT_800a3e7c) != 0) {
      FUN_8003a668();
      DAT_800d4235 = 0xb;
      DAT_800a3e84 = '\0';
    }
    break;
  case 10:
    iVar4 = FUN_800398ac(0,2);
    if (iVar4 != 0) {
      FUN_8003a668();
      DAT_800d4236 = 2;
      goto LAB_8003b1a4;
    }
    break;
  case 0xb:
    FUN_8003b918(1);
    return;
  }
  if (*(char *)(iVar5 + 0x2d9) == '\x01') {
    *(undefined1 *)(iVar5 + 0x2d9) = 0;
    DAT_800d4235 = 0xb;
  }
LAB_8003b1e0:
  if (((DAT_800a3e7c & DAT_800a3e80) != 0) || (unaff_s3 != DAT_800d4235)) {
    DAT_800a3e80 = 0;
    DAT_800a3e7c = 0;
    DAT_800c3b54 = (undefined *)0x0;
    DAT_800c3b50 = (undefined *)0x0;
  }
  FUN_8003a6ac(1);
  FUN_8003a794(0x16c);
  return;
}


