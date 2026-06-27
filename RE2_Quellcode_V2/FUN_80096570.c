/* FUN_80096570 @ 0x80096570  (Ghidra headless, raw MIPS overlay) */

undefined8 FUN_80096570(int param_1)

{
  code *pcVar1;
  undefined *puVar2;
  int iVar3;
  undefined2 uVar4;
  undefined *puVar5;
  undefined4 uVar6;
  
  puVar5 = PTR_JOY_MCD_DATA_800c3a00;
  *(undefined2 *)(PTR_JOY_MCD_DATA_800c3a00 + 10) = 0x40;
  *(undefined2 *)(puVar5 + 10) = 0;
  *(undefined2 *)(puVar5 + 8) = 0xd;
  *(undefined2 *)(puVar5 + 0xe) = 0x88;
  uVar6 = 0x91;
  if (*(char *)(param_1 + 0xe8) == '\b') {
    uVar6 = 0x50;
  }
  FUN_80098754(uVar6);
  uVar4 = 0x1003;
  if (DAT_800c39dc != 0) {
    uVar4 = 0x3003;
  }
  iVar3 = DAT_800c39dc * 4;
  *(undefined2 *)(PTR_JOY_MCD_DATA_800c3a00 + 10) = uVar4;
  iVar3 = *(int *)(&DAT_800c39f4 + iVar3);
  pcVar1 = DAT_800c39bc;
  if (-1 < iVar3) {
    while (DAT_800c39bc = pcVar1, 0 < iVar3) {
      iVar3 = *(int *)(&DAT_800c39f4 + DAT_800c39dc * 4) + -1;
      *(int *)(&DAT_800c39f4 + DAT_800c39dc * 4) = iVar3;
      (*DAT_800c39bc)(*(int *)(param_1 + 0xc) + iVar3 * 0xf0);
      pcVar1 = DAT_800c39bc;
      iVar3 = *(int *)(&DAT_800c39f4 + DAT_800c39dc * 4);
    }
    if (*(int *)(&DAT_800c39f4 + DAT_800c39dc * 4) == 0) {
      *(int *)(&DAT_800c39f4 + DAT_800c39dc * 4) = -1;
      (*pcVar1)(param_1);
      (*DAT_800c39c0)(param_1);
    }
  }
  puVar2 = PTR_JOY_MCD_DATA_800c3a00;
  puVar5 = PTR_JOY_MCD_DATA_800c3a00;
  if ((*(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 4) & 0x200) != 0) {
    *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 10) = *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 10) | 0x10
    ;
    puVar5 = PTR_I_STAT_800c39fc;
    if ((*(ushort *)(puVar2 + 4) & 0x200) != 0) {
      *puVar2 = 1;
      FUN_80096ce8();
      uVar6 = 0;
      puVar5 = (undefined *)(uint)(byte)*PTR_JOY_MCD_DATA_800c3a00;
      goto LAB_80096770;
    }
    *(undefined4 *)PTR_I_STAT_800c39fc = 0xffffff7f;
  }
  if ((*(char *)(param_1 + 0x50) == '\0') || (uVar6 = 0, *(char *)(param_1 + 0x36) == '\0')) {
    puVar5 = *(undefined **)(param_1 + 0x3c);
    uVar6 = 1;
    *puVar5 = 0;
  }
LAB_80096770:
  return CONCAT44(puVar5,uVar6);
}


