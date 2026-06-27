void FUN_8004ee78(u_long *param_1)

{
  TIM_IMAGE *pTVar1;
  int iVar2;
  int iVar3;
  short sVar4;
  TIM_IMAGE TStack_28;
  
  OpenTIM(param_1);
  pTVar1 = ReadTIM(&TStack_28);
  sVar4 = (ushort)DAT_800aca4c * 0x40;
  if (0xf < DAT_800aca4c) {
    sVar4 = sVar4 + -0x400;
  }
  pTVar1->prect->x = sVar4;
  pTVar1->prect->y = (DAT_800aca4c < 0x10 ^ 1) << 8;
  LoadImage(pTVar1->prect,pTVar1->paddr);
  iVar2 = (int)pTVar1->prect->w;
  iVar3 = iVar2 + 0x3f;
  if (iVar3 < 0) {
    iVar3 = iVar2 + 0x7e;
  }
  DAT_800aca4c = DAT_800aca4c + (char)(iVar3 >> 6);
  DrawSync(0);
  if (pTVar1->caddr != (u_long *)0x0) {
    pTVar1->crect->y = DAT_800aca4d + 0x1e0;
    LoadImage(pTVar1->crect,pTVar1->caddr);
    DAT_800aca4d = DAT_800aca4d + (char)pTVar1->crect->h;
    DrawSync(0);
  }
  return;
}
