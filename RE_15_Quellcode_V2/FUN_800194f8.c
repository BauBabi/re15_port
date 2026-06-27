void FUN_800194f8(int *param_1,int param_2)

{
  byte bVar1;
  u_short uVar2;
  TIM_IMAGE *pTVar3;
  char *pcVar4;
  uint uVar5;
  ushort *puVar6;
  uint uVar7;
  ushort uVar8;
  ushort uVar9;
  short sVar10;
  TIM_IMAGE TStack_58;
  RECT *local_40 [8];
  
  uVar7 = 0;
  pTVar3 = &TStack_58;
  do {
    param_1 = param_1 + -1;
    uVar7 = uVar7 + 1;
    pTVar3[1].crect = (RECT *)(param_2 + *param_1);
    pTVar3 = (TIM_IMAGE *)&pTVar3->crect;
  } while (uVar7 < 8);
  sVar10 = 0xf;
  uVar8 = 0;
  uVar9 = 0x1e0;
  uVar7 = 0;
  do {
    bVar1 = (&DAT_800bbdf8)[uVar7];
    if (bVar1 == 0xff) {
      return;
    }
    OpenTIM((u_long *)local_40[uVar7]);
    pTVar3 = ReadTIM(&TStack_58);
    if (0x100 < (int)((uint)uVar8 + (int)pTVar3->prect->h)) {
      sVar10 = sVar10 + -1;
      uVar8 = 0;
    }
    pTVar3->prect->x = sVar10 << 6;
    pTVar3->prect->y = uVar8;
    LoadImage(pTVar3->prect,pTVar3->paddr);
    DrawSync(0);
    if (pTVar3->caddr != (u_long *)0x0) {
      pTVar3->crect->x = 0x120;
      pTVar3->crect->y = uVar9;
      LoadImage(pTVar3->crect,pTVar3->caddr);
      DrawSync(0);
    }
    puVar6 = *(ushort **)(&DAT_800b2248 + (uint)bVar1 * 4);
    uVar2 = GetClut(0x120,(uint)uVar9);
    puVar6[2] = uVar2;
    uVar2 = GetTPage(0,0,(int)pTVar3->prect->x,(int)pTVar3->prect->y);
    uVar5 = 0;
    *(char *)(puVar6 + 3) = (char)uVar2;
    pcVar4 = (char *)((int)puVar6 + (uint)*puVar6 * 8 + 9);
    do {
      uVar5 = uVar5 + 1;
      *pcVar4 = *pcVar4 + (char)uVar8;
      pcVar4 = pcVar4 + 4;
    } while (uVar5 < puVar6[1]);
    uVar7 = uVar7 + 1;
    uVar8 = uVar8 + pTVar3->prect->h;
    uVar9 = uVar9 + pTVar3->crect->h;
  } while (uVar7 < 8);
  return;
}
