/* FUN_8009684c @ 0x8009684c  (Ghidra headless, raw MIPS overlay) */

uint FUN_8009684c(int param_1,int param_2)

{
  byte bVar1;
  ushort uVar2;
  byte bVar3;
  undefined *puVar4;
  int iVar5;
  uint uVar6;
  undefined2 uVar7;
  
  puVar4 = PTR_I_STAT_800c39fc;
  bVar3 = (byte)param_2;
  if (param_2 < 0) {
    bVar1 = *PTR_JOY_MCD_DATA_800c3a00;
    *(undefined1 *)(param_1 + 0x44) = 0xff;
    *(undefined1 *)(param_1 + 0x45) = 1;
    **(byte **)(param_1 + 0x40) = ~bVar3;
    uVar2 = *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 4);
    while ((uVar2 & 1) == 0) {
      uVar2 = *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 4);
    }
    do {
      iVar5 = FUN_80098774();
    } while (iVar5 == 0);
    *PTR_JOY_MCD_DATA_800c3a00 = ~bVar3;
  }
  else {
    uVar7 = 0x88;
    if (((int)(uint)**(byte **)(param_1 + 0x3c) >> 4 == 8) &&
       (uVar7 = 0x88, 8 < *(byte *)(param_1 + 0x44))) {
      uVar7 = 0x22;
    }
    DAT_800cbc2c = (uint)(ushort)TMR_SYSCLOCK_VAL;
    uVar2 = *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 4);
    DAT_800e8990 = 0x1ae;
    while ((uVar2 & 2) == 0) {
      uVar2 = *(ushort *)(PTR_JOY_MCD_DATA_800c3a00 + 4);
    }
    bVar1 = *PTR_JOY_MCD_DATA_800c3a00;
    *(undefined2 *)(PTR_JOY_MCD_DATA_800c3a00 + 0xe) = uVar7;
    uVar6 = *(uint *)puVar4;
    while ((uVar6 & 0x80) == 0) {
      iVar5 = FUN_80098774();
      if (iVar5 != 0) {
        return 0xffffffec;
      }
      uVar6 = *(uint *)PTR_I_STAT_800c39fc;
    }
    *PTR_JOY_MCD_DATA_800c3a00 = bVar3;
    *(char *)(param_1 + 0x45) = *(char *)(param_1 + 0x45) + '\x01';
    *(byte *)(*(int *)(param_1 + 0x3c) + (uint)*(byte *)(param_1 + 0x44)) = bVar1;
    *(char *)(param_1 + 0x44) = *(char *)(param_1 + 0x44) + '\x01';
  }
  return (uint)bVar1;
}


