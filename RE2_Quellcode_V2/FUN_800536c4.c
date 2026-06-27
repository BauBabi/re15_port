/* FUN_800536c4 @ 0x800536c4  (Ghidra headless, raw MIPS overlay) */

void FUN_800536c4(void)

{
  int iVar1;
  undefined *puVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = 0;
  iVar3 = 0;
  do {
    puVar2 = &DAT_800d7860 + iVar3;
    if ((&DAT_800d7861)[iVar3] != '\0') {
      while( true ) {
        do {
          iVar1 = (**(code **)((uint)**(byte **)((int)&DAT_800d787c + iVar3) * 4 + 0x1f800000))
                            (puVar2);
        } while (iVar1 == 1);
        if ((iVar1 == 2) || ((char)puVar2[(char)(&DAT_800d7862)[iVar3] + 4] < '\0')) break;
        iVar1 = *(int *)((int)&DAT_800d79a0 + iVar3);
        *(int *)((int)&DAT_800d79a0 + iVar3) = iVar1 + -4;
        *(undefined4 *)((int)&DAT_800d787c + iVar3) = *(undefined4 *)(iVar1 + -4);
        puVar2[(char)(&DAT_800d7862)[iVar3] + 4] = puVar2[(char)(&DAT_800d7862)[iVar3] + 4] + -1;
      }
    }
    uVar4 = uVar4 + 1;
    iVar3 = iVar3 + 0x174;
  } while (uVar4 < 10);
  FUN_80052e20();
  FUN_80052e7c();
  return;
}


