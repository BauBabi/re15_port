/* PutDrawEnv @ 0x80090a14  (Ghidra headless, raw MIPS overlay) */

DRAWENV * PutDrawEnv(DRAWENV *env)

{
  undefined *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  DRAWENV *pDVar6;
  DRAWENV *pDVar7;
  undefined4 *puVar8;
  
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("PutDrawEnv(%08x)...\n",env);
  }
  SetDrawEnv2(&env->dr_env,env);
  puVar1 = PTR_PTR_800b26f8;
  (env->dr_env).tag = (env->dr_env).tag | 0xffffff;
  (**(code **)(puVar1 + 8))(*(undefined4 *)(puVar1 + 0x18),&env->dr_env,0x40,0);
  pDVar7 = env;
  puVar2 = &DAT_800b2710;
  do {
    puVar8 = puVar2;
    pDVar6 = pDVar7;
    uVar3 = *(undefined4 *)&(pDVar6->clip).w;
    uVar4 = *(undefined4 *)pDVar6->ofs;
    uVar5 = *(undefined4 *)&pDVar6->tw;
    *puVar8 = *(undefined4 *)&pDVar6->clip;
    puVar8[1] = uVar3;
    puVar8[2] = uVar4;
    puVar8[3] = uVar5;
    pDVar7 = (DRAWENV *)&(pDVar6->tw).w;
    puVar2 = puVar8 + 4;
  } while (pDVar7 != (DRAWENV *)((env->dr_env).code + 0xc));
  uVar3 = *(undefined4 *)&pDVar6->tpage;
  uVar4 = *(undefined4 *)&pDVar6->isbg;
  puVar8[4] = *(undefined4 *)pDVar7;
  puVar8[5] = uVar3;
  puVar8[6] = uVar4;
  return env;
}


