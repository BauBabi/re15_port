/* FUN_8006b358 @ 0x8006b358  (Ghidra headless, raw MIPS overlay) */

void FUN_8006b358(void)

{
  char cVar1;
  undefined1 uVar2;
  byte bVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  undefined1 unaff_s0;
  uint uVar7;
  uint uVar8;
  int iVar9;
  
  DAT_800d5c06 = 0;
  uVar7 = (uint)(byte)(&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] +
          (uint)(byte)(&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4];
  switch(DAT_800d5bf4) {
  case '\0':
    uVar7 = (uint)DAT_800d5bfd;
    if (DAT_800cfb74 < 0) {
      if (((DAT_800ce304 & 0x2000) != 0) && ((int)uVar7 < (int)(DAT_800d46ac - 1))) {
        if ((&DAT_800d4a3e)[uVar7 * 4] == '\x01') {
          DAT_800d5bfd = DAT_800d5bfd + 1;
        }
        DAT_800d5bfd = DAT_800d5bfd + 1;
      }
      if ((((DAT_800ce304 & 0x8000) != 0) && (uVar8 = (uint)DAT_800d5bfd, uVar8 != 0)) &&
         (uVar8 != 10)) {
        bVar3 = DAT_800d5bfd;
        if (((&DAT_800d4a3e)[uVar8 * 4] == '\x02') && (bVar3 = DAT_800d5bfd - 1, uVar8 == 1)) {
          bVar3 = DAT_800d5bfd + 1;
        }
        DAT_800d5bfd = bVar3;
        DAT_800d5bfd = DAT_800d5bfd - 1;
      }
      if ((DAT_800ce304 & 0x4000) != 0) {
        bVar3 = DAT_800d5bfd + 2;
        if (((int)(uint)DAT_800d5bfd < (int)(DAT_800d46ac - 2)) || (bVar3 = 1, DAT_800d5bfd == 10))
        {
          DAT_800d5bfd = bVar3;
        }
      }
      bVar3 = DAT_800d5bfd;
      if ((((DAT_800ce304 & 0x1000) != 0) && (DAT_800d5bfd != 10)) && (bVar3 = 10, 1 < DAT_800d5bfd)
         ) {
        bVar3 = DAT_800d5bfd - 2;
      }
      DAT_800d5bfd = bVar3;
      if (uVar7 != DAT_800d5bfd) {
        FUN_8005ba28(0x4040000,0);
      }
    }
    if ((DAT_800ce310 & 0x1000) == 0) {
      if ((DAT_800ce310 & 0x2000) != 0) {
        FUN_8005ba28(0x4050000,0);
        DAT_800d5bf2 = 1;
        DAT_800d5bf3 = 0;
        DAT_800d5bf4 = '\0';
      }
    }
    else {
      DAT_800d5bf4 = FUN_800695b0();
      uVar6 = 0x4070000;
      if (DAT_800d5bf4 != '\0') {
        uVar6 = 0x4060000;
      }
      FUN_8005ba28(uVar6,0);
    }
    FUN_80073674((&DAT_800d4a3e)[(uint)DAT_800d5bfd * 4]);
    FUN_800693d0(0x10,0xaf,2,(&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4]);
    break;
  case '\x01':
    if (9 < DAT_800d5bf5) {
      if (DAT_800d5bf6 != '\0') {
        DAT_800d5bfc = DAT_800d5bfd;
      }
      DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
      DAT_800d5bf5 = 0;
      DAT_800d5bf6 = 0;
      DAT_800d5c06 = 0;
      DAT_800d5c0c = 0;
      DAT_800d5c0d = 0;
      DAT_800d5c0e = 0;
      DAT_800d5c0f = 0;
      DAT_800d5c10 = 0;
      DAT_800d5c11 = 0;
      DAT_800d5c12 = 0;
      DAT_800d5c13 = 0;
      return;
    }
    DAT_800d5c0e = DAT_800d5c0e + DAT_800d5c0c;
    DAT_800d5c0f = DAT_800d5c0f + DAT_800d5c0d;
    DAT_800d5c12 = DAT_800d5c12 + DAT_800d5c10;
    DAT_800d5c13 = DAT_800d5c13 + DAT_800d5c11;
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    goto LAB_8006c278;
  case '\x02':
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    if (DAT_800d5bf5 < 4) {
LAB_8006b818:
      if (DAT_800d5bf6 == '\0') {
        return;
      }
    }
    else {
      uVar7 = FUN_80069668(0);
      if (uVar7 < 9) {
        iVar4 = FUN_80069668(1);
        uVar8 = uVar7 + 1;
        if ((int)uVar7 < (int)((uint)DAT_800d46ac - iVar4)) {
          iVar9 = (int)uVar8 / 2 + 1 + uVar7;
          FUN_80069d88(iVar9,(int)uVar7 / 2 + uVar7);
          iVar5 = uVar7 * 4;
          iVar4 = uVar8 * 4;
          (&DAT_800d4a3c)[iVar5] = (&DAT_800d4a3c)[iVar4];
          (&DAT_800d4a3d)[iVar5] = (&DAT_800d4a3d)[iVar4];
          (&DAT_800d4a3e)[iVar5] = (&DAT_800d4a3e)[iVar4];
          DAT_800d5bf5 = 0;
          if (DAT_800d5bf8 == uVar8) {
            DAT_800d5bf8 = DAT_800d5bf8 - 1;
          }
          FUN_80069e54(iVar9);
          FUN_8006947c(uVar8,0,0,0);
          goto LAB_8006b818;
        }
      }
    }
    DAT_800d5bf2 = 4;
    DAT_800d5bf6 = '\0';
    DAT_800d5bf5 = 0;
    DAT_800d5bf4 = '\0';
    DAT_800d5bf3 = 0;
    break;
  case '\x03':
    if (DAT_800d5bf5 == 0) {
      DAT_800d5bf5 = DAT_800d5bf5 + 1;
      FUN_8002fe38(0xaf0010,0xe400,2,0);
    }
    else {
      DAT_800d5bf5 = DAT_800d5bf5 + 1;
      if ((DAT_800e873c & 0x80) == 0) {
        DAT_800d5bf5 = 0;
        DAT_800d5bf4 = '\0';
        DAT_800d5bf3 = 0;
        DAT_800d5bf2 = 0;
      }
    }
    break;
  case '\x04':
    FUN_80012fb8(0xac,&DAT_801a0000,0,"MIX ITP");
    FUN_80069bb4((uint)(DAT_800d5bfc >> 1) + (uint)DAT_800d5bfc,
                 &DAT_801a0000 + (uint)DAT_800d5c17 * 0x4b0);
    (&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] = DAT_800d5c16;
    (&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] = (&DAT_800a9e1c)[(uint)DAT_800d5c16 * 8];
    if (DAT_800d5bfc == DAT_800d5bf8) {
      DAT_800d5bfa = DAT_800d5c16;
    }
    if (DAT_800d5bfd == DAT_800d5bf8) {
      DAT_800d5bf8 = DAT_800d5bfc;
      DAT_800d5bfa = DAT_800d5c16;
    }
    FUN_8006947c(DAT_800d5bfd,0,0,0);
    FUN_80069e54((uint)(DAT_800d5bfd >> 1) + (uint)DAT_800d5bfd);
    uVar6 = 0;
    goto LAB_8006bf40;
  case '\x05':
    if (((&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] !=
         (&DAT_800a9e1c)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] * 8]) ||
       (0x13 < (byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4])) {
      iVar4 = (uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] * 8;
      if (((&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] != '\x0f') ||
         ((&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] != -1)) {
        if ((byte)(&DAT_800a9e1c)[iVar4] < uVar7) {
          FUN_800694b8(DAT_800d5bfc);
          (&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4] = (char)uVar7 - (&DAT_800a9e1c)[iVar4];
          uVar6 = 0;
        }
        else {
          FUN_800694b8(DAT_800d5bfc,uVar7);
          FUN_8006947c(DAT_800d5bfd,0,0,0);
          FUN_80069e54((uint)(DAT_800d5bfd >> 1) + (uint)DAT_800d5bfd);
          uVar6 = 0;
        }
        goto LAB_8006bf40;
      }
    }
    goto LAB_8006bb60;
  case '\x06':
    if (((&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4] !=
         (&DAT_800a9e1c)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] * 8]) ||
       (0x13 < (byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4])) {
      iVar4 = (uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] * 8;
      if (((&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] != '\x0f') ||
         ((&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4] != -1)) {
        if ((byte)(&DAT_800a9e1c)[iVar4] < uVar7) {
          FUN_800694b8(DAT_800d5bfd);
          (&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] = (char)uVar7 - (&DAT_800a9e1c)[iVar4];
        }
        else {
          FUN_800694b8(DAT_800d5bfd,uVar7);
          FUN_8006947c(DAT_800d5bfc,0,0,0);
          FUN_80069e54((uint)(DAT_800d5bfc >> 1) + (uint)DAT_800d5bfc);
        }
        DAT_800d5bf4 = 1;
        DAT_800d5bf5 = 0;
        DAT_800d5bf6 = 1;
        FUN_8006a38c(1);
        return;
      }
    }
