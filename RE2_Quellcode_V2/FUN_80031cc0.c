/* FUN_80031cc0 @ 0x80031cc0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80031cc0(void)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = 0;
  iVar4 = 0xc0;
  iVar3 = 0;
  do {
    *(undefined2 *)((int)&DAT_800d76a4 + iVar3) = 0;
    iVar5 = iVar5 + 1;
    iVar1 = _DAT_00000110 + iVar4;
    iVar4 = iVar4 + 0xc0;
    *(int *)((int)&DAT_800d76b0 + iVar3) = iVar1;
    *(undefined4 *)(iVar1 + 0x94) = 0x404;
    uVar2 = FUN_80095844();
    *(undefined4 *)((int)&DAT_800d76e8 + iVar3) = uVar2;
    iVar3 = iVar3 + 0x80;
  } while (iVar5 < 3);
  DAT_800d76b4 = &DAT_801ffb00;
  DAT_800d7734 = &DAT_801ff400;
  DAT_800d77b4 = &DAT_801fed00;
  return;
}


