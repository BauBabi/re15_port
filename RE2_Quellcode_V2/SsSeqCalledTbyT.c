/* SsSeqCalledTbyT @ 0x8007d720  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void SsSeqCalledTbyT(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  
  if (DAT_800d7658 != 1) {
    DAT_800d7658 = 1;
    uVar6 = 0;
    FUN_80081f20();
    if (0 < DAT_800ead72) {
      piVar7 = &DAT_800ea250;
      do {
        if (((DAT_800dcbd4 & 1 << (uVar6 & 0x1f)) != 0) && (iVar5 = 0, 0 < DAT_800ead78)) {
          iVar4 = (int)(short)uVar6;
          iVar3 = 0;
          iVar1 = 0;
          do {
            iVar2 = iVar3 >> 0x10;
            if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 1) != 0) {
              FUN_8007e0fc(iVar4,iVar2);
              if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 0x10) != 0) {
                FUN_8007d994(iVar4,iVar2);
              }
              if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 0x20) != 0) {
                FUN_8007dcb4(iVar4,iVar2);
              }
              if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 0x40) != 0) {
                _SsSndTempo(iVar4,iVar2);
              }
              if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 0x80) != 0) {
                _SsSndTempo(iVar4,iVar2);
              }
            }
            if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 2) != 0) {
              _SsSndPause(iVar4,iVar2);
            }
            if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 8) != 0) {
              _SsSndReplay(iVar4,iVar2);
            }
            if ((*(uint *)(iVar1 + *piVar7 + 0x98) & 4) != 0) {
              FUN_8007e974(iVar4,iVar2);
              *(undefined4 *)(iVar1 + *piVar7 + 0x98) = 0;
            }
            iVar3 = iVar3 + 0x10000;
            iVar5 = iVar5 + 1;
            iVar1 = iVar1 + 0xb0;
          } while (iVar5 < DAT_800ead78);
        }
        uVar6 = uVar6 + 1;
        piVar7 = piVar7 + 1;
      } while ((int)uVar6 < (int)DAT_800ead72);
    }
    DAT_800d7658 = 0;
  }
  return;
}