LAB_8006bb60:
    DAT_800d5bf4 = '\x03';
    DAT_800d5bf5 = 0;
    break;
  case '\a':
    FUN_80012fb8(0xac,&DAT_801a0000,0,"MIX ITP");
    cVar1 = (&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4];
    (&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] = (&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] + -0xf;
    if ((uint)DAT_800d5bf8 == (uint)DAT_800d5bfc) {
      DAT_800d5bfa = (&DAT_800d4a3c)[(uint)DAT_800d5bf8 * 4];
    }
    (&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] = cVar1 + '\x0f';
    uVar2 = (&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4];
    (&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] = (&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4];
    (&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4] = uVar2;
    uVar7 = (uint)DAT_800d5bfd;
    if ((&DAT_800d4a3d)[uVar7 * 4] == '\0') {
      FUN_8006947c(uVar7,0,0,0);
      FUN_80069e54((uint)(DAT_800d5bfd >> 1) + (uint)DAT_800d5bfd);
      uVar6 = 0;
    }
    else {
      FUN_80069bb4((DAT_800d5bfd >> 1) + uVar7,&DAT_801a0000 + (uint)DAT_800d5c17 * 0x4b0);
      uVar6 = 0;
    }
    goto LAB_8006bf40;
  case '\b':
    FUN_80012fb8(0xac,&DAT_801a0000,0,"MIX ITP");
    FUN_80069bb4((uint)(DAT_800d5bfc >> 1) + (uint)DAT_800d5bfc,
                 &DAT_801a0000 + (uint)DAT_800d5c17 * 0x4b0);
    cVar1 = (&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4];
    (&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] = (&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] + -0xf;
    if ((uint)DAT_800d5bf8 == (uint)DAT_800d5bfd) {
      DAT_800d5bfa = (&DAT_800d4a3c)[(uint)DAT_800d5bf8 * 4];
    }
    (&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] = cVar1 + '\x0f';
    uVar2 = (&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4];
    (&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] = (&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4];
    (&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4] = uVar2;
    uVar7 = (uint)DAT_800d5bfc;
    if ((&DAT_800d4a3d)[uVar7 * 4] == '\0') {
      FUN_8006947c(uVar7,0,0,0);
      FUN_80069e54((uint)(DAT_800d5bfc >> 1) + (uint)DAT_800d5bfc);
      uVar6 = 1;
    }
    else {
      FUN_80069bb4((DAT_800d5bfc >> 1) + uVar7,&DAT_801a0000 + (uint)DAT_800d5c17 * 0x4b0);
      uVar6 = 1;
    }
