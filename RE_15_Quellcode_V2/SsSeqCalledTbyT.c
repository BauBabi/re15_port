void SsSeqCalledTbyT(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  
  if (DAT_800b283c != 1) {
    DAT_800b283c = 1;
    uVar6 = 0;
    SpuVmFlush();
    if (0 < DAT_800bbd84) {
      piVar7 = &DAT_800bb500;
      do {
        if (((1 << (uVar6 & 0x1f) & DAT_800b52e4) != 0) && (iVar5 = 0, 0 < DAT_800bbd8c)) {
          iVar4 = (int)(short)uVar6;
          iVar3 = 0;
          iVar1 = 0;
          do {
            iVar2 = iVar3 >> 0x10;
            if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 1) != 0) {
              FUN_80060d8c(iVar4,iVar2);
              if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 0x10) != 0) {
                _SsSndCrescendo(iVar4,iVar2);
              }
              if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 0x20) != 0) {
                _SsSndDecrescendo(iVar4,iVar2);
              }
              if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 0x40) != 0) {
                _SsSndTempo(iVar4,iVar2);
              }
              if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 0x80) != 0) {
                _SsSndTempo(iVar4,iVar2);
              }
            }
            if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 2) != 0) {
              _SsSndPause(iVar4,iVar2);
            }
            if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 8) != 0) {
              _SsSndReplay(iVar4,iVar2);
            }
            if ((*(uint *)(iVar1 + *piVar7 + 0x90) & 4) != 0) {
              _SsSndStop(uVar6,iVar5);
              *(undefined4 *)(iVar1 + *piVar7 + 0x90) = 0;
            }
            iVar3 = iVar3 + 0x10000;
            iVar5 = iVar5 + 1;
            iVar1 = iVar1 + 0xac;
          } while (iVar5 < DAT_800bbd8c);
        }
        uVar6 = uVar6 + 1;
        piVar7 = piVar7 + 1;
      } while ((int)uVar6 < (int)DAT_800bbd84);
    }
    DAT_800b283c = 0;
  }
  return;
}
