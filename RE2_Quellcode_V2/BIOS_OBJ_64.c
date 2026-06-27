/* BIOS_OBJ_64 @ 0x80087490  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 BIOS_OBJ_64(byte *param_1)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined1 *puVar4;
  int iVar5;
  uint unaff_s1;
  byte in_stack_00000010;
  byte bStack00000018;
  byte bStack00000019;
  
  while (iVar5 = 0, in_stack_00000010 != (*param_1 & 7)) {
    in_stack_00000010 = *param_1 & 7;
  }
  puVar3 = (undefined4 *)&stack0x00000018;
  do {
    if ((*PTR_CDROM_REG0_800ad0b4 & 0x20) == 0) break;
    iVar5 = iVar5 + 1;
    *(undefined *)puVar3 = *PTR_CDROM_REG1_800ad0b8;
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  } while (iVar5 < 8);
  if (iVar5 < 8) {
    puVar4 = (undefined1 *)((int)&stack0x00000018 + iVar5);
    do {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    } while ((int)puVar4 < (int)&stack0x00000020);
  }
  *PTR_CDROM_REG0_800ad0b4 = 1;
  *PTR_CDROM_REG3_800ad0c0 = 7;
  *PTR_CDROM_REG2_800ad0bc = 7;
  if ((in_stack_00000010 != 3) || (*(int *)(&DAT_800acfb4 + (uint)DAT_800ace0d * 4) != 0)) {
    if (((_DAT_800acdfc & 0x10) == 0) && ((bStack00000018 & 0x10) != 0)) {
      DAT_800ace04 = DAT_800ace04 + 1;
    }
    _DAT_800acdfc = (uint)bStack00000018;
    DAT_800ace00 = (uint)bStack00000019;
    unaff_s1 = _DAT_800acdfc & 0x1d;
  }
  if (((in_stack_00000010 == 5) && (0 < DAT_800acdf8)) && (printf("DiskError: "), 0 < DAT_800acdf8))
  {
    printf("com=%s,code=(%02x:%02x)\n",(&PTR_s_CdlSync_800ace14)[DAT_800ace0d],_DAT_800acdfc,
           DAT_800ace00);
  }
  switch(in_stack_00000010) {
  case 1:
    DAT_800ad0cd = 1;
    if (unaff_s1 != 0) {
      if (iVar5 == 1) {
        unaff_s1 = 0;
      }
      DAT_800ad0cd = 1;
      if (unaff_s1 != 0) {
        DAT_800ad0cd = 5;
      }
    }
    puVar4 = &DAT_800c3d48;
    puVar3 = (undefined4 *)&stack0x00000018;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    *PTR_CDROM_REG0_800ad0b4 = 0;
    *PTR_CDROM_REG3_800ad0c0 = 0;
    uVar2 = BIOS_OBJ_570();
    return uVar2;
  case 2:
    DAT_800ad0cc = 2;
    if (unaff_s1 != 0) {
      DAT_800ad0cc = 5;
    }
    puVar4 = &DAT_800c3d40;
    puVar3 = (undefined4 *)&stack0x00000018;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    uVar2 = BIOS_OBJ_570();
    return uVar2;
  case 3:
    break;
  case 4:
    puVar4 = &DAT_800c3d50;
    DAT_800ad0ce = 4;
    puVar3 = (undefined4 *)&stack0x00000018;
    DAT_800ad0cd = 4;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    puVar4 = &DAT_800c3d48;
    puVar3 = (undefined4 *)&stack0x00000018;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    uVar2 = BIOS_OBJ_570();
    return uVar2;
  case 5:
    puVar4 = &DAT_800c3d40;
    puVar3 = (undefined4 *)&stack0x00000018;
    DAT_800ad0cd = 5;
    DAT_800ad0cc = 5;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    puVar4 = &DAT_800c3d48;
    puVar3 = (undefined4 *)&stack0x00000018;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    uVar2 = BIOS_OBJ_570();
    return uVar2;
  default:
    FUN_8009881c("CDROM: unknown intr");
    printf("(%d)\n",(uint)in_stack_00000010);
    return 0;
  }
  if (unaff_s1 != 0) {
    DAT_800ad0cc = 5;
    puVar4 = &DAT_800c3d40;
    puVar3 = (undefined4 *)&stack0x00000018;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    uVar2 = BIOS_OBJ_570();
    return uVar2;
  }
  if (*(int *)(&DAT_800aceb4 + (uint)DAT_800ace0d * 4) != 0) {
    DAT_800ad0cc = 3;
    puVar4 = &DAT_800c3d40;
    puVar3 = (undefined4 *)&stack0x00000018;
    iVar5 = 7;
    do {
      uVar1 = *(undefined1 *)puVar3;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
      iVar5 = iVar5 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar5 != -1);
    uVar2 = BIOS_OBJ_570();
    return uVar2;
  }
  DAT_800ad0cc = 2;
  puVar4 = &DAT_800c3d40;
  puVar3 = (undefined4 *)&stack0x00000018;
  iVar5 = 7;
  do {
    uVar1 = *(undefined1 *)puVar3;
    puVar3 = (undefined4 *)((int)puVar3 + 1);
    iVar5 = iVar5 + -1;
    *puVar4 = uVar1;
    puVar4 = puVar4 + 1;
  } while (iVar5 != -1);
  uVar2 = BIOS_OBJ_570();
  return uVar2;
}