LAB_8006bf40:
    DAT_800d5bf4 = '\x01';
    DAT_800d5bf6 = '\0';
    DAT_800d5bf5 = 0;
    FUN_8006a38c(uVar6);
    break;
  case '\t':
    FUN_80012fb8(0xac,&DAT_801a0000,0,"MIX ITP");
    if ((&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] == '$') {
      unaff_s0 = 0x19;
    }
    if ((&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] == '%') {
      unaff_s0 = 0x1a;
    }
    iVar4 = (uint)DAT_800d5bfc * 4;
    if ((byte)(&DAT_800d4a3d)[iVar4] < 7) {
      (&DAT_800d4a3c)[iVar4] = unaff_s0;
      FUN_80069bb4((uint)(DAT_800d5bfc >> 1) + (uint)DAT_800d5bfc,
                   &DAT_801a0000 + (uint)DAT_800d5c17 * 0x4b0);
      FUN_8006947c(DAT_800d5bfd,0,0,0);
      bVar3 = DAT_800d5bfd;
      goto LAB_8006c15c;
    }
    (&DAT_800d4a3d)[iVar4] = (&DAT_800d4a3d)[iVar4] - 6;
    (&DAT_800d4a3c)[(uint)DAT_800d5bfd * 4] = unaff_s0;
    (&DAT_800d4a3d)[(uint)DAT_800d5bfd * 4] = 6;
    bVar3 = DAT_800d5bfd;
