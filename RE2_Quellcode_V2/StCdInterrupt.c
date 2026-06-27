/* StCdInterrupt @ 0x80088e64  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void StCdInterrupt(void)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  uint *puVar4;
  uint uVar5;
  short *psVar6;
  uint local_18;
  uint uStack_14;
  byte local_10 [8];
  
  if (DAT_800dbb90 != 1) {
    if ((DAT_800d4c68 != 0) && ((*(uint *)PTR_DMA_MDEC_OUT_CHCR_800ad16c & 0x1000000) != 0)) {
      DAT_800d5268 = 1;
      if (DAT_800ead74 != 0) {
        DAT_800df33c = DAT_800df33c + 1;
      }
      DAT_800ad194 = 1;
      C_011_OBJ_960();
      return;
    }
    iVar2 = CdReady(1,local_10);
    if (iVar2 != 5) {
      if ((local_10[0] & 4) != 0) {
        DAT_800ad194 = 3;
        C_011_OBJ_960();
        return;
      }
      DAT_800c3d68 = DAT_800ead9c + DAT_800ea22c * 0x10;
      if (*DAT_800c3d68 != 0) {
        if (DAT_800ead74 != 0) {
          DAT_800df33c = DAT_800df33c + 1;
        }
        DAT_800ad194 = 4;
        C_011_OBJ_960();
        return;
      }
      *PTR_CDROM_REG0_800ad14c = 0;
      *PTR_CDROM_REG3_800ad158 = 0;
      *PTR_CDROM_REG0_800ad14c = 0;
      *PTR_CDROM_REG3_800ad158 = 0x80;
      *(undefined4 *)PTR_CDROM_DELAY_800ad15c = 0x20943;
      *(undefined4 *)PTR_COMMON_DELAY_800ad160 = 0x1323;
      if (DAT_800d4c6c == 0) {
        puVar4 = &local_18;
        do {
          *(undefined *)puVar4 = *PTR_CDROM_REG2_800ad154;
          puVar4 = (uint *)((int)puVar4 + 1);
        } while (puVar4 < &uStack_14);
        uVar5 = 0;
        do {
          uVar1 = *PTR_CDROM_REG2_800ad154;
          uVar5 = uVar5 + 1;
        } while (uVar5 < 8);
      }
      if (DAT_800ead74 != 0) {
        mem2mem(DAT_800c3d68,DAT_800ead74 + DAT_800df33c * 0x800,8,0);
        C_011_OBJ_270();
        return;
      }
      dma_execute(3,DAT_800c3d68,0,8,0x11000000,0,0);
      psVar6 = DAT_800c3d68;
      uVar5 = *(uint *)PTR_DMA_CDROM_CHCR_800ad17c;
      while ((uVar5 & 0x1000000) != 0) {
        uVar5 = *(uint *)PTR_DMA_CDROM_CHCR_800ad17c;
      }
      uVar5 = (int)DAT_800c3d68 + 0x1fU & 3;
      puVar4 = (uint *)(((int)DAT_800c3d68 + 0x1fU) - uVar5);
      *puVar4 = *puVar4 & -1 << (uVar5 + 1) * 8 | local_18 >> (3 - uVar5) * 8;
      uVar5 = (uint)(psVar6 + 0xe) & 3;
      puVar4 = (uint *)((int)(psVar6 + 0xe) - uVar5);
      *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar5) * 8 | local_18 << uVar5 * 8;
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
          uVar5 = 0;
          DAT_800ea22c = 0;
          psVar6 = DAT_800ead9c;
          do {
            uVar3 = *(undefined4 *)DAT_800c3d68;
            DAT_800c3d68 = DAT_800c3d68 + 2;
            uVar5 = uVar5 + 1;
            *(undefined4 *)psVar6 = uVar3;
            psVar6 = psVar6 + 2;
          } while (uVar5 < 8);
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
      if ((ushort)DAT_800c3d68[3] - 1 == (uint)(ushort)DAT_800c3d68[2]) {
        DAT_800dbb90 = 1;
        if (DAT_800ead74 == 0) {
          dma_execute(3,DAT_800ead98,0,0x1f8,0x11400100,1,0);
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
      if (DAT_800ead74 != 0) {
        mem2mem(DAT_800ead98,DAT_800ead74 + DAT_800df33c * 0x800 + 0x20,0x1f8,0);
        DAT_800df33c = DAT_800df33c + 1;
        C_011_OBJ_900();
        return;
      }
      dma_execute(3,DAT_800ead98,0,0x1f8,0x11400100,0,0);
      *(undefined4 *)PTR_COMMON_DELAY_800ad160 = 0x1325;
      *DAT_800c3d68 = 3;
      DAT_800ea22c = DAT_800ea22c + 1;
      if ((DAT_800ead74 != 0) && (DAT_800dbb90 != 0)) {
        data_ready_callback();
      }
    }
  }
  return;
}


