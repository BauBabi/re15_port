undefined4 BIOS_OBJ_64(byte *param_1)

{
  undefined1 uVar1;
  undefined4 uVar2;
  byte *pbVar3;
  undefined1 *puVar4;
  undefined4 *puVar5;
  int iVar6;
  uint unaff_s1;
  byte in_stack_00000010;
  byte bStack00000018;
  byte bStack00000019;
  
  while (iVar6 = 0, in_stack_00000010 != (*param_1 & 7)) {
    in_stack_00000010 = *param_1 & 7;
  }
  pbVar3 = &stack0x00000018;
  do {
    if ((CDROM_REG0 & 0x20) == 0) break;
    iVar6 = iVar6 + 1;
    *pbVar3 = CDROM_REG1;
    pbVar3 = pbVar3 + 1;
  } while (iVar6 < 8);
  if (iVar6 < 8) {
    puVar4 = (undefined1 *)((int)&stack0x00000018 + iVar6);
    do {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    } while ((int)puVar4 < (int)&stack0x00000020);
  }
  CDROM_REG0 = 1;
  CDROM_REG3 = 7;
  CDROM_REG2 = 7;
  if ((in_stack_00000010 != 3) || (*(int *)(&DAT_80078a68 + (uint)DAT_800788c5 * 4) != 0)) {
    if (((_DAT_800788b4 & 0x10) == 0) && ((bStack00000018 & 0x10) != 0)) {
      DAT_800788bc = DAT_800788bc + 1;
    }
    _DAT_800788b4 = (uint)bStack00000018;
    DAT_800788b8 = (uint)bStack00000019;
    unaff_s1 = _DAT_800788b4 & 0x1d;
  }
  if ((in_stack_00000010 == 5) && (puts("DiskError: "), 0 < DAT_800788b0)) {
    printf("com=%s,code=(%02x:%02x)\n",(&PTR_s_CdlSync_800788c8)[DAT_800788c5],_DAT_800788b4,
           DAT_800788b8);
  }
  switch(in_stack_00000010) {
  case 1:
    DAT_80078b81 = 1;
    if (unaff_s1 != 0) {
      if (iVar6 == 1) {
        unaff_s1 = 0;
      }
      DAT_80078b81 = 1;
      if (unaff_s1 != 0) {
        DAT_80078b81 = 5;
      }
    }
    puVar4 = &DAT_8008fe00;
    puVar5 = (undefined4 *)&stack0x00000018;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    CDROM_REG0 = 0;
    CDROM_REG3 = 0;
    uVar2 = BIOS_OBJ_560();
    return uVar2;
  case 2:
    DAT_80078b80 = 2;
    if (unaff_s1 != 0) {
      DAT_80078b80 = 5;
    }
    puVar4 = &DAT_8008fdf8;
    puVar5 = (undefined4 *)&stack0x00000018;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    uVar2 = BIOS_OBJ_560();
    return uVar2;
  case 3:
    break;
  case 4:
    puVar4 = &DAT_8008fe08;
    puVar5 = (undefined4 *)&stack0x00000018;
    DAT_80078b82 = 4;
    DAT_80078b81 = 4;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    puVar4 = &DAT_8008fe00;
    puVar5 = (undefined4 *)&stack0x00000018;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    uVar2 = BIOS_OBJ_560();
    return uVar2;
  case 5:
    puVar4 = &DAT_8008fdf8;
    puVar5 = (undefined4 *)&stack0x00000018;
    DAT_80078b81 = 5;
    DAT_80078b80 = 5;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    puVar4 = &DAT_8008fe00;
    puVar5 = (undefined4 *)&stack0x00000018;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    uVar2 = BIOS_OBJ_560();
    return uVar2;
  default:
    puts("CDROM: unknown intr");
    printf("(%d)\n",(uint)in_stack_00000010);
    return 0;
  }
  if (unaff_s1 != 0) {
    DAT_80078b80 = 5;
    puVar4 = &DAT_8008fdf8;
    puVar5 = (undefined4 *)&stack0x00000018;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    uVar2 = BIOS_OBJ_560();
    return uVar2;
  }
  if (*(int *)(&DAT_80078968 + (uint)DAT_800788c5 * 4) != 0) {
    DAT_80078b80 = 3;
    puVar4 = &DAT_8008fdf8;
    puVar5 = (undefined4 *)&stack0x00000018;
    iVar6 = 7;
    do {
      uVar1 = *(undefined1 *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 1;
    } while (iVar6 != -1);
    uVar2 = BIOS_OBJ_560();
    return uVar2;
  }
  DAT_80078b80 = 2;
  puVar4 = &DAT_8008fdf8;
  puVar5 = (undefined4 *)&stack0x00000018;
  iVar6 = 7;
  do {
    uVar1 = *(undefined1 *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    iVar6 = iVar6 + -1;
    *puVar4 = uVar1;
    puVar4 = puVar4 + 1;
  } while (iVar6 != -1);
  uVar2 = BIOS_OBJ_560();
  return uVar2;
}
