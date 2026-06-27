void FUN_80036b68(void)

{
  int iVar1;
  TIM_IMAGE *pTVar2;
  undefined4 uVar3;
  TIM_IMAGE TStack_30;
  
  FUN_80013b60((uint)*(ushort *)(&DAT_800741e8 + (uint)DAT_800aca5c * 2) + (uint)DAT_800aca5d,
               &DAT_801d7700,0);
  iVar1 = DAT_801d7700;
  DAT_800acbc8 = (int)&DAT_801d7700 + *(int *)((int)&DAT_801d7700 + DAT_801d7700);
  DAT_800acbc4 = (int)&DAT_801d7700 + *(int *)(&DAT_801d7704 + DAT_801d7700);
  if (DAT_800aca5d != 0) {
    OpenTIM((u_long *)((int)&DAT_801d7700 + *(int *)(&DAT_801d770c + DAT_801d7700)));
    pTVar2 = ReadTIM(&TStack_30);
    pTVar2->prect->x = (ushort)DAT_800acad4 * 0x40 + -0x39c;
    pTVar2->prect->y = 0x1e0;
    pTVar2->prect->w = 0x1c;
    pTVar2->prect->h = 0x20;
    LoadImage(pTVar2->prect,pTVar2->paddr);
    DrawSync(0);
    pTVar2->crect->x = 0xe0;
    pTVar2->crect->y = DAT_800acad5 + 0x1e1;
    pTVar2->crect->w = 0x10;
    LoadImage(pTVar2->crect,pTVar2->caddr);
    DrawSync(0);
    FUN_80022150(2,(int)&DAT_801d7700 + *(int *)(&DAT_801d7708 + iVar1),DAT_800acad4,DAT_800acad5);
    puRam0000076c = &DAT_801d770c + *(int *)(&DAT_801d7708 + iVar1);
    puRam00000774 = &UNK_801d7728 + *(int *)(&DAT_801d7708 + iVar1);
    uVar3 = FUN_80025940(0x768,&DAT_801d5d00);
    FUN_80025a98(0x768,uVar3);
    if (DAT_800aca5d == 1) {
      DAT_800bb4b4 = puRam0000076c;
      DAT_800bbe04 = puRam00000774;
      DAT_800b8988 = uRam00000770;
      DAT_800bbd90 = uRam00000778;
      puRam0000076c = DAT_800b2b2c;
      puRam00000774 = DAT_800b521c;
      uRam00000770 = DAT_800b2848;
      uRam00000778 = DAT_800b2b48;
    }
  }
  return;
}
