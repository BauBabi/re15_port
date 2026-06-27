/* FUN_80049ca8 @ 0x80049ca8  (Ghidra headless, raw MIPS overlay) */

void FUN_80049ca8(void)

{
  int iVar1;
  int iVar2;
  DR_MODE *p;
  ushort *puVar3;
  uint uVar4;
  byte *pbVar5;
  
  FUN_8008de04(0xa0,0x78);
  uVar4 = (uint)*DAT_800ce324;
  iVar1 = (int)DAT_800d4820;
  pbVar5 = (byte *)(DAT_800eaad0 + uVar4 * 4);
  if (uVar4 != 0) {
    puVar3 = (ushort *)(pbVar5 + 2);
    do {
      uVar4 = uVar4 - 1;
      pbVar5 = pbVar5 + -4;
      if (((*pbVar5 & 1) != 0) &&
         (iVar2 = FUN_80077360(&DAT_800d4928 + iVar1 * 4,*(byte *)((int)puVar3 + -5)), iVar2 != 0))
      {
        p = (DR_MODE *)((&DAT_800ea240)[DAT_800ce5e0] + uVar4 * 0x20);
        SetSprt((SPRT *)(p + 1));
        SetDrawMode(p,1,0,0x95,(RECT *)0x0);
        MargePrim(p,(SPRT *)(p + 1));
        AddPrim((void *)(DAT_800ce22c + (uint)puVar3[-2] * 4),p);
      }
      puVar3 = puVar3 + -2;
    } while (uVar4 != 0);
  }
  return;
}


