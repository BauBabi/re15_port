void FUN_80019354(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  iVar2 = 0x60;
  iVar1 = 0x30fc;
  do {
    iVar2 = iVar2 + -1;
    (&DAT_800a7424)[iVar1] = 0;
    iVar1 = iVar1 + -0x84;
  } while (iVar2 != 0);
  uVar3 = 8;
  do {
    iVar1 = (uint)(byte)(&DAT_800bbdf0)[uVar3] * 4;
    if ((byte)(&DAT_800bbdf0)[uVar3] == 0xff) {
      uVar3 = 0x10;
    }
    else {
      *(undefined4 *)(&DAT_800b2248 + iVar1) = 0xffffffff;
      *(undefined4 *)(&DAT_800b22d4 + iVar1) = 0xffffffff;
      (&DAT_800bbdf0)[uVar3] = 0xff;
      uVar3 = uVar3 + 1;
    }
  } while (uVar3 < 0x10);
  piVar4 = *(int **)(DAT_800ac778 + 0x4c);
  if ((piVar4 != (int *)0x0) && (*piVar4 != -1)) {
    FUN_8001945c(piVar4,*(undefined4 *)(DAT_800ac778 + 0x50),piVar4,8);
    FUN_800194f8(*(undefined4 *)(DAT_800ac778 + 0x58),*(undefined4 *)(DAT_800ac778 + 0x54));
  }
  return;
}