LAB_8006c1b4:
    FUN_80069bb4((uint)(bVar3 >> 1) + (uint)bVar3,&DAT_801a0000 + (uint)DAT_800d5c17 * 0x4b0);
    goto LAB_8006c1e0;
  case '\n':
    FUN_80012fb8(0xac,&DAT_801a0000,0,"MIX ITP");
    if ((&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] == '$') {
      unaff_s0 = 0x19;
    }
    if ((&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] == '%') {
      unaff_s0 = 0x1a;
    }
    iVar4 = (uint)DAT_800d5bfd * 4;
    if (6 < (byte)(&DAT_800d4a3d)[iVar4]) {
      (&DAT_800d4a3d)[iVar4] = (&DAT_800d4a3d)[iVar4] - 6;
      (&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] = unaff_s0;
      (&DAT_800d4a3d)[(uint)DAT_800d5bfc * 4] = 6;
      bVar3 = DAT_800d5bfc;
      goto LAB_8006c1b4;
    }
    (&DAT_800d4a3c)[iVar4] = unaff_s0;
    FUN_80069bb4((uint)(DAT_800d5bfd >> 1) + (uint)DAT_800d5bfd,
                 &DAT_801a0000 + (uint)DAT_800d5c17 * 0x4b0);
    FUN_8006947c(DAT_800d5bfc,0,0,0);
    bVar3 = DAT_800d5bfc;
LAB_8006c15c:
    FUN_80069e54((uint)(bVar3 >> 1) + (uint)bVar3);
LAB_8006c1e0:
    FUN_8006a38c(0);
    DAT_800d5bf4 = '\x01';
    DAT_800d5bf6 = '\0';
    DAT_800d5bf5 = 0;
    break;
  case '\v':
    if (DAT_800d5bf5 == 0) {
      FUN_8002fe38(0xaf0010,0xe400,0xb,0);
      DAT_800d5bf5 = DAT_800d5bf5 + 1;
    }
    else if ((DAT_800e873c & 0x80) == 0) {
      if ((DAT_800e873c & 1) == 0) {
        DAT_800d5bf4 = '\x04';
      }
      else {
        DAT_800d5bf6 = '\0';
        DAT_800d5bf5 = 0;
        DAT_800d5bf4 = '\0';
      }
    }
LAB_8006c278:
    FUN_80073674((&DAT_800d4a3e)[(uint)DAT_800d5bfd * 4]);
  }
  return;
}


