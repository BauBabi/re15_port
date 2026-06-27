void _SsContDataEntry(ushort param_1,short param_2,byte param_3)

{
  undefined1 *puVar1;
  undefined1 uVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  undefined4 uVar11;
  char cVar12;
  short sVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  undefined1 auStack_f8 [4];
  undefined1 auStack_f4 [4];
  undefined1 auStack_f0 [4];
  undefined1 auStack_ec [4];
  undefined1 auStack_e8 [4];
  undefined1 auStack_e4 [4];
  undefined1 auStack_e0 [4];
  undefined1 auStack_dc [4];
  undefined1 auStack_d8 [4];
  undefined1 auStack_d4 [4];
  undefined1 auStack_d0 [4];
  undefined2 local_cc;
  uint local_c8;
  uint local_c4;
  ProgAtr local_c0;
  VagAtr local_b0;
  uint uStack_90;
  uint uStack_8c;
  uint uStack_88;
  uint uStack_84;
  undefined2 local_80;
  short local_78;
  
  iVar16 = param_2 * 0xac + (&DAT_800bb500)[(short)param_1];
  uVar17 = (uint)*(byte *)(iVar16 + 0x12);
  local_78 = param_2;
  SsUtGetProgAtr(*(short *)(iVar16 + 0x4c),(ushort)*(byte *)(uVar17 + iVar16 + 0x2c),&local_c0);
  if ((*(char *)(iVar16 + 0x27) == '\x01') && (*(char *)(iVar16 + 0x10) == '\0')) {
    *(byte *)(iVar16 + 0x28) = param_3;
    *(undefined1 *)(iVar16 + 0x10) = 1;
    SEQREAD_OBJ_159C((int)(short)param_1,(int)param_2);
    return;
  }
  if (*(char *)(iVar16 + 0x29) != '\x02') {
    if (*(char *)(iVar16 + 0x2a) != '\x02') {
      uVar11 = _SsReadDeltaValue((int)(short)param_1,(int)local_78);
      *(undefined4 *)(iVar16 + 0x88) = uVar11;
      return;
    }
    cVar12 = *(char *)(iVar16 + 0x16);
    if (cVar12 == '\x10') {
      iVar15 = 0;
      if (local_c0.tones != 0) {
        do {
          uVar10 = uStack_84;
          uVar9 = uStack_88;
          uVar8 = uStack_8c;
          auStack_dc = (undefined1  [4])uStack_90;
          sVar13 = *(short *)(iVar16 + 0x4c);
          uVar2 = *(undefined1 *)(uVar17 + iVar16 + 0x2c);
          puVar1 = auStack_dc + 3;
          uVar3 = (uint)puVar1 & 3;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | uStack_90 >> (3 - uVar3) * 8;
          puVar1 = auStack_d8 + 3;
          uVar3 = (uint)puVar1 & 3;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | uVar8 >> (3 - uVar3) * 8;
          auStack_d8 = (undefined1  [4])uVar8;
          puVar1 = auStack_d4 + 3;
          uVar3 = (uint)puVar1 & 3;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | uVar9 >> (3 - uVar3) * 8;
          auStack_d4 = (undefined1  [4])uVar9;
          puVar1 = auStack_d0 + 3;
          uVar3 = (uint)puVar1 & 3;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | uVar10 >> (3 - uVar3) * 8;
          uVar7 = local_b0._16_4_;
          uVar6 = local_b0._12_4_;
          uVar5 = local_b0._8_4_;
          uVar11 = local_b0._4_4_;
          auStack_d0 = (undefined1  [4])uVar10;
          local_cc = local_80;
          local_c4 = (uint)param_3;
          local_c8 = (uint)*(byte *)(iVar16 + 0x15);
          puVar1 = auStack_f8 + 3;
          uVar3 = (uint)puVar1 & 3;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 |
               (uint)local_b0._4_4_ >> (3 - uVar3) * 8;
          uVar4 = local_b0._4_4_;
          local_b0.center = (uchar)uVar11;
          local_b0.shift = SUB41(uVar11,1);
          local_b0.min = SUB41(uVar11,2);
          local_b0.max = SUB41(uVar11,3);
          auStack_f8[0] = local_b0.center;
          auStack_f8[1] = local_b0.shift;
          auStack_f8[2] = local_b0.min;
          auStack_f8[3] = local_b0.max;
          puVar1 = auStack_f4 + 3;
          uVar3 = (uint)puVar1 & 3;
          local_b0._4_4_ = uVar4;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | (uint)uVar5 >> (3 - uVar3) * 8;
          uVar11 = local_b0._8_4_;
          local_b0.vibW = (uchar)uVar5;
          local_b0.vibT = SUB41(uVar5,1);
          local_b0.porW = SUB41(uVar5,2);
          local_b0.porT = SUB41(uVar5,3);
          auStack_f4[0] = local_b0.vibW;
          auStack_f4[1] = local_b0.vibT;
          auStack_f4[2] = local_b0.porW;
          auStack_f4[3] = local_b0.porT;
          puVar1 = auStack_f0 + 3;
          uVar3 = (uint)puVar1 & 3;
          local_b0._8_4_ = uVar11;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | (uint)uVar6 >> (3 - uVar3) * 8;
          uVar11 = local_b0._12_4_;
          local_b0.pbmin = (uchar)uVar6;
          local_b0.pbmax = SUB41(uVar6,1);
          local_b0.reserved1 = SUB41(uVar6,2);
          local_b0.reserved2 = SUB41(uVar6,3);
          auStack_f0[0] = local_b0.pbmin;
          auStack_f0[1] = local_b0.pbmax;
          auStack_f0[2] = local_b0.reserved1;
          auStack_f0[3] = local_b0.reserved2;
          puVar1 = auStack_ec + 3;
          uVar3 = (uint)puVar1 & 3;
          local_b0._12_4_ = uVar11;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | (uint)uVar7 >> (3 - uVar3) * 8;
          uVar6 = local_b0.reserved._4_4_;
          uVar5 = local_b0.reserved._0_4_;
          uVar4 = local_b0._20_4_;
          uVar11 = local_b0._16_4_;
          local_b0.adsr1 = (ushort)uVar7;
          local_b0.adsr2 = SUB42(uVar7,2);
          auStack_ec._0_2_ = local_b0.adsr1;
          auStack_ec._2_2_ = local_b0.adsr2;
          puVar1 = auStack_e8 + 3;
          uVar3 = (uint)puVar1 & 3;
          local_b0._16_4_ = uVar11;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 |
               (uint)local_b0._20_4_ >> (3 - uVar3) * 8;
          uVar11 = local_b0._20_4_;
          local_b0.prog = (short)uVar4;
          local_b0.vag = SUB42(uVar4,2);
          auStack_e8._0_2_ = local_b0.prog;
          auStack_e8._2_2_ = local_b0.vag;
          puVar1 = auStack_e4 + 3;
          uVar3 = (uint)puVar1 & 3;
          local_b0._20_4_ = uVar11;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | (uint)uVar5 >> (3 - uVar3) * 8;
          uVar11 = local_b0.reserved._0_4_;
          local_b0.reserved[0] = (short)uVar5;
          local_b0.reserved[1] = SUB42(uVar5,2);
          auStack_e4._0_2_ = local_b0.reserved[0];
          auStack_e4._2_2_ = local_b0.reserved[1];
          puVar1 = auStack_e0 + 3;
          uVar3 = (uint)puVar1 & 3;
          local_b0.reserved._0_4_ = uVar11;
          *(uint *)(puVar1 + -uVar3) =
               *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | (uint)uVar6 >> (3 - uVar3) * 8;
          uVar11 = local_b0.reserved._4_4_;
          local_b0.reserved[2] = (short)uVar6;
          local_b0.reserved[3] = SUB42(uVar6,2);
          auStack_e0._0_2_ = local_b0.reserved[2];
          auStack_e0._2_2_ = local_b0.reserved[3];
          local_b0.reserved._4_4_ = uVar11;
          _SsSndSetVabAttr((int)sVar13,uVar2);
          iVar15 = iVar15 + 1;
        } while (iVar15 < (int)(uint)local_c0.tones);
        SEQREAD_OBJ_156C((uint)param_1 << 0x10);
        return;
      }
    }
    else {
      sVar13 = *(short *)(iVar16 + 0x4c);
      uVar2 = *(undefined1 *)(uVar17 + iVar16 + 0x2c);
      puVar1 = auStack_dc + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 | uStack_90 >> (3 - uVar17) * 8;
      auStack_dc = (undefined1  [4])uStack_90;
      puVar1 = auStack_d8 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 | uStack_8c >> (3 - uVar17) * 8;
      auStack_d8 = (undefined1  [4])uStack_8c;
      puVar1 = auStack_d4 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 | uStack_88 >> (3 - uVar17) * 8;
      auStack_d4 = (undefined1  [4])uStack_88;
      puVar1 = auStack_d0 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 | uStack_84 >> (3 - uVar17) * 8;
      auStack_d0 = (undefined1  [4])uStack_84;
      local_cc = local_80;
      local_c4 = (uint)param_3;
      local_c8 = (uint)*(byte *)(iVar16 + 0x15);
      puVar1 = auStack_f8 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 |
           (uint)local_b0._4_4_ >> (3 - uVar17) * 8;
      auStack_f8[0] = local_b0.center;
      auStack_f8[1] = local_b0.shift;
      auStack_f8[2] = local_b0.min;
      auStack_f8[3] = local_b0.max;
      puVar1 = auStack_f4 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 |
           (uint)local_b0._8_4_ >> (3 - uVar17) * 8;
      auStack_f4[0] = local_b0.vibW;
      auStack_f4[1] = local_b0.vibT;
      auStack_f4[2] = local_b0.porW;
      auStack_f4[3] = local_b0.porT;
      puVar1 = auStack_f0 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 |
           (uint)local_b0._12_4_ >> (3 - uVar17) * 8;
      auStack_f0[0] = local_b0.pbmin;
      auStack_f0[1] = local_b0.pbmax;
      auStack_f0[2] = local_b0.reserved1;
      auStack_f0[3] = local_b0.reserved2;
      puVar1 = auStack_ec + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 |
           (uint)local_b0._16_4_ >> (3 - uVar17) * 8;
      auStack_ec._0_2_ = local_b0.adsr1;
      auStack_ec._2_2_ = local_b0.adsr2;
      puVar1 = auStack_e8 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 |
           (uint)local_b0._20_4_ >> (3 - uVar17) * 8;
      auStack_e8._0_2_ = local_b0.prog;
      auStack_e8._2_2_ = local_b0.vag;
      puVar1 = auStack_e4 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 |
           (uint)local_b0.reserved._0_4_ >> (3 - uVar17) * 8;
      auStack_e4._0_2_ = local_b0.reserved[0];
      auStack_e4._2_2_ = local_b0.reserved[1];
      puVar1 = auStack_e0 + 3;
      uVar17 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar17) =
           *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 |
           (uint)local_b0.reserved._4_4_ >> (3 - uVar17) * 8;
      auStack_e0._0_2_ = local_b0.reserved[2];
      auStack_e0._2_2_ = local_b0.reserved[3];
      _SsSndSetVabAttr((int)sVar13,uVar2,cVar12,local_b0._0_4_);
    }
    uVar11 = _SsReadDeltaValue((int)(short)param_1,(int)local_78);
    *(undefined4 *)(iVar16 + 0x88) = uVar11;
    *(undefined1 *)(iVar16 + 0x2a) = 0;
    SEQREAD_OBJ_15A8();
    return;
  }
  if (((*(char *)(iVar16 + 0x13) == '\0') && (*(char *)(iVar16 + 0x14) == '\0')) &&
     (iVar15 = 0, local_c0.tones != 0)) {
    iVar14 = 0;
    do {
      sVar13 = (short)((uint)iVar14 >> 0x10);
      SsUtGetVagAtr(*(short *)(iVar16 + 0x4c),(ushort)*(byte *)(uVar17 + iVar16 + 0x2c),sVar13,
                    &local_b0);
      local_b0._12_4_ = CONCAT31(CONCAT21(local_b0._14_2_,param_3),param_3) & 0xffff7f7f;
      SsUtSetVagAtr(*(short *)(iVar16 + 0x4c),(ushort)*(byte *)(uVar17 + iVar16 + 0x2c),sVar13,
                    &local_b0);
      iVar15 = iVar15 + 1;
      iVar14 = iVar15 * 0x10000;
    } while (iVar15 < (int)(uint)local_c0.tones);
  }
  cVar12 = *(char *)(iVar16 + 0x13);
  if ((cVar12 == '\x01') && (*(char *)(iVar16 + 0x14) == '\0')) {
    iVar15 = 0;
    if (local_c0.tones != 0) {
      iVar14 = 0;
      do {
        sVar13 = (short)((uint)iVar14 >> 0x10);
        SsUtGetVagAtr(*(short *)(iVar16 + 0x4c),(ushort)*(byte *)(uVar17 + iVar16 + 0x2c),sVar13,
                      &local_b0);
        SsUtSetVagAtr(*(short *)(iVar16 + 0x4c),(ushort)*(byte *)(uVar17 + iVar16 + 0x2c),sVar13,
                      &local_b0);
        iVar15 = iVar15 + 1;
        iVar14 = iVar15 * 0x10000;
      } while (iVar15 < (int)(uint)local_c0.tones);
    }
    cVar12 = *(char *)(iVar16 + 0x13);
  }
  if ((cVar12 == '\x02') && (*(char *)(iVar16 + 0x14) == '\0')) {
    if ((byte)(param_3 - 0x40) < 0x40) {
      SEQREAD_OBJ_12B4();
      return;
    }
    iVar15 = 0;
    if (local_c0.tones != 0) {
      iVar14 = 0;
      do {
        sVar13 = (short)((uint)iVar14 >> 0x10);
        SsUtGetVagAtr(*(short *)(iVar16 + 0x4c),(ushort)*(byte *)(uVar17 + iVar16 + 0x2c),sVar13,
                      &local_b0);
        SsUtSetVagAtr(*(short *)(iVar16 + 0x4c),(ushort)*(byte *)(uVar17 + iVar16 + 0x2c),sVar13,
                      &local_b0);
        iVar15 = iVar15 + 1;
        iVar14 = iVar15 * 0x10000;
      } while (iVar15 < (int)(uint)local_c0.tones);
    }
  }
  uVar11 = _SsReadDeltaValue((int)(short)param_1,(int)local_78);
  *(undefined4 *)(iVar16 + 0x88) = uVar11;
  *(undefined1 *)(iVar16 + 0x29) = 0;
  SEQREAD_OBJ_15A8();
  return;
}
