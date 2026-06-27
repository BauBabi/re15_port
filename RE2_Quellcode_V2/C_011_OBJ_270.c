/* C_011_OBJ_270 @ 0x800890d4  (Ghidra headless, raw MIPS overlay) */

void C_011_OBJ_270(void)

{
  uint *puVar1;
  uint uVar2;
  undefined4 uVar3;
  short *psVar4;
  uint in_stack_00000028;
  
  psVar4 = DAT_800c3d68;
  uVar2 = *(uint *)PTR_DMA_CDROM_CHCR_800ad17c;
  while ((uVar2 & 0x1000000) != 0) {
    uVar2 = *(uint *)PTR_DMA_CDROM_CHCR_800ad17c;
  }
  uVar2 = (int)DAT_800c3d68 + 0x1fU & 3;
  puVar1 = (uint *)(((int)DAT_800c3d68 + 0x1fU) - uVar2);
  *puVar1 = *puVar1 & -1 << (uVar2 + 1) * 8 | in_stack_00000028 >> (3 - uVar2) * 8;
  uVar2 = (uint)(psVar4 + 0xe) & 3;
  puVar1 = (uint *)((int)(psVar4 + 0xe) - uVar2);
  *puVar1 = *puVar1 & 0xffffffffU >> (4 - uVar2) * 8 | in_stack_00000028 << uVar2 * 8;
  *(undefined4 *)PTR_CDROM_DELAY_800ad15c = 0x20843;
  *(undefined4 *)PTR_COMMON_DELAY_800ad160 = 0x1325;
  if ((DAT_800ead90 == 1) && (DAT_800d6c3c != 0)) {
    if (DAT_800d6c3c != (ushort)DAT_800c3d68[4]) {
      *DAT_800c3d68 = 0;
      if (DAT_800ead74 == 0) {
        return;
      }
      DAT_800df33c = DAT_800df33c + 1;
      C_011_OBJ_960();
      return;
    }
    DAT_800ead90 = 0;
  }
  if ((*DAT_800c3d68 != 0x160) || (((ushort)DAT_800c3d68[1] >> 10 & 0x1f) != DAT_800d7850)) {
    if (DAT_800ead74 == 0) {
      DAT_800ad194 = 5;
      *DAT_800c3d68 = 0;
      C_011_OBJ_960();
      return;
    }
    DAT_800df33c = 0;
    C_011_OBJ_3D0();
    return;
  }
  if (((int)DAT_800d4c64 != (uint)(ushort)DAT_800c3d68[2]) ||
     ((DAT_800cbc24 != 0 && (DAT_800cbc24 != (ushort)DAT_800c3d68[4])))) {
    DAT_800cbc24 = 0;
    DAT_800d4c64 = 0;
    init_ring_status(DAT_800ea230,DAT_800ea22c - DAT_800ea230);
    DAT_800ea22c = DAT_800ea230;
    *DAT_800c3d68 = 0;
    if (DAT_800ead74 != 0) {
      DAT_800df33c = DAT_800df33c + 1;
    }
    DAT_800ad194 = 6;
    C_011_OBJ_960();
    return;
  }
  if (DAT_800c3d68[2] == 0) {
    DAT_800cbc24 = (uint)(ushort)DAT_800c3d68[4];
    DAT_800d4c64 = 0;
    if ((DAT_800ead7c != 0) && (DAT_800ead7c <= DAT_800cbc24)) {
      DAT_800cbc24 = 0;
      DAT_800d4c64 = 0;
      init_ring_status(DAT_800ea230,DAT_800ea22c - DAT_800ea230);
      DAT_800ea22c = DAT_800ea230;
      *DAT_800c3d68 = 0;
      DAT_800ead90 = 1;
      if (DAT_800d521c != (code *)0x0) {
        (*DAT_800d521c)();
      }
      if (DAT_800ead74 != 0) {
        DAT_800df33c = DAT_800df33c + 1;
      }
      DAT_800ad194 = 7;
      C_011_OBJ_960();
      return;
    }
    if ((DAT_800eae9c - DAT_800ea22c) - 1U < (uint)(ushort)DAT_800c3d68[3]) {
      if (DAT_800ead7c == 0) {
        *DAT_800c3d68 = 1;
        DAT_800ead90 = 1;
        if (DAT_800d521c != (code *)0x0) {
          (*DAT_800d521c)();
        }
        if (DAT_800ead74 != 0) {
          DAT_800df33c = DAT_800df33c + 1;
        }
        DAT_800ad194 = 8;
        C_011_OBJ_960();
        return;
      }
      if (*DAT_800ead9c != 0) {
        *DAT_800c3d68 = 0;
        if (DAT_800ead74 != 0) {
          DAT_800df33c = DAT_800df33c + 1;
        }
        DAT_800ad194 = 9;
        C_011_OBJ_960();
        return;
      }
      *DAT_800c3d68 = 1;
      uVar2 = 0;
      DAT_800ea22c = 0;
      psVar4 = DAT_800ead9c;
      do {
        uVar3 = *(undefined4 *)DAT_800c3d68;
        DAT_800c3d68 = DAT_800c3d68 + 2;
        uVar2 = uVar2 + 1;
        *(undefined4 *)psVar4 = uVar3;
        psVar4 = psVar4 + 2;
      } while (uVar2 < 8);
      DAT_800c3d68 = DAT_800ead9c;
    }
    DAT_800ea230 = DAT_800ea22c;
  }
  DAT_800ad194 = 10;
  DAT_800d4c64 = DAT_800d4c64 + 1;
  DAT_800ead98 = DAT_800ead9c + DAT_800eae9c * 0x10 + DAT_800ea22c * 0x3f0;
  if (DAT_800d4c68 != 0) {
    *(undefined4 *)PTR_CDROM_DELAY_800ad15c = 0x20943;
    *(undefined4 *)PTR_COMMON_DELAY_800ad160 = 0x1323;
    C_011_OBJ_7BC();
    return;
  }
  *(undefined4 *)PTR_CDROM_DELAY_800ad15c = 0x21020843;
  if ((ushort)DAT_800c3d68[3] - 1 != (uint)(ushort)DAT_800c3d68[2]) {
    if (DAT_800ead74 == 0) {
      dma_execute(3,DAT_800ead98,0,0x1f8);
      *(undefined4 *)PTR_COMMON_DELAY_800ad160 = 0x1325;
      *DAT_800c3d68 = 3;
      DAT_800ea22c = DAT_800ea22c + 1;
      if ((DAT_800ead74 != 0) && (DAT_800dbb90 != 0)) {
        data_ready_callback();
      }
      return;
    }
    mem2mem(DAT_800ead98,DAT_800ead74 + DAT_800df33c * 0x800 + 0x20,0x1f8,0);
    DAT_800df33c = DAT_800df33c + 1;
    C_011_OBJ_900();
    return;
  }
  DAT_800dbb90 = 1;
  if (DAT_800ead74 == 0) {
    dma_execute(3,DAT_800ead98,0,0x1f8);
    DAT_800d4c64 = 0;
    DAT_800cbc24 = 0;
    DAT_800d7850 = DAT_800d6c38;
    C_011_OBJ_900();
    return;
  }
  mem2mem(DAT_800ead98,DAT_800ead74 + DAT_800df33c * 0x800 + 0x20,0x1f8,1);
  DAT_800df33c = DAT_800df33c + 1;
  C_011_OBJ_85C();
  return;
}


