/* SetDrawEnv @ 0x80091304  (Ghidra headless, raw MIPS overlay) */

void SetDrawEnv(DR_ENV *dr_env,DRAWENV *env)

{
  short sVar1;
  short sVar2;
  u_long uVar3;
  int iVar4;
  char cVar5;
  undefined4 local_30;
  undefined4 local_2c;
  
  uVar3 = get_cs((int)(env->clip).x,(int)(env->clip).y);
  dr_env->code[0] = uVar3;
  uVar3 = get_ce((int)(((uint)(ushort)(env->clip).w + (uint)(ushort)(env->clip).x + -1) * 0x10000)
                 >> 0x10,(int)(((uint)(ushort)(env->clip).y + (uint)(ushort)(env->clip).h + -1) *
                              0x10000) >> 0x10);
  dr_env->code[1] = uVar3;
  uVar3 = get_ofs((int)env->ofs[0],(int)env->ofs[1]);
  dr_env->code[2] = uVar3;
  uVar3 = get_mode(env->dfe,env->dtd,env->tpage);
  dr_env->code[3] = uVar3;
  uVar3 = get_tw(&env->tw);
  dr_env->code[4] = uVar3;
  dr_env->code[5] = 0xe6000000;
  cVar5 = '\a';
  if (env->isbg != '\0') {
    sVar2 = (env->clip).w;
    sVar1 = 0;
    if ((-1 < sVar2) && (sVar1 = DAT_800b2704 + -1, (int)sVar2 <= DAT_800b2704 + -1)) {
      sVar1 = sVar2;
    }
    iVar4 = (int)(env->clip).h;
    if (iVar4 < 0) {
      sVar2 = 0;
    }
    else {
      sVar2 = DAT_800b2706 + -1;
      if (iVar4 <= DAT_800b2706 + -1) {
        SYS_OBJ_1578(iVar4);
        return;
      }
    }
    local_2c = CONCAT22(sVar2,sVar1);
    local_30 = CONCAT22((env->clip).y - env->ofs[1],(env->clip).x - env->ofs[0]);
    dr_env->code[6] = (uint)env->b0 << 0x10 | (uint)env->g0 << 8 | 0x60000000 | (uint)env->r0;
    dr_env->code[7] = local_30;
    dr_env->code[8] = local_2c;
    cVar5 = '\n';
  }
  *(char *)((int)&dr_env->tag + 3) = cVar5 + -1;
  return;
}


