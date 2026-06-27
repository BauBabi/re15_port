/* _SsContDataEntry @ 0x8007a894  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8007aa28) */

void _SsContDataEntry(short param_1,short param_2,uchar param_3)

{
  undefined1 *puVar1;
  byte bVar2;
  char cVar3;
  undefined1 uVar4;
  undefined1 uVar5;
  uint uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  short sVar13;
  int iVar14;
  int iVar15;
  uint uVar16;
  int iVar17;
  undefined1 auStack_c8 [4];
  undefined1 auStack_c4 [4];
  undefined1 auStack_c0 [4];
  undefined1 auStack_bc [4];
  undefined1 auStack_b8 [4];
  undefined1 auStack_b4 [4];
  undefined1 auStack_b0 [4];
  uint uStack_ac;
  uint uStack_a8;
  ProgAtr local_a0;
  VagAtr VStack_90;
  int local_70;
  
  iVar14 = (&DAT_800ea250)[param_1] + param_2 * 0xb0;
  uVar16 = (uint)*(byte *)(iVar14 + 0x17);
  SsUtGetProgAtr((ushort)*(byte *)(iVar14 + 0x26),(ushort)*(byte *)(uVar16 + iVar14 + 0x37),
                 &local_a0);
  if ((*(char *)(iVar14 + 0x1c) == '\x01') && (*(char *)(iVar14 + 0x15) == '\0')) {
    *(uchar *)(iVar14 + 0x1d) = param_3;
    *(undefined1 *)(iVar14 + 0x1c) = 0;
    *(undefined1 *)(iVar14 + 0x15) = 1;
    CC_6_OBJ_3E8((int)param_1,(int)param_2);
    return;
  }
  if (*(char *)(iVar14 + 0x1e) != '\x02') {
    if (*(char *)(iVar14 + 0x1f) != '\x02') {
      uVar12 = _SsReadDeltaValue((int)param_1,(int)param_2);
      *(undefined4 *)(iVar14 + 0x90) = uVar12;
      return;
    }
    cVar3 = *(char *)(iVar14 + 0x1b);
    if (cVar3 == '\x10') {
      iVar15 = 0;
      if (local_a0.tones != 0) {
        do {
          uVar11 = VStack_90._16_4_;
          uVar10 = VStack_90._12_4_;
          uVar9 = VStack_90._8_4_;
          uVar7 = VStack_90._4_4_;
          uVar12 = VStack_90._0_4_;
          bVar2 = *(byte *)(iVar14 + 0x1a);
          uVar4 = *(undefined1 *)(iVar14 + 0x26);
          uVar5 = *(undefined1 *)(uVar16 + iVar14 + 0x37);
          uStack_a8 = (uint)param_3;
          uStack_ac = (uint)bVar2;
          puVar1 = auStack_c8 + 3;
          uVar6 = (uint)puVar1 & 3;
          *(uint *)(puVar1 + -uVar6) =
               *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 |
               (uint)VStack_90._4_4_ >> (3 - uVar6) * 8;
          uVar8 = VStack_90._4_4_;
          VStack_90.center = (uchar)uVar7;
          VStack_90.shift = SUB41(uVar7,1);
          VStack_90.min = SUB41(uVar7,2);
          VStack_90.max = SUB41(uVar7,3);
          auStack_c8[0] = VStack_90.center;
          auStack_c8[1] = VStack_90.shift;
          auStack_c8[2] = VStack_90.min;
          auStack_c8[3] = VStack_90.max;
          puVar1 = auStack_c4 + 3;
          uVar6 = (uint)puVar1 & 3;
          VStack_90._4_4_ = uVar8;
          *(uint *)(puVar1 + -uVar6) =
               *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 | (uint)uVar9 >> (3 - uVar6) * 8;
          uVar7 = VStack_90._8_4_;
          VStack_90.vibW = (uchar)uVar9;
          VStack_90.vibT = SUB41(uVar9,1);
          VStack_90.porW = SUB41(uVar9,2);
          VStack_90.porT = SUB41(uVar9,3);
          auStack_c4[0] = VStack_90.vibW;
          auStack_c4[1] = VStack_90.vibT;
          auStack_c4[2] = VStack_90.porW;
          auStack_c4[3] = VStack_90.porT;
          puVar1 = auStack_c0 + 3;
          uVar6 = (uint)puVar1 & 3;
          VStack_90._8_4_ = uVar7;
          *(uint *)(puVar1 + -uVar6) =
               *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 | (uint)uVar10 >> (3 - uVar6) * 8;
          uVar7 = VStack_90._12_4_;
          VStack_90.pbmin = (uchar)uVar10;
          VStack_90.pbmax = SUB41(uVar10,1);
          VStack_90.reserved1 = SUB41(uVar10,2);
          VStack_90.reserved2 = SUB41(uVar10,3);
          auStack_c0[0] = VStack_90.pbmin;
          auStack_c0[1] = VStack_90.pbmax;
          auStack_c0[2] = VStack_90.reserved1;
          auStack_c0[3] = VStack_90.reserved2;
          puVar1 = auStack_bc + 3;
          uVar6 = (uint)puVar1 & 3;
          VStack_90._12_4_ = uVar7;
          *(uint *)(puVar1 + -uVar6) =
               *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 | (uint)uVar11 >> (3 - uVar6) * 8;
          uVar10 = VStack_90.reserved._4_4_;
          uVar9 = VStack_90.reserved._0_4_;
          uVar8 = VStack_90._20_4_;
          uVar7 = VStack_90._16_4_;
          VStack_90.adsr1 = (ushort)uVar11;
          VStack_90.adsr2 = SUB42(uVar11,2);
          auStack_bc._0_2_ = VStack_90.adsr1;
          auStack_bc._2_2_ = VStack_90.adsr2;
          puVar1 = auStack_b8 + 3;
          uVar6 = (uint)puVar1 & 3;
          VStack_90._16_4_ = uVar7;
          *(uint *)(puVar1 + -uVar6) =
               *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 |
               (uint)VStack_90._20_4_ >> (3 - uVar6) * 8;
          uVar7 = VStack_90._20_4_;
          VStack_90.prog = (short)uVar8;
          VStack_90.vag = SUB42(uVar8,2);
          auStack_b8._0_2_ = VStack_90.prog;
          auStack_b8._2_2_ = VStack_90.vag;
          puVar1 = auStack_b4 + 3;
          uVar6 = (uint)puVar1 & 3;
          VStack_90._20_4_ = uVar7;
          *(uint *)(puVar1 + -uVar6) =
               *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 | (uint)uVar9 >> (3 - uVar6) * 8;
          uVar7 = VStack_90.reserved._0_4_;
          VStack_90.reserved[0] = (short)uVar9;
          VStack_90.reserved[1] = SUB42(uVar9,2);
          auStack_b4._0_2_ = VStack_90.reserved[0];
          auStack_b4._2_2_ = VStack_90.reserved[1];
          puVar1 = auStack_b0 + 3;
          uVar6 = (uint)puVar1 & 3;
          VStack_90.reserved._0_4_ = uVar7;
          *(uint *)(puVar1 + -uVar6) =
               *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 | (uint)uVar10 >> (3 - uVar6) * 8;
          uVar7 = VStack_90.reserved._4_4_;
          VStack_90.reserved[2] = (short)uVar10;
          VStack_90.reserved[3] = SUB42(uVar10,2);
          auStack_b0._0_2_ = VStack_90.reserved[2];
          auStack_b0._2_2_ = VStack_90.reserved[3];
          VStack_90.reserved._4_4_ = uVar7;
          (*(code *)(&DAT_800cbc7c)[bVar2])(uVar4,uVar5,iVar15,uVar12);
          iVar15 = iVar15 + 1;
        } while (iVar15 < (int)(uint)local_a0.tones);
        CC_6_OBJ_3C0((uint)(ushort)param_1 << 0x10);
        return;
      }
    }
    else {
      bVar2 = *(byte *)(iVar14 + 0x1a);
      uVar4 = *(undefined1 *)(iVar14 + 0x26);
      uVar5 = *(undefined1 *)(uVar16 + iVar14 + 0x37);
      uStack_a8 = (uint)param_3;
      uStack_ac = (uint)bVar2;
      puVar1 = auStack_c8 + 3;
      uVar16 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar16) =
           *(uint *)(puVar1 + -uVar16) & -1 << (uVar16 + 1) * 8 |
           (uint)VStack_90._4_4_ >> (3 - uVar16) * 8;
      auStack_c8[0] = VStack_90.center;
      auStack_c8[1] = VStack_90.shift;
      auStack_c8[2] = VStack_90.min;
      auStack_c8[3] = VStack_90.max;
      puVar1 = auStack_c4 + 3;
      uVar16 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar16) =
           *(uint *)(puVar1 + -uVar16) & -1 << (uVar16 + 1) * 8 |
           (uint)VStack_90._8_4_ >> (3 - uVar16) * 8;
      auStack_c4[0] = VStack_90.vibW;
      auStack_c4[1] = VStack_90.vibT;
      auStack_c4[2] = VStack_90.porW;
      auStack_c4[3] = VStack_90.porT;
      puVar1 = auStack_c0 + 3;
      uVar16 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar16) =
           *(uint *)(puVar1 + -uVar16) & -1 << (uVar16 + 1) * 8 |
           (uint)VStack_90._12_4_ >> (3 - uVar16) * 8;
      auStack_c0[0] = VStack_90.pbmin;
      auStack_c0[1] = VStack_90.pbmax;
      auStack_c0[2] = VStack_90.reserved1;
      auStack_c0[3] = VStack_90.reserved2;
      puVar1 = auStack_bc + 3;
      uVar16 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar16) =
           *(uint *)(puVar1 + -uVar16) & -1 << (uVar16 + 1) * 8 |
           (uint)VStack_90._16_4_ >> (3 - uVar16) * 8;
      auStack_bc._0_2_ = VStack_90.adsr1;
      auStack_bc._2_2_ = VStack_90.adsr2;
      puVar1 = auStack_b8 + 3;
      uVar16 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar16) =
           *(uint *)(puVar1 + -uVar16) & -1 << (uVar16 + 1) * 8 |
           (uint)VStack_90._20_4_ >> (3 - uVar16) * 8;
      auStack_b8._0_2_ = VStack_90.prog;
      auStack_b8._2_2_ = VStack_90.vag;
      puVar1 = auStack_b4 + 3;
      uVar16 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar16) =
           *(uint *)(puVar1 + -uVar16) & -1 << (uVar16 + 1) * 8 |
           (uint)VStack_90.reserved._0_4_ >> (3 - uVar16) * 8;
      auStack_b4._0_2_ = VStack_90.reserved[0];
      auStack_b4._2_2_ = VStack_90.reserved[1];
      puVar1 = auStack_b0 + 3;
      uVar16 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar16) =
           *(uint *)(puVar1 + -uVar16) & -1 << (uVar16 + 1) * 8 |
           (uint)VStack_90.reserved._4_4_ >> (3 - uVar16) * 8;
      auStack_b0._0_2_ = VStack_90.reserved[2];
      auStack_b0._2_2_ = VStack_90.reserved[3];
      (*(code *)(&DAT_800cbc7c)[bVar2])(uVar4,uVar5,cVar3,VStack_90._0_4_);
    }
    uVar12 = _SsReadDeltaValue((int)param_1,(int)param_2);
    *(undefined4 *)(iVar14 + 0x90) = uVar12;
    *(undefined1 *)(iVar14 + 0x1f) = 0;
    CC_6_OBJ_3F4();
    return;
  }
  if ((*(char *)(iVar14 + 0x19) == '\0') && (iVar15 = 0, local_a0.tones != 0)) {
    local_70 = (uint)param_3 * 0x1900;
    iVar17 = 0;
    do {
      sVar13 = (short)((uint)iVar17 >> 0x10);
      SsUtGetVagAtr((ushort)*(byte *)(iVar14 + 0x26),(ushort)*(byte *)(uVar16 + iVar14 + 0x37),
                    sVar13,&VStack_90);
      bVar2 = *(byte *)(iVar14 + 0x18);
      if (bVar2 == 1) {
        CC_6_OBJ_1BC();
        return;
      }
      if (bVar2 < 2) {
        if (bVar2 == 0) {
          VStack_90._12_4_ = CONCAT31(CONCAT21(VStack_90._14_2_,param_3),param_3) & 0xffff7f7f;
          CC_6_OBJ_1BC();
          return;
        }
        CC_6_OBJ_1C0();
        return;
      }
      if (bVar2 != 2) {
        CC_6_OBJ_1C0();
        return;
      }
      iVar15 = iVar15 + 1;
      SsUtSetVagAtr((ushort)*(byte *)(iVar14 + 0x26),(ushort)*(byte *)(uVar16 + iVar14 + 0x37),
                    sVar13,&VStack_90);
      iVar17 = iVar17 + 0x10000;
    } while (iVar15 < (int)(uint)local_a0.tones);
  }
  uVar12 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar14 + 0x90) = uVar12;
  *(undefined1 *)(iVar14 + 0x1e) = 0;
  CC_6_OBJ_3F4();
  return;
}


