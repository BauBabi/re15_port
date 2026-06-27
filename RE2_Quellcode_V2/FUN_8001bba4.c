/* FUN_8001bba4 @ 0x8001bba4  (Ghidra headless, raw MIPS overlay) */

void FUN_8001bba4(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  iVar2 = 0x60;
  iVar1 = 0x2e04;
  do {
    iVar2 = iVar2 + -1;
    *(undefined2 *)((int)&DAT_800d8d08 + iVar1) = 0;
    iVar1 = iVar1 + -0x7c;
  } while (iVar2 != 0);
  uVar3 = 8;
  do {
    iVar1 = (uint)(byte)(&DAT_800eae48)[uVar3] * 4;
    if ((byte)(&DAT_800eae48)[uVar3] == 0xff) {
      uVar3 = 0x10;
    }
    else {
      *(undefined4 *)(&DAT_800d4cd8 + iVar1) = 0xffffffff;
      *(undefined4 *)(&DAT_800d4e18 + iVar1) = 0xffffffff;
      (&DAT_800eae48)[uVar3] = 0xff;
      uVar3 = uVar3 + 1;
    }
  } while (uVar3 < 0x10);
  piVar4 = *(int **)(DAT_800ce324 + 0x50);
  if ((piVar4 != (int *)0x0) && (*piVar4 != -1)) {
    FUN_8001bca0(piVar4,*(undefined4 *)(DAT_800ce324 + 0x54),piVar4,8);
    FUN_8001bd38(*(undefined4 *)(DAT_800ce324 + 0x5c),*(undefined4 *)(DAT_800ce324 + 0x58));
  }
  return;
}


