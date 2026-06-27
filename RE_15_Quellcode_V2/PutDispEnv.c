DISPENV * PutDispEnv(DISPENV *env)

{
  short sVar1;
  bool bVar2;
  u_char uVar3;
  DISPENV *pDVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  
  uVar14 = 0x8000000;
  if (1 < DAT_8007e352) {
    printf("PutDispEnv(%08x)...\n",env);
  }
  if (DAT_8007e350 - 1 < 2) {
    get_dx(env);
    pDVar4 = (DISPENV *)SYS_OBJ_CFC();
    return pDVar4;
  }
  _ctl(((ushort)(env->disp).y & 0x3ff) << 10 | (ushort)(env->disp).x & 0x3ff | 0x5000000);
  if (((((env->screen).x == 0) && ((env->screen).y == 0)) && ((env->screen).w == 0)) &&
     ((env->screen).h == 0)) goto SYS_OBJ_F54;
  uVar3 = FUN_800614d0();
  sVar1 = (env->screen).x;
  env->pad0 = uVar3;
  iVar5 = sVar1 * 10;
  uVar10 = iVar5 + 0x260;
  iVar8 = (int)(env->screen).y;
  iVar12 = iVar8 + 0x13;
  if (env->pad0 == '\0') {
    iVar12 = iVar8 + 0x10;
  }
  iVar6 = (int)(env->screen).w;
  if (iVar6 != 0) {
    pDVar4 = (DISPENV *)SYS_OBJ_DE8(iVar8,uVar10,uVar10 + iVar6 * 10);
    return pDVar4;
  }
  uVar11 = iVar5 + 0xc60;
  iVar5 = (int)(env->screen).h;
  uVar13 = iVar12 + iVar5;
  if (iVar5 == 0) {
    uVar13 = iVar12 + 0xf0;
  }
  uVar9 = 500;
  if ((499 < (int)uVar10) && (uVar9 = uVar10, 0xcda < (int)uVar10)) {
    uVar9 = 0xcda;
  }
  uVar10 = uVar9 + 0x50;
  if (((int)(uVar9 + 0x50) <= (int)uVar11) && (uVar10 = uVar11, 0xcda < (int)uVar11)) {
    uVar10 = 0xcda;
  }
  if (iVar12 < 0x10) {
    uVar11 = 0x10;
  }
  else {
    if (env->pad0 == '\0') {
      if (iVar12 < 0x101) {
        pDVar4 = (DISPENV *)SYS_OBJ_E9C(iVar12);
        return pDVar4;
      }
    }
    else if (iVar12 < 0x137) {
      pDVar4 = (DISPENV *)SYS_OBJ_E9C(iVar12);
      return pDVar4;
    }
    uVar11 = 0x100;
    if (env->pad0 != '\0') {
      pDVar4 = (DISPENV *)SYS_OBJ_E9C(0x136);
      return pDVar4;
    }
  }
  uVar7 = uVar11 + 2;
  if ((int)(uVar11 + 2) <= (int)uVar13) {
    if (env->pad0 == '\0') {
      uVar7 = uVar13;
      if ((int)uVar13 < 0x103) goto code_r0x800694bc;
    }
    else if ((int)uVar13 < 0x139) {
      pDVar4 = (DISPENV *)SYS_OBJ_EF8();
      return pDVar4;
    }
    uVar7 = 0x102;
    if (env->pad0 != '\0') {
      pDVar4 = (DISPENV *)SYS_OBJ_EF8();
      return pDVar4;
    }
  }
code_r0x800694bc:
  _ctl((uVar10 & 0xfff) << 0xc | uVar9 & 0xfff | 0x6000000);
  _ctl((uVar7 & 0x3ff) << 10 | uVar11 | 0x7000000);
SYS_OBJ_F54:
  iVar5._0_1_ = env->isinter;
  iVar5._1_1_ = env->isrgb24;
  iVar5._2_1_ = env->pad0;
  iVar5._3_1_ = env->pad1;
  if ((((iVar5 != 0) || ((env->disp).x != 0)) || ((env->disp).y != 0)) ||
     (((env->disp).w != 0 || ((env->disp).h != 0)))) {
    uVar3 = FUN_800614d0();
    env->pad0 = uVar3;
    if (env->pad0 == '\x01') {
      uVar14 = 0x8000008;
    }
    if (env->isrgb24 != '\0') {
      uVar14 = uVar14 | 0x10;
    }
    if (env->isinter != '\0') {
      uVar14 = uVar14 | 0x20;
    }
    if (DAT_8007e353 != '\0') {
      uVar14 = uVar14 | 0x80;
    }
    sVar1 = (env->disp).w;
    if (0x118 < sVar1) {
      if (sVar1 < 0x161) {
        pDVar4 = (DISPENV *)SYS_OBJ_1088();
        return pDVar4;
      }
      if (sVar1 < 0x191) {
        pDVar4 = (DISPENV *)SYS_OBJ_1088();
        return pDVar4;
      }
      if (sVar1 < 0x231) {
        pDVar4 = (DISPENV *)SYS_OBJ_1088();
        return pDVar4;
      }
      uVar14 = uVar14 | 3;
    }
    sVar1 = (env->disp).h;
    bVar2 = sVar1 < 0x121;
    if (env->pad0 == '\0') {
      bVar2 = sVar1 < 0x101;
    }
    if (!bVar2) {
      uVar14 = uVar14 | 0x24;
    }
    _ctl(uVar14);
  }
  memcpy("",(uchar *)env,0x14);
  return env;
}
