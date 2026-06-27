/* FUN_8003a5a4 @ 0x8003a5a4  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a5a4(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar1 = DAT_800ea23c;
  uVar2 = 0;
  iVar6 = 0x18a;
  iVar5 = 0x16c;
  iVar4 = 0x22;
  iVar3 = 4;
  do {
    FUN_80076a00(iVar1 + iVar3,iVar1 + iVar4,0x1e);
    FUN_80076a00(iVar1 + iVar5,iVar1 + iVar6,0x1e);
    iVar6 = iVar6 + 0x1e;
    iVar5 = iVar5 + 0x1e;
    iVar4 = iVar4 + 0x1e;
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x1e;
  } while (uVar2 < 0xb);
  uVar2 = 0;
  iVar3 = iVar1;
  do {
    uVar2 = uVar2 + 1;
                    /* Possible PsyQ macro: setPolyF3() */
    *(undefined1 *)(iVar3 + 0x14e) = 0x20;
    *(undefined1 *)(iVar3 + 0x2b6) = 0;
    iVar3 = iVar1 + uVar2;
  } while (uVar2 < 0x1e);
  return;
}


