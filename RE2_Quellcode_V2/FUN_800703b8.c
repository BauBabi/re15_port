/* FUN_800703b8 @ 0x800703b8  (Ghidra headless, raw MIPS overlay) */

void FUN_800703b8(void)

{
  undefined1 uVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  byte bVar4;
  byte bVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  undefined *puVar11;
  char cVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  
  bVar5 = DAT_800d5bfc;
  uVar14 = (uint)DAT_800d5bfc;
  iVar6 = uVar14 * 4;
  uVar1 = (&DAT_800d4a3c)[iVar6];
  uVar2 = (&DAT_800d4a3d)[iVar6];
  uVar3 = (&DAT_800d4a3e)[iVar6];
  uVar15 = DAT_800d5c14 + 2 & 0x3f;
  cVar12 = ((&DAT_800d4a3e)[iVar6] != '\0') * '\x02';
  iVar7 = uVar15 * 4;
  if ((&DAT_800d4a6a)[iVar7] != '\0') {
    cVar12 = cVar12 + '\x01';
  }
  bVar4 = (&DAT_800d4a69)[iVar7];
  uVar10 = (uint)(byte)(&DAT_800d4a68)[iVar7];
  if (uVar10 - 0x14 < 0xc) {
    if (uVar10 == (&DAT_800d4a3c)[iVar6]) {
      bVar5 = (&DAT_800a9e1c)[uVar10 * 8];
      if ((uint)(byte)(&DAT_800d4a3d)[iVar6] == (uint)bVar5) {
        (&DAT_800d4a3d)[iVar6] = bVar4;
        (&DAT_800d4a69)[iVar7] = bVar5;
        return;
      }
      uVar8 = (uint)(byte)(&DAT_800d4a3d)[iVar6] + (uint)bVar4;
      uVar9 = uVar14;
      if (bVar5 < uVar8) {
        (&DAT_800d4a3d)[iVar6] = bVar5;
        (&DAT_800d4a69)[iVar7] = (char)uVar8 - (&DAT_800a9e1c)[uVar10 * 8];
        return;
      }
LAB_8007059c:
      (&DAT_800d4a3d)[uVar9 * 4] = (&DAT_800d4a3d)[uVar9 * 4] + bVar4;
      iVar6 = uVar15 * 4;
      (&DAT_800d4a68)[iVar6] = 0;
      (&DAT_800d4a69)[iVar6] = 0;
      (&DAT_800d4a6a)[iVar6] = 0;
      FUN_80069c40(0x10,(uint)(byte)(&DAT_800d4a68)[iVar6],
                   &DAT_8019c000 + (uint)(byte)(&DAT_800d4a68)[iVar6] * 0x4b0);
      return;
    }
    if ((&DAT_800d4a3c)[iVar6] == 0) {
      uVar9 = 0;
      iVar6 = 0;
      do {
        if ((uVar10 == (&DAT_800d4a3c)[iVar6]) &&
           ((uint)(byte)(&DAT_800d4a3d)[iVar6] <=
            (uint)(byte)(&DAT_800a9e1c)[uVar10 * 8] - (uint)bVar4)) goto LAB_8007059c;
        uVar9 = uVar9 + 1;
        iVar6 = uVar9 * 4;
      } while (uVar9 < DAT_800d46ac);
    }
  }
  if (cVar12 == '\x01') {
    iVar6 = FUN_80069668(1);
    if (((iVar6 == 1) && ((&DAT_800d4a3c)[uVar14 * 4] == '\0')) || (iVar6 == 0)) {
      FUN_8002fe38(0xaf0010,0xe400,8,0);
      DAT_800d5bf2 = 5;
      return;
    }
    if (DAT_800d5bf8 == uVar14) {
      DAT_800d5bf8 = 0x80;
      DAT_800d5bfa = 0;
    }
    (&DAT_800d4a3c)[uVar14 * 4] = 0;
    FUN_80069714();
    FUN_800698b4(1);
    iVar6 = uVar15 * 4;
    DAT_800d4a3c = (&DAT_800d4a68)[iVar6];
    DAT_800d4a3d = (&DAT_800d4a69)[iVar6];
    DAT_800d4a3e = 1;
    DAT_800d4a42 = 2;
    DAT_800d4a40 = DAT_800d4a3c;
    DAT_800d4a41 = DAT_800d4a3d;
    (&DAT_800d4a68)[iVar6] = uVar1;
    (&DAT_800d4a69)[iVar6] = uVar2;
    (&DAT_800d4a6a)[iVar6] = uVar3;
    FUN_80069c40(0x10,(uint)(byte)(&DAT_800d4a68)[iVar6],
                 &DAT_8019c000 + (uint)(byte)(&DAT_800d4a68)[iVar6] * 0x4b0);
    FUN_80069bb4(0,&DAT_8019c000 + (uint)DAT_800d4a3c * 0x4b0);
    iVar6 = 1;
    puVar11 = &DAT_8019c000 + (DAT_800d4a3c + 0x56) * 0x4b0;
    goto LAB_80070e28;
  }
  if (cVar12 == '\0') {
    if (DAT_800d5bf8 == uVar14) {
      DAT_800d5bf8 = 0x80;
      DAT_800d5bfa = 0;
    }
    uVar10 = FUN_80069668(0);
    if (uVar10 < uVar14) {
      uVar14 = uVar10;
    }
    iVar7 = uVar14 * 4;
    iVar6 = uVar15 * 4;
    (&DAT_800d4a3c)[iVar7] = (&DAT_800d4a68)[iVar6];
    (&DAT_800d4a3d)[iVar7] = (&DAT_800d4a69)[iVar6];
    (&DAT_800d4a3e)[iVar7] = (&DAT_800d4a6a)[iVar6];
    (&DAT_800d4a68)[iVar6] = uVar1;
    (&DAT_800d4a69)[iVar6] = uVar2;
    (&DAT_800d4a6a)[iVar6] = uVar3;
    FUN_80069c40(0x10,(uint)(byte)(&DAT_800d4a68)[iVar6],
                 &DAT_8019c000 + (uint)(byte)(&DAT_800d4a68)[iVar6] * 0x4b0);
    FUN_80069bb4((uVar14 >> 1) + uVar14,&DAT_8019c000 + (uint)(&DAT_800d4a3c)[iVar7] * 0x4b0);
    FUN_80069714();
    return;
  }
  if (cVar12 != '\x02') {
    if (cVar12 != '\x03') {
      return;
    }
    if ((&DAT_800d4a3e)[uVar14 * 4] == '\x01') {
      if ((DAT_800d5bf8 == uVar14) || ((uint)DAT_800d5bf8 == uVar14 + 1)) {
        DAT_800d5bf8 = 0x80;
        DAT_800d5bfa = 0;
      }
      iVar7 = (uVar14 + 1) * 4;
      iVar6 = uVar15 * 4;
      iVar13 = uVar14 * 4;
      uVar3 = (&DAT_800d4a68)[iVar6];
      (&DAT_800d4a3c)[iVar7] = uVar3;
      (&DAT_800d4a3c)[iVar13] = uVar3;
      uVar3 = (&DAT_800d4a69)[iVar6];
      (&DAT_800d4a3d)[iVar7] = uVar3;
      (&DAT_800d4a3d)[iVar13] = uVar3;
      (&DAT_800d4a3e)[iVar13] = 1;
      (&DAT_800d4a3e)[iVar7] = 2;
      (&DAT_800d4a68)[iVar6] = uVar1;
      (&DAT_800d4a6a)[iVar6] = 3;
      (&DAT_800d4a69)[iVar6] = uVar2;
      FUN_80069c40(0x10,(uint)(byte)(&DAT_800d4a68)[iVar6],
                   &DAT_8019c000 + (uint)(byte)(&DAT_800d4a68)[iVar6] * 0x4b0);
      FUN_80069bb4((bVar5 >> 1) + uVar14,&DAT_8019c000 + (uint)(&DAT_800d4a3c)[iVar13] * 0x4b0);
      iVar6 = (uVar14 + 1 >> 1) + uVar14 + 1;
      uVar15 = (&DAT_800d4a3c)[iVar13] + 0x56;
    }
    else {
      if ((DAT_800d5bf8 == uVar14) || ((uint)DAT_800d5bf8 == uVar14 - 1)) {
        DAT_800d5bf8 = 0x80;
        DAT_800d5bfa = 0;
      }
      iVar7 = (uVar14 - 1) * 4;
      iVar6 = uVar15 * 4;
      iVar13 = uVar14 * 4;
      uVar3 = (&DAT_800d4a68)[iVar6];
      (&DAT_800d4a3c)[iVar7] = uVar3;
      (&DAT_800d4a3c)[iVar13] = uVar3;
      uVar3 = (&DAT_800d4a69)[iVar6];
      (&DAT_800d4a3d)[iVar7] = uVar3;
      (&DAT_800d4a3d)[iVar13] = uVar3;
      (&DAT_800d4a3e)[iVar13] = 2;
      (&DAT_800d4a3e)[iVar7] = 1;
      (&DAT_800d4a68)[iVar6] = uVar1;
      (&DAT_800d4a6a)[iVar6] = 3;
      (&DAT_800d4a69)[iVar6] = uVar2;
      FUN_80069c40(0x10,(uint)(byte)(&DAT_800d4a68)[iVar6],
                   &DAT_8019c000 + (uint)(byte)(&DAT_800d4a68)[iVar6] * 0x4b0);
      FUN_80069bb4((bVar5 >> 1) + uVar14,&DAT_8019c000 + ((&DAT_800d4a3c)[iVar13] + 0x56) * 0x4b0);
      uVar15 = (uint)(&DAT_800d4a3c)[iVar13];
      iVar6 = (uVar14 - 1 >> 1) + uVar14 + -1;
    }
    puVar11 = &DAT_8019c000 + uVar15 * 0x4b0;
    goto LAB_80070e28;
  }
  if ((&DAT_800d4a3e)[uVar14 * 4] == '\x02') {
    if ((DAT_800d5bf8 == uVar14) || ((uint)DAT_800d5bf8 == uVar14 - 1)) {
      DAT_800d5bf8 = 0x80;
      DAT_800d5bfa = 0;
    }
    (&DAT_800d4a3c)[uVar14 * 4] = 0;
    FUN_80069714();
    (&DAT_800d4a3c)[(uVar14 - 1) * 4] = 0;
LAB_80070a38:
    FUN_80069714();
  }
  else if ((&DAT_800d4a3e)[uVar14 * 4] == '\x01') {
    if ((DAT_800d5bf8 == uVar14) || ((uint)DAT_800d5bf8 == uVar14 + 1)) {
      DAT_800d5bf8 = 0x80;
      DAT_800d5bfa = 0;
    }
    (&DAT_800d4a3c)[uVar14 * 4] = 0;
    FUN_80069714();
    (&DAT_800d4a3c)[uVar14 * 4] = 0;
    goto LAB_80070a38;
  }
  iVar6 = FUN_80069668(0);
  iVar13 = iVar6 * 4;
  iVar7 = uVar15 * 4;
  (&DAT_800d4a3c)[iVar13] = (&DAT_800d4a68)[iVar7];
  (&DAT_800d4a3d)[iVar13] = (&DAT_800d4a69)[iVar7];
  (&DAT_800d4a3e)[iVar13] = (&DAT_800d4a6a)[iVar7];
  (&DAT_800d4a68)[iVar7] = uVar1;
  (&DAT_800d4a6a)[iVar7] = 3;
  (&DAT_800d4a69)[iVar7] = uVar2;
  FUN_80069c40(0x10,(uint)(byte)(&DAT_800d4a68)[iVar7],
               &DAT_8019c000 + (uint)(byte)(&DAT_800d4a68)[iVar7] * 0x4b0);
  iVar6 = iVar6 / 2 + iVar6;
  puVar11 = &DAT_8019c000 + (uint)(&DAT_800d4a3c)[iVar13] * 0x4b0;
LAB_80070e28:
  FUN_80069bb4(iVar6,puVar11);
  return;
}


