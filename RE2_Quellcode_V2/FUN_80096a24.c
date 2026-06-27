/* FUN_80096a24 @ 0x80096a24  (Ghidra headless, raw MIPS overlay) */

uint FUN_80096a24(int param_1,undefined1 param_2)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  undefined2 uVar4;
  
  uVar4 = 0x88;
  if (((int)(uint)**(byte **)(param_1 + 0x3c) >> 4 == 8) && (8 < *(byte *)(param_1 + 0x44))) {
    uVar4 = 0x22;
  }
  do {
  } while ((*(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 4) & 2) == 0);
  FUN_80098754(400);
  bVar1 = *PTR_JOY_MCD_DATA_800c3a00;
  if ((*(char *)(param_1 + 0x44) == '\0') && ((int)(uint)bVar1 >> 4 == 8)) {
    *(undefined2 *)(PTR_JOY_MCD_DATA_800c3a00 + 0xe) = 0x22;
  }
  else {
    *(undefined2 *)(PTR_JOY_MCD_DATA_800c3a00 + 0xe) = uVar4;
  }
  uVar2 = *(uint *)PTR_I_STAT_800c39fc;
  while( true ) {
    if ((uVar2 & 0x80) != 0) {
      if ((*(char *)(param_1 + 0xe8) != '\b') && (DAT_800c39e0 == 2)) {
        FUN_80098754(0x3c);
        do {
          iVar3 = FUN_80098774();
        } while (iVar3 == 0);
      }
      *PTR_JOY_MCD_DATA_800c3a00 = param_2;
      *(char *)(param_1 + 0x45) = *(char *)(param_1 + 0x45) + '\x01';
      if (*(char *)(param_1 + 0x44) != -1) {
        *(byte *)(*(int *)(param_1 + 0x3c) + (uint)*(byte *)(param_1 + 0x44)) = bVar1;
      }
      *(char *)(param_1 + 0x44) = *(char *)(param_1 + 0x44) + '\x01';
      return (uint)bVar1;
    }
    uVar2 = (uint)(ushort)TMR_SYSCLOCK_VAL;
    if (uVar2 < DAT_800cbc2c) {
      if ((ushort)TMR_SYSCLOCK_MAX == 0) {
        uVar2 = uVar2 + 0x10000;
      }
      else {
        uVar2 = (ushort)TMR_SYSCLOCK_MAX + uVar2;
      }
    }
    if ((((ushort)TMR_SYSCLOCK_MODE & 0x200) != 0) && (DAT_800e8990 <= uVar2 - DAT_800cbc2c)) break;
    if (DAT_800e8990 <= uVar2 - DAT_800cbc2c >> 3) {
      return 0xfffffffe;
    }
    uVar2 = *(uint *)PTR_I_STAT_800c39fc;
  }
  return 0xfffffffe;
}


