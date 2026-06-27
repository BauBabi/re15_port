void FUN_80029590(void)

{
  u_short uVar1;
  undefined *puVar2;
  DR_MODE *p;
  int iVar3;
  int iVar4;
  DR_MODE *pDVar5;
  int iVar6;
  
  iVar6 = 0;
  pDVar5 = (DR_MODE *)&DAT_800b82a8;
  iVar4 = 0;
  do {
    iVar3 = 0;
    puVar2 = &DAT_800b829c;
    p = pDVar5;
    do {
      uVar1 = GetTPage(0,0,0x300,0x100);
      SetDrawMode((DR_MODE *)(puVar2 + iVar4),0,0,(uint)uVar1,(RECT *)0x0);
      uVar1 = GetTPage(0,0,0x340,0x100);
      SetDrawMode(p,0,0,(uint)uVar1,(RECT *)0x0);
      p = p + 2;
      iVar3 = iVar3 + 1;
      puVar2 = puVar2 + 0x18;
    } while (iVar3 < 8);
    pDVar5 = pDVar5 + 0x10;
    iVar6 = iVar6 + 1;
    iVar4 = iVar4 + 0xc0;
  } while (iVar6 < 2);
  FUN_80029560();
  FUN_80029690();
  return;
}
