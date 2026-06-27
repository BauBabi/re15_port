void FUN_80020b04(void)

{
  DR_MOVE *p;
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  
  iVar5 = 0;
  puVar4 = &DAT_800b8284;
  iVar3 = 0;
  do {
    iVar2 = 0;
    iVar1 = 0;
    do {
      p = (DR_MOVE *)(&DAT_800b5584 + iVar3 + iVar1);
      SetDrawMove(p);
      iVar1 = iVar1 + 0x18;
      iVar2 = iVar2 + 1;
      p->h = 1;
      p->x0 = 0;
    } while (iVar2 < 0xf0);
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
    iVar5 = iVar5 + 1;
    iVar3 = iVar3 + 0x1680;
  } while (iVar5 < 2);
  return;
}
