DECDCTENV * DecDCTGetEnv(DECDCTENV *env)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  DECDCTENV *pDVar4;
  u_char *puVar5;
  short *psVar6;
  
  puVar3 = &DAT_8007e474;
  iVar2 = 0xf;
  pDVar4 = env;
  do {
    uVar1 = *puVar3;
    puVar3 = puVar3 + 1;
    iVar2 = iVar2 + -1;
    *(undefined4 *)pDVar4->iq_y = uVar1;
    pDVar4 = (DECDCTENV *)(pDVar4->iq_y + 4);
  } while (iVar2 != -1);
  puVar5 = env->iq_c;
  puVar3 = &DAT_8007e4b4;
  iVar2 = 0xf;
  do {
    uVar1 = *puVar3;
    puVar3 = puVar3 + 1;
    iVar2 = iVar2 + -1;
    *(undefined4 *)puVar5 = uVar1;
    puVar5 = puVar5 + 4;
  } while (iVar2 != -1);
  psVar6 = env->dct;
  puVar3 = &DAT_8007e4f8;
  iVar2 = 0x1f;
  do {
    uVar1 = *puVar3;
    puVar3 = puVar3 + 1;
    iVar2 = iVar2 + -1;
    *(undefined4 *)psVar6 = uVar1;
    psVar6 = psVar6 + 2;
  } while (iVar2 != -1);
  return env;
}
