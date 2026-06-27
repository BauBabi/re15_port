/* FUN_80052d14 @ 0x80052d14  (Ghidra headless, raw MIPS overlay) */

void FUN_80052d14(void)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  
  puVar3 = &DAT_800d0324;
  iVar4 = 0x20;
  DAT_800ce548 = 0;
  DAT_800ce54c = 0;
  do {
    *puVar3 = 0;
    *(undefined1 *)((int)puVar3 + 0x145) = 0;
    iVar4 = iVar4 + -1;
    puVar3 = puVar3 + 0x7e;
  } while (iVar4 != 0);
  uVar5 = (uint)*(byte *)(DAT_800ce324 + 2);
  piVar6 = *(int **)(DAT_800ce324 + 0x30);
  puVar3 = &DAT_800d0324;
  if (uVar5 != 0) {
    puVar2 = &DAT_800d0429;
    do {
      uVar1 = DAT_800cfbf0._1_1_;
      iVar4 = *piVar6;
      puVar2[-1] = (undefined1)DAT_800cfbf0;
      *puVar2 = uVar1;
      if (DAT_800ce548 == iVar4) {
        puVar2[-1] = puVar2[-0x1f9];
        *puVar2 = puVar2[-0x1f8];
      }
      else {
        DAT_800ce548 = iVar4;
        if (iVar4 == DAT_800ce324) {
          *puVar3 = 0x80000000;
        }
        else {
          FUN_80076a40();
        }
      }
      puVar3 = puVar3 + 0x7e;
      puVar2 = puVar2 + 0x1f8;
      uVar5 = uVar5 - 1;
      piVar6 = piVar6 + 2;
    } while (uVar5 != 0);
  }
  return;
}


