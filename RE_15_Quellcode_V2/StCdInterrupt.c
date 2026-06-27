void StCdInterrupt(void)

{
  int iVar1;
  undefined4 uVar2;
  uint *puVar3;
  uint uVar4;
  short *psVar5;
  uint local_18;
  uint uStack_14;
  byte local_10 [8];
  
  if (DAT_800b5220 != 1) {
    if ((DAT_800b223c != 0) && ((DMA_MDEC_OUT_CHCR & 0x1000000) != 0)) {
      DAT_800b24fc = 1;
      if (DAT_800bbd88 != 0) {
        DAT_800b5350 = DAT_800b5350 + 1;
      }
      DAT_80078c38 = 1;
      C_011_OBJ_960();
      return;
    }
    iVar1 = CdReady(1,local_10);
    if (iVar1 != 5) {
      if ((local_10[0] & 4) != 0) {
        DAT_80078c38 = 3;
        C_011_OBJ_960();
        return;
      }
      DAT_8008fe1c = DAT_800bbda4 + DAT_800bb4c4 * 0x10;
      if (*DAT_8008fe1c != 0) {
        if (DAT_800bbd88 != 0) {
          DAT_800b5350 = DAT_800b5350 + 1;
        }
        DAT_80078c38 = 4;
        C_011_OBJ_960();
        return;
      }
      CDROM_REG0 = 0;
      CDROM_REG3 = 0x80;
      CDROM_DELAY = 0x20943;
      COMMON_DELAY = 0x1323;
      if (DAT_800b2240 == 0) {
        puVar3 = &local_18;
        do {
          *(byte *)puVar3 = CDROM_REG2;
          puVar3 = (uint *)((int)puVar3 + 1);
        } while (puVar3 < &uStack_14);
        uVar4 = 0;
        do {
          uVar4 = uVar4 + 1;
        } while (uVar4 < 8);
      }
      if (DAT_800bbd88 != 0) {
        mem2mem(DAT_8008fe1c,DAT_800b5350 * 0x800 + DAT_800bbd88,8,0);
        C_011_OBJ_270();
        return;
      }
      dma_execute(3,DAT_8008fe1c,0,8,0x11000000,0,0);
      psVar5 = DAT_8008fe1c;
      do {
      } while ((DMA_CDROM_CHCR & 0x1000000) != 0);
      uVar4 = (int)DAT_8008fe1c + 0x1fU & 3;
      puVar3 = (uint *)(((int)DAT_8008fe1c + 0x1fU) - uVar4);
      *puVar3 = *puVar3 & -1 << (uVar4 + 1) * 8 | local_18 >> (3 - uVar4) * 8;
      uVar4 = (uint)(psVar5 + 0xe) & 3;
      puVar3 = (uint *)((int)(psVar5 + 0xe) - uVar4);
      *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar4) * 8 | local_18 << uVar4 * 8;
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
          uVar4 = 0;
          DAT_800bb4c4 = 0;
          psVar5 = DAT_800bbda4;
          do {
            uVar2 = *(undefined4 *)DAT_8008fe1c;
            DAT_8008fe1c = DAT_8008fe1c + 2;
            uVar4 = uVar4 + 1;
            *(undefined4 *)psVar5 = uVar2;
            psVar5 = psVar5 + 2;
          } while (uVar4 < 8);
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
      if ((ushort)DAT_8008fe1c[3] - 1 == (uint)(ushort)DAT_8008fe1c[2]) {
        DAT_800b5220 = 1;
        if (DAT_800bbd88 != 0) {
          mem2mem(DAT_800bbda0,DAT_800b5350 * 0x800 + DAT_800bbd88 + 0x20,0x1f8,1);
          DAT_800b5350 = DAT_800b5350 + 1;
          C_011_OBJ_85C();
          return;
        }
        dma_execute(3,DAT_800bbda0,0,0x1f8,0x11400100,1,0);
        DAT_800b2208 = 0;
        DAT_800aa5e0 = 0;
        DAT_800b2b40 = DAT_800b266c;
        C_011_OBJ_900();
        return;
      }
      if (DAT_800bbd88 != 0) {
        mem2mem(DAT_800bbda0,DAT_800b5350 * 0x800 + DAT_800bbd88 + 0x20,0x1f8,0);
        DAT_800b5350 = DAT_800b5350 + 1;
        C_011_OBJ_900();
        return;
      }
      dma_execute(3,DAT_800bbda0,0,0x1f8,0x11400100,0,0);
      COMMON_DELAY = 0x1325;
      *DAT_8008fe1c = 3;
      DAT_800bb4c4 = DAT_800bb4c4 + 1;
      if ((DAT_800bbd88 != 0) && (DAT_800b5220 != 0)) {
        data_ready_callback();
      }
    }
  }
  return;
}
