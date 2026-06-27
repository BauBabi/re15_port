void C_011_OBJ_270(void)

{
  uint *puVar1;
  undefined4 uVar2;
  uint uVar3;
  short *psVar4;
  uint in_stack_00000028;
  
  psVar4 = DAT_8008fe1c;
  do {
  } while ((DMA_CDROM_CHCR & 0x1000000) != 0);
  uVar3 = (int)DAT_8008fe1c + 0x1fU & 3;
  puVar1 = (uint *)(((int)DAT_8008fe1c + 0x1fU) - uVar3);
  *puVar1 = *puVar1 & -1 << (uVar3 + 1) * 8 | in_stack_00000028 >> (3 - uVar3) * 8;
  uVar3 = (uint)(psVar4 + 0xe) & 3;
  puVar1 = (uint *)((int)(psVar4 + 0xe) - uVar3);
  *puVar1 = *puVar1 & 0xffffffffU >> (4 - uVar3) * 8 | in_stack_00000028 << uVar3 * 8;
  CDROM_DELAY = 0x20843;
  COMMON_DELAY = 0x1325;
  if ((DAT_800bbd98 == 1) && (DAT_800b2674 != 0)) {
    if (DAT_800b2674 != (ushort)DAT_8008fe1c[4]) {
      *DAT_8008fe1c = 0;
      if (DAT_800bbd88 == 0) {
        return;
      }
      DAT_800b5350 = DAT_800b5350 + 1;
      C_011_OBJ_960();
      return;
    }
    DAT_800bbd98 = 0;
  }
  if ((*DAT_8008fe1c != 0x160) || (((ushort)DAT_8008fe1c[1] >> 10 & 0x1f) != DAT_800b2b40)) {
    if (DAT_800bbd88 != 0) {
      DAT_800b5350 = 0;
      C_011_OBJ_3D0();
      return;
    }
    DAT_80078c38 = 5;
    *DAT_8008fe1c = 0;
    C_011_OBJ_960();
    return;
  }
  if (((int)DAT_800b2208 != (uint)(ushort)DAT_8008fe1c[2]) ||
     ((DAT_800aa5e0 != 0 && (DAT_800aa5e0 != (ushort)DAT_8008fe1c[4])))) {
    DAT_800aa5e0 = 0;
    DAT_800b2208 = 0;
    init_ring_status(DAT_800bb4c8,DAT_800bb4c4 - DAT_800bb4c8);
    DAT_800bb4c4 = DAT_800bb4c8;
    *DAT_8008fe1c = 0;
    if (DAT_800bbd88 != 0) {
      DAT_800b5350 = DAT_800b5350 + 1;
    }
    DAT_80078c38 = 6;
    C_011_OBJ_960();
    return;
  }
  if (DAT_8008fe1c[2] == 0) {
    DAT_800aa5e0 = (uint)(ushort)DAT_8008fe1c[4];
    DAT_800b2208 = 0;
    if ((DAT_800bbd94 != 0) && (DAT_800bbd94 <= DAT_800aa5e0)) {
      DAT_800aa5e0 = 0;
      DAT_800b2208 = 0;
      init_ring_status(DAT_800bb4c8,DAT_800bb4c4 - DAT_800bb4c8);
      DAT_800bb4c4 = DAT_800bb4c8;
      *DAT_8008fe1c = 0;
      DAT_800bbd98 = 1;
      if (DAT_800b24b4 != (code *)0x0) {
        (*DAT_800b24b4)();
      }
      if (DAT_800bbd88 != 0) {
        DAT_800b5350 = DAT_800b5350 + 1;
      }
      DAT_80078c38 = 7;
      C_011_OBJ_960();
      return;
    }
    if ((DAT_800bee74 - DAT_800bb4c4) - 1U < (uint)(ushort)DAT_8008fe1c[3]) {
      if (DAT_800bbd94 == 0) {
        *DAT_8008fe1c = 1;
        DAT_800bbd98 = 1;
        if (DAT_800b24b4 != (code *)0x0) {
          (*DAT_800b24b4)();
        }
        if (DAT_800bbd88 != 0) {
          DAT_800b5350 = DAT_800b5350 + 1;
        }
        DAT_80078c38 = 8;
        C_011_OBJ_960();
        return;
      }
      if (*DAT_800bbda4 != 0) {
        *DAT_8008fe1c = 0;
        if (DAT_800bbd88 != 0) {
          DAT_800b5350 = DAT_800b5350 + 1;
        }
        DAT_80078c38 = 9;
        C_011_OBJ_960();
        return;
      }
      *DAT_8008fe1c = 1;
      uVar3 = 0;
      DAT_800bb4c4 = 0;
      psVar4 = DAT_800bbda4;
      do {
        uVar2 = *(undefined4 *)DAT_8008fe1c;
        DAT_8008fe1c = DAT_8008fe1c + 2;
        uVar3 = uVar3 + 1;
        *(undefined4 *)psVar4 = uVar2;
        psVar4 = psVar4 + 2;
      } while (uVar3 < 8);
      DAT_8008fe1c = DAT_800bbda4;
    }
    DAT_800bb4c8 = DAT_800bb4c4;
  }
  DAT_80078c38 = 10;
  DAT_800b2208 = DAT_800b2208 + 1;
  DAT_800bbda0 = DAT_800bbda4 + DAT_800bee74 * 0x10 + DAT_800bb4c4 * 0x3f0;
  if (DAT_800b223c != 0) {
    CDROM_DELAY = 0x20943;
    COMMON_DELAY = 0x1323;
    C_011_OBJ_7BC();
    return;
  }
  CDROM_DELAY = 0x21020843;
  if ((ushort)DAT_8008fe1c[3] - 1 != (uint)(ushort)DAT_8008fe1c[2]) {
    if (DAT_800bbd88 == 0) {
      dma_execute(3,DAT_800bbda0,0,0x1f8);
      COMMON_DELAY = 0x1325;
      *DAT_8008fe1c = 3;
      DAT_800bb4c4 = DAT_800bb4c4 + 1;
      if ((DAT_800bbd88 != 0) && (DAT_800b5220 != 0)) {
        data_ready_callback();
      }
      return;
    }
    mem2mem(DAT_800bbda0,DAT_800b5350 * 0x800 + DAT_800bbd88 + 0x20,0x1f8,0);
    DAT_800b5350 = DAT_800b5350 + 1;
    C_011_OBJ_900();
    return;
  }
  DAT_800b5220 = 1;
  if (DAT_800bbd88 != 0) {
    mem2mem(DAT_800bbda0,DAT_800b5350 * 0x800 + DAT_800bbd88 + 0x20,0x1f8,1);
    DAT_800b5350 = DAT_800b5350 + 1;
    C_011_OBJ_85C();
    return;
  }
  dma_execute(3,DAT_800bbda0,0,0x1f8);
  DAT_800b2208 = 0;
  DAT_800aa5e0 = 0;
  DAT_800b2b40 = DAT_800b266c;
  C_011_OBJ_900();
  return;
}
