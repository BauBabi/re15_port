void FUN_8002c18c(void)

{
  byte bVar1;
  bool bVar2;
  int iVar3;
  MATRIX *m1;
  undefined *puVar4;
  uint uVar5;
  int iVar6;
  MATRIX MStack_50;
  
  bVar1 = *(byte *)(DAT_800ac778 + 2);
  if (((DAT_800aca38 & 0x4000000) == 0) && (uVar5 = 0, bVar1 != 0)) {
    iVar6 = 0;
    do {
      uVar5 = uVar5 + 1;
      if ((*(uint *)((int)&DAT_800b3f98 + iVar6) & 1) != 0) {
        RotMatrix((SVECTOR *)(&DAT_800b4000 + iVar6),(MATRIX *)(&DAT_800b3fb8 + iVar6));
        m1 = (MATRIX *)(&DAT_800b3fe0 + iVar6);
        FUN_80022da0(*(undefined4 *)((int)&DAT_800b400c + iVar6),(MATRIX *)(&DAT_800b3fb8 + iVar6),
                     m1);
        if ((&DAT_800b3fa0)[iVar6] == '\x04') {
          *(int *)((int)&DAT_800b3ff8 + iVar6) = *(int *)((int)&DAT_800b3ff8 + iVar6) + -900;
        }
        FUN_80053fc0(&DAT_800b3ff4 + iVar6);
        SetColorMatrix((MATRIX *)&DAT_80076d34);
        MulMatrix0((MATRIX *)&DAT_80076d14,m1,&MStack_50);
        SetLightMatrix(&MStack_50);
        FUN_80022da0(&DAT_800b5288,m1,&MStack_50);
        SetRotMatrix(&MStack_50);
        SetTransMatrix(&MStack_50);
        bVar2 = (*(uint *)((int)&DAT_800b3fa4 + iVar6) & 2) != 0;
        if ((&DAT_800b3fa0)[iVar6] == '\x03') {
          FUN_8003e64c((uint *)((int)&DAT_800b3f98 + iVar6),bVar2);
        }
        else {
          iVar3 = FUN_80014368(&DAT_800b3ff4 + iVar6,DAT_800ac790);
          if (iVar3 != 0) {
            puVar4 = &DAT_800b4008 + iVar6;
            if ((*(uint *)((int)&DAT_800b3fa4 + iVar6) & 0x10) == 0) {
              FUN_800254a0(*(undefined4 *)((int)&DAT_800b3fa8 + iVar6),
                           (uint)DAT_800aca34 * 0x28 + *(int *)((int)&DAT_800b3fac + iVar6),puVar4,
                           bVar2);
              FUN_800256b0(*(undefined4 *)((int)&DAT_800b3fb0 + iVar6),
                           (uint)DAT_800aca34 * 0x34 + *(int *)((int)&DAT_800b3fb4 + iVar6),puVar4,
                           bVar2);
            }
            else {
              FUN_80022f0c(*(undefined4 *)((int)&DAT_800b3fa8 + iVar6),
                           (uint)DAT_800aca34 * 0x28 + *(int *)((int)&DAT_800b3fac + iVar6),puVar4,
                           bVar2);
              FUN_80023098(*(undefined4 *)((int)&DAT_800b3fb0 + iVar6),
                           (uint)DAT_800aca34 * 0x34 + *(int *)((int)&DAT_800b3fb4 + iVar6),puVar4,
                           bVar2);
            }
          }
        }
      }
      iVar6 = iVar6 + 0x94;
    } while (uVar5 < bVar1);
  }
  return;
}
