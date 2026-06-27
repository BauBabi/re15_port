/* FUN_8001bd38 @ 0x8001bd38  (Ghidra headless, raw MIPS overlay) */

void FUN_8001bd38(int *param_1,int param_2)

{
  u_short uVar1;
  int iVar2;
  TIM_IMAGE *pTVar3;
  char *pcVar4;
  uint uVar5;
  ushort *puVar6;
  uint uVar7;
  int iVar8;
  byte *pbVar9;
  int y;
  int iVar10;
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
  iVar10 = 0xf;
  iVar8 = 0;
  y = 0x1e0;
  uVar7 = 0;
  pbVar9 = &DAT_800eae50;
  do {
    if (*pbVar9 == 0xff) {
      return;
    }
    OpenTIM((u_long *)local_40[uVar7]);
    ReadTIM(&TStack_58);
    iVar2 = iVar8 << 0x10;
    if (0x100 < (uint)(iVar8 + (TStack_58.prect)->h)) {
      iVar10 = iVar10 + -1;
      iVar8 = 0;
      iVar2 = 0;
    }
    *(int *)TStack_58.prect = iVar2 + iVar10 * 0x40;
    LoadImage(TStack_58.prect,TStack_58.paddr);
    DrawSync(0);
    if (TStack_58.caddr != (u_long *)0x0) {
      *(int *)TStack_58.crect = y * 0x10000 + 0x120;
      LoadImage(TStack_58.crect,TStack_58.caddr);
      DrawSync(0);
    }
    puVar6 = *(ushort **)(&DAT_800d4cd8 + (uint)*pbVar9 * 4);
    uVar1 = GetClut(0x120,y);
    puVar6[2] = uVar1;
    uVar1 = GetTPage(0,0,(int)(TStack_58.prect)->x,(int)(TStack_58.prect)->y);
    uVar5 = (uint)puVar6[1];
    *(char *)(puVar6 + 3) = (char)uVar1;
    pcVar4 = (char *)((int)puVar6 + (uint)*puVar6 * 8 + 9);
    do {
      uVar5 = uVar5 - 1;
      *pcVar4 = *pcVar4 + (char)iVar8;
      pcVar4 = pcVar4 + 4;
    } while (uVar5 != 0);
    pbVar9 = pbVar9 + 1;
    uVar7 = uVar7 + 1;
    iVar8 = iVar8 + (TStack_58.prect)->h;
    y = y + (TStack_58.crect)->h;
  } while (uVar7 < 8);
  return;
}


