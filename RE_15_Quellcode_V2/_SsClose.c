void _SsClose(short param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  
  uVar6 = (uint)param_1;
  SpuVmSetSeqVol(uVar6,0,0,1);
  SpuVmSeqKeyOff(uVar6);
  DAT_800b52e4 = ~(1 << (uVar6 & 0x1f)) & DAT_800b52e4;
  iVar5 = 0;
  if (0 < DAT_800bbd8c) {
    piVar4 = &DAT_800bb500 + uVar6;
    iVar3 = 0;
    do {
      *(undefined4 *)(iVar3 + *piVar4 + 0x90) = 0;
      *(undefined1 *)(iVar3 + *piVar4 + 0x3c) = 0xff;
      *(undefined1 *)(iVar3 + *piVar4) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x3e) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x40) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0x94) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0x98) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x42) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0xa4) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0xa0) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0x9c) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x44) = 0;
      iVar5 = iVar5 + 1;
                    /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
                       setShadeTex(sprt16, 1) */
      *(undefined2 *)(iVar3 + *piVar4 + 0x74) = 0x7f;
      iVar1 = (int)DAT_800bbd8c;
      iVar2 = iVar3 + *piVar4;
      iVar3 = iVar3 + 0xac;
      *(undefined2 *)(iVar2 + 0x76) = 0x7f;
    } while (iVar5 < iVar1);
  }
  return;
}
