void FUN_80039ca0(void)

{
  int iVar1;
  MATRIX *m1;
  undefined4 *puVar2;
  uint *puVar3;
  undefined *puVar4;
  uint uVar5;
  MATRIX MStack_40;
  
  uVar5 = (uint)DAT_800afbb7;
  puVar3 = &DAT_800af33c;
  if (uVar5 != 0) {
    puVar2 = &DAT_800af348;
    puVar4 = &DAT_800af3a0;
    do {
      RotMatrix((SVECTOR *)(puVar3 + 0x17),(MATRIX *)(puVar3 + 5));
      m1 = (MATRIX *)(puVar3 + 0xf);
      FUN_80022da0(puVar2[0x17],(MATRIX *)(puVar3 + 5),m1);
      FUN_80053fc0(puVar3 + 0x14);
      SetColorMatrix((MATRIX *)&DAT_80076d34);
      MulMatrix0((MATRIX *)&DAT_80076d14,m1,&MStack_40);
      SetLightMatrix(&MStack_40);
      FUN_80022da0(&DAT_800b5288,m1,&MStack_40);
      SetRotMatrix(&MStack_40);
      SetTransMatrix(&MStack_40);
      iVar1 = FUN_80014368(puVar3 + 0x14,DAT_800ac790);
      if (iVar1 != 0) {
        if ((*puVar3 & 0x10) == 0) {
          FUN_800254a0(puVar2[-2],(uint)DAT_800aca34 * 0x28 + puVar2[-1],puVar4,0);
          FUN_800256b0(*puVar2,(uint)DAT_800aca34 * 0x34 + puVar2[1],puVar4,0);
        }
        else {
          FUN_80022f0c(puVar2[-2],(uint)DAT_800aca34 * 0x28 + puVar2[-1],puVar4,0);
          FUN_80023098(*puVar2,(uint)DAT_800aca34 * 0x34 + puVar2[1],puVar4,0);
        }
      }
      uVar5 = uVar5 - 1;
      puVar2 = puVar2 + 0x1b;
      puVar4 = puVar4 + 0x6c;
      puVar3 = puVar3 + 0x1b;
    } while (uVar5 != 0);
  }
  return;
}
