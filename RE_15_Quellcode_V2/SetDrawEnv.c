void SetDrawEnv(DR_ENV *dr_env,DRAWENV *env)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  short sVar4;
  u_long uVar5;
  char cVar6;
  undefined4 local_18;
  undefined4 local_14;
  
  uVar5 = get_cs((int)(env->clip).x,(int)(env->clip).y);
  dr_env->code[0] = uVar5;
  uVar5 = get_ce((int)(((uint)(ushort)(env->clip).w + (uint)(ushort)(env->clip).x + -1) * 0x10000)
                 >> 0x10,(int)(((uint)(ushort)(env->clip).y + (uint)(ushort)(env->clip).h + -1) *
                              0x10000) >> 0x10);
  dr_env->code[1] = uVar5;
  uVar5 = get_ofs((int)env->ofs[0],(int)env->ofs[1]);
  dr_env->code[2] = uVar5;
  uVar5 = get_mode(env->dfe,env->dtd,env->tpage);
  dr_env->code[3] = uVar5;
  uVar5 = get_tw(&env->tw);
  dr_env->code[4] = uVar5;
  dr_env->code[5] = 0xe6000000;
  cVar6 = '\a';
  if (env->isbg != '\0') {
    uVar1 = (env->clip).x;
    uVar5 = *(u_long *)&env->clip;
    uVar2 = (env->clip).w;
    sVar4 = (env->clip).h;
    uVar3 = 0;
    if ((-1 < (short)uVar2) && (uVar3 = uVar2, DAT_8007e354 + -1 < (int)(short)uVar2)) {
      uVar3 = (ushort)(DAT_8007e354 + -1);
    }
    if (sVar4 < 0) {
      sVar4 = 0;
    }
    else if (DAT_8007e356 + -1 < (int)sVar4) {
      SYS_OBJ_1454();
      return;
    }
    local_14 = CONCAT22(sVar4,uVar3);
    if (((uVar1 & 0x3f) != 0) || ((uVar3 & 0x3f) != 0)) {
      local_18 = CONCAT22((env->clip).y - env->ofs[1],uVar1 - env->ofs[0]);
      dr_env->code[6] = (uint)env->b0 << 0x10 | (uint)env->g0 << 8 | 0x60000000 | (uint)env->r0;
      dr_env->code[7] = local_18;
      dr_env->code[8] = local_14;
      SYS_OBJ_1570();
      return;
    }
    cVar6 = '\n';
    dr_env->code[6] = (uint)env->b0 << 0x10 | (uint)env->g0 << 8 | 0x2000000 | (uint)env->r0;
    dr_env->code[7] = uVar5;
    dr_env->code[8] = local_14;
  }
  *(char *)((int)&dr_env->tag + 3) = cVar6 + -1;
  return;
}
