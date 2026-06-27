/* FUN_80076a40 @ 0x80076a40  (Ghidra headless, raw MIPS overlay) */

void FUN_80076a40(u_long *param_1)

{
  short sVar1;
  TIM_IMAGE *pTVar2;
  int iVar3;
  int iVar4;
  TIM_IMAGE TStack_28;
  
  OpenTIM(param_1);
  pTVar2 = ReadTIM(&TStack_28);
  sVar1 = (ushort)(byte)DAT_800cfbf0 * 0x40;
  if (0xf < (byte)DAT_800cfbf0) {
    sVar1 = sVar1 + -0x400;
  }
  pTVar2->prect->x = sVar1;
  pTVar2->prect->y = ((byte)DAT_800cfbf0 < 0x10 ^ 1) << 8;
  LoadImage(pTVar2->prect,pTVar2->paddr);
  iVar3 = (int)pTVar2->prect->w;
  iVar4 = iVar3 + 0x3f;
  if (iVar4 < 0) {
    iVar4 = iVar3 + 0x7e;
  }
  DAT_800cfbf0._0_1_ = (byte)DAT_800cfbf0 + (char)(iVar4 >> 6);
  if (pTVar2->caddr != (u_long *)0x0) {
    pTVar2->crect->y = DAT_800cfbf0._1_1_ + 0x1e0;
    LoadImage(pTVar2->crect,pTVar2->caddr);
    DAT_800cfbf0._1_1_ = DAT_800cfbf0._1_1_ + (char)pTVar2->crect->h;
  }
  DrawSync(0);
  return;
}


