void FUN_80039590(void)

{
  byte bVar1;
  DR_MODE *p;
  uint uVar2;
  byte *pbVar3;
  
  bVar1 = *DAT_800ac778;
  uVar2 = 0;
  pbVar3 = DAT_800b2584;
  if (bVar1 != 0) {
    do {
      if ((*pbVar3 & 1) != 0) {
        p = (DR_MODE *)(uVar2 * 0x20 + (&DAT_800bb4d4)[DAT_800aca34]);
        SetSprt((SPRT *)(p + 1));
        SetDrawMode(p,1,1,0x95,(RECT *)0x0);
        MargePrim(p,(SPRT *)(p + 1));
        AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000 + *(short *)(pbVar3 + 2) * 4,p);
      }
      uVar2 = uVar2 + 1;
      pbVar3 = pbVar3 + 4;
    } while (uVar2 < bVar1);
  }
  return;
}
