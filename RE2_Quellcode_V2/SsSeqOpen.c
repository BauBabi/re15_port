/* SsSeqOpen @ 0x8007a4fc  (Ghidra headless, raw MIPS overlay) */

short SsSeqOpen(ulong *param_1,short param_2)

{
  bool bVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  uint uVar5;
  uint uVar6;
  
  uVar6 = 0;
  if (DAT_800dcbd4 == 0xffffffff) {
    printf("Can\'t Open Sequence data any more\n\n");
    sVar2 = SSOPENQ_OBJ_30C();
    return sVar2;
  }
  uVar5 = 0;
  bVar1 = false;
  do {
    if ((DAT_800dcbd4 & 1 << (uVar5 & 0x1f)) == 0) {
      bVar1 = true;
      uVar6 = uVar5;
    }
    uVar5 = uVar5 + 1;
  } while (!bVar1);
  sVar3 = (short)uVar6;
  DAT_800dcbd4 = DAT_800dcbd4 | 1 << ((int)sVar3 & 0x1fU);
  sVar4 = _SsInitSoundSeq((int)sVar3,(int)param_2,param_1);
  DAT_800cbc38 = &LAB_8007c4f4;
  DAT_800cbc3c = _SsSetProgramChange;
  DAT_800cbc44 = _SsGetMetaEvent;
  DAT_800cbc40 = _SsSetPitchBend;
  DAT_800cbc48 = _SsSetControlChange;
  DAT_800cbc4c = _SsContBankChange;
  DAT_800cbc54 = _SsContMainVol;
  DAT_800cbc58 = _SsContPanpot;
  DAT_800cbc5c = _SsContExpression;
  DAT_800cbc60 = _SsContDamper;
  DAT_800cbc64 = _SsContNrpn1;
  DAT_800cbc68 = _SsContNrpn2;
  DAT_800cbc6c = _SsContRpn1;
  DAT_800cbc70 = _SsContRpn2;
  DAT_800cbc74 = _SsContExternal;
  DAT_800cbc78 = _SsContResetAll;
  DAT_800cbc50 = _SsContDataEntry;
  DAT_800cbc7c = _SsSetNrpnVabAttr0;
  DAT_800cbc80 = _SsSetNrpnVabAttr1;
  DAT_800cbc84 = _SsSetNrpnVabAttr2;
  DAT_800cbc88 = _SsSetNrpnVabAttr3;
  DAT_800cbc8c = _SsSetNrpnVabAttr4;
  DAT_800cbc90 = _SsSetNrpnVabAttr5;
  DAT_800cbc94 = _SsSetNrpnVabAttr6;
  DAT_800cbc98 = _SsSetNrpnVabAttr7;
  DAT_800cbc9c = _SsSetNrpnVabAttr8;
  DAT_800cbca0 = _SsSetNrpnVabAttr9;
  DAT_800cbca4 = _SsSetNrpnVabAttr10;
  DAT_800cbca8 = _SsSetNrpnVabAttr11;
  DAT_800cbcac = _SsSetNrpnVabAttr12;
  DAT_800cbcb0 = _SsSetNrpnVabAttr13;
  DAT_800cbcb4 = _SsSetNrpnVabAttr14;
  DAT_800cbcb8 = _SsSetNrpnVabAttr15;
  DAT_800cbcbc = _SsSetNrpnVabAttr16;
  DAT_800cbcc0 = &LAB_8007bfd0;
  DAT_800cbcc4 = &LAB_8007bff4;
  DAT_800cbcc8 = &LAB_8007c018;
  sVar2 = -1;
  if (sVar4 != -1) {
    sVar2 = sVar3;
  }
  return sVar2;
}


