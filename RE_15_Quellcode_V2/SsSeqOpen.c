short SsSeqOpen(ulong *param_1,short param_2)

{
  bool bVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  uint uVar5;
  uint in_t0;
  
  if (DAT_800b52e4 == 0xffffffff) {
    printf("Can\'t Open Sequence data any more\n\n");
    sVar2 = SEQINIT_OBJ_3E0();
    return sVar2;
  }
  uVar5 = 0;
  bVar1 = false;
  do {
    if ((1 << (uVar5 & 0x1f) & DAT_800b52e4) == 0) {
      bVar1 = true;
      in_t0 = uVar5;
    }
    uVar5 = uVar5 + 1;
  } while (!bVar1);
  sVar3 = (short)in_t0;
  DAT_800b52e4 = 1 << ((int)sVar3 & 0x1fU) | DAT_800b52e4;
  sVar4 = _SsInitSoundSeq((int)sVar3,(int)param_2,param_1);
  sVar2 = -1;
  if (sVar4 != -1) {
    sVar2 = sVar3;
  }
  return sVar2;
}
