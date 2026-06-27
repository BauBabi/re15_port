/* trapIntr @ 0x800862f8  (Ghidra headless, raw MIPS overlay) */

void trapIntr(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined4 *puVar5;
  
  if (DAT_800abc44 == 0) {
    printf("unexpected interrupt(%04x)\n",(uint)*(ushort *)PTR_I_STAT_800accd0);
    ReturnFromException();
  }
  DAT_800abc46 = 1;
  uVar2 = (uint)(*(ushort *)PTR_I_MASK_800accd4 & DAT_800abc74 & *(ushort *)PTR_I_STAT_800accd0);
  if (uVar2 != 0) {
    do {
      puVar5 = &DAT_800abc48;
      for (uVar4 = 0; (uVar2 != 0 && ((int)uVar4 < 0xb)); uVar4 = uVar4 + 1) {
        if ((uVar2 & 1) != 0) {
          *(ushort *)PTR_I_STAT_800accd0 = ~(ushort)(1 << (uVar4 & 0x1f));
          if ((code *)*puVar5 != (code *)0x0) {
            (*(code *)*puVar5)();
          }
        }
        puVar5 = puVar5 + 1;
        uVar2 = uVar2 >> 1;
      }
      uVar2 = (uint)(*(ushort *)PTR_I_MASK_800accd4 & DAT_800abc74 & *(ushort *)PTR_I_STAT_800accd0)
      ;
    } while (uVar2 != 0);
  }
  if ((*(ushort *)PTR_I_MASK_800accd4 & *(ushort *)PTR_I_STAT_800accd0) == 0) {
    DAT_800accdc = 0;
  }
  else {
    iVar3 = DAT_800accdc + 1;
    bVar1 = 0x800 < DAT_800accdc;
    DAT_800accdc = iVar3;
    if (bVar1) {
      printf("intr timeout(%04x:%04x)\n",(uint)*(ushort *)PTR_I_STAT_800accd0,
             (uint)*(ushort *)PTR_I_MASK_800accd4);
      DAT_800accdc = 0;
      *(undefined2 *)PTR_I_STAT_800accd0 = 0;
      INTR_OBJ_428();
      return;
    }
  }
  DAT_800abc46 = 0;
  ReturnFromException();
  return;
}


