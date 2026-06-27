/* PutDispEnv @ 0x80090c6c  (Ghidra headless, raw MIPS overlay) */

DISPENV * PutDispEnv(DISPENV *env)

{
  short sVar1;
  bool bVar2;
  u_char uVar3;
  DISPENV *pDVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  
  uVar15 = 0x8000000;
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("PutDispEnv(%08x)...\n",env);
  }
  if (DAT_800b2700 - 1 < 2) {
    get_dx(env);
    pDVar4 = (DISPENV *)SYS_OBJ_E34();
    return pDVar4;
  }
  (**(code **)(PTR_PTR_800b26f8 + 0x10))
            (((ushort)(env->disp).y & 0x3ff) << 10 | (ushort)(env->disp).x & 0x3ff | 0x5000000);
  if ((((DAT_800b2774 == (env->screen).x) && (DAT_800b2776 == (env->screen).y)) &&
      (DAT_800b2778 == (env->screen).w)) && (DAT_800b277a == (env->screen).h)) goto SYS_OBJ_1090;
  uVar3 = FUN_80086b7c();
  sVar1 = (env->screen).x;
  env->pad0 = uVar3;
  iVar5 = sVar1 * 10;
  iVar7 = (int)(env->screen).y;
  iVar6 = iVar5 + 0x260;
  if (uVar3 != '\0') {
    pDVar4 = (DISPENV *)SYS_OBJ_F00();
    return pDVar4;
  }
  iVar13 = iVar7 + 0x10;
  iVar8 = (int)(env->screen).w;
  if (iVar8 != 0) {
    pDVar4 = (DISPENV *)SYS_OBJ_F24(iVar7,iVar8,iVar6 + iVar8 * 10);
    return pDVar4;
  }
  uVar12 = iVar5 + 0xc60;
  iVar5 = (int)(env->screen).h;
  uVar14 = iVar13 + iVar5;
  if (iVar5 == 0) {
    uVar14 = iVar7 + 0x100;
  }
  if (iVar6 < 500) {
    uVar9 = 500;
  }
  else {
    uVar9 = 0xcda;
    if (iVar6 < 0xcdb) {
      pDVar4 = (DISPENV *)SYS_OBJ_F58(iVar7,iVar6);
      return pDVar4;
    }
  }
  uVar10 = uVar9 + 0x50;
  if (((int)(uVar9 + 0x50) <= (int)uVar12) && (uVar10 = 0xcda, (int)uVar12 < 0xcdb)) {
    uVar10 = uVar12;
  }
  if (iVar13 < 0x10) {
    uVar12 = 0x10;
  }
  else {
    if (env->pad0 == '\0') {
      if (iVar13 < 0x101) {
        pDVar4 = (DISPENV *)SYS_OBJ_FD8(iVar13);
        return pDVar4;
      }
    }
    else if (iVar13 < 0x137) {
      pDVar4 = (DISPENV *)SYS_OBJ_FD8(iVar13);
      return pDVar4;
    }
    uVar12 = 0x100;
    if (env->pad0 != '\0') {
      pDVar4 = (DISPENV *)SYS_OBJ_FD8(0x136);
      return pDVar4;
    }
  }
  uVar11 = uVar12 + 2;
  if ((int)(uVar12 + 2) <= (int)uVar14) {
    if (env->pad0 == '\0') {
      uVar11 = uVar14;
      if ((int)uVar14 < 0x103) goto code_r0x80090f14;
    }
    else if ((int)uVar14 < 0x139) {
      pDVar4 = (DISPENV *)SYS_OBJ_1034(uVar12,uVar14);
      return pDVar4;
    }
    uVar11 = 0x102;
    if (env->pad0 != '\0') {
      pDVar4 = (DISPENV *)SYS_OBJ_1034(uVar12,0x138);
      return pDVar4;
    }
  }
code_r0x80090f14:
  (**(code **)(PTR_PTR_800b26f8 + 0x10))((uVar10 & 0xfff) << 0xc | uVar9 | 0x6000000);
  (**(code **)(PTR_PTR_800b26f8 + 0x10))((uVar11 & 0x3ff) << 10 | uVar12 | 0x7000000);
SYS_OBJ_1090:
  iVar5._0_1_ = env->isinter;
  iVar5._1_1_ = env->isrgb24;
  iVar5._2_1_ = env->pad0;
  iVar5._3_1_ = env->pad1;
  if (((DAT_800b277c != iVar5) || (DAT_800b276c != (env->disp).x)) ||
     ((DAT_800b276e != (env->disp).y ||
      ((DAT_800b2770 != (env->disp).w || (DAT_800b2772 != (env->disp).h)))))) {
    uVar3 = FUN_80086b7c();
    env->pad0 = uVar3;
    if (uVar3 == '\x01') {
      uVar15 = 0x8000008;
    }
    if (env->isrgb24 != '\0') {
      uVar15 = uVar15 | 0x10;
    }
    if (env->isinter != '\0') {
      uVar15 = uVar15 | 0x20;
    }
    if (DAT_800b2703 != '\0') {
      uVar15 = uVar15 | 0x80;
    }
    sVar1 = (env->disp).w;
    if (0x118 < sVar1) {
      if (sVar1 < 0x161) {
        pDVar4 = (DISPENV *)SYS_OBJ_11C0();
        return pDVar4;
      }
      if (sVar1 < 0x191) {
        pDVar4 = (DISPENV *)SYS_OBJ_11C0();
        return pDVar4;
      }
      if (sVar1 < 0x231) {
        pDVar4 = (DISPENV *)SYS_OBJ_11C0();
        return pDVar4;
      }
      uVar15 = uVar15 | 3;
    }
    sVar1 = (env->disp).h;
    bVar2 = sVar1 < 0x121;
    if (env->pad0 == '\0') {
      bVar2 = sVar1 < 0x101;
    }
    if (!bVar2) {
      uVar15 = uVar15 | 0x24;
    }
    (**(code **)(PTR_PTR_800b26f8 + 0x10))(uVar15);
  }
  memcpy("",(uchar *)env,0x14);
  return env;
}


