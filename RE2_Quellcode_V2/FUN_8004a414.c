/* FUN_8004a414 @ 0x8004a414  (Ghidra headless, raw MIPS overlay) */

void FUN_8004a414(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  piVar1 = *(int **)(DAT_800ce324 + 0x60);
  if (piVar1 != (int *)0x0) {
    iVar7 = piVar1[1];
    iVar5 = *(int *)((int)piVar1 + *piVar1);
    iVar6 = 0;
    if (iVar7 != 0) {
      uVar4 = 0;
      do {
        iVar2 = **(int **)(DAT_800ce324 + 0x60);
        piVar1 = (int *)((int)*(int **)(DAT_800ce324 + 0x60) + iVar2 + uVar4);
        iVar3 = *piVar1;
        if (-1 < DAT_800cfb70) {
          if (iVar5 != iVar3) {
            iVar2 = iVar2 - iVar3;
            if (uVar4 != (iVar7 + -1) * 8) {
              iVar2 = piVar1[3] - iVar3;
            }
            FUN_80076a00(*(int *)(DAT_800ce324 + 0x60) + iVar5,*(int *)(DAT_800ce324 + 0x60) + iVar3
                         ,iVar2);
            piVar1 = *(int **)(DAT_800ce324 + 0x60);
            *(int *)((int)piVar1 + *piVar1 + iVar6 * 8) =
                 *(int *)((int)piVar1 + *piVar1 + uVar4) - (iVar3 - iVar5);
            piVar1 = *(int **)(DAT_800ce324 + 0x60);
            *(int *)((int)piVar1 + *piVar1 + iVar6 * 8 + 4) =
                 *(int *)((int)piVar1 + *piVar1 + uVar4 + 4) - (iVar3 - iVar5);
          }
          if (uVar4 == (iVar7 + -1) * 8) {
            iVar3 = **(int **)(DAT_800ce324 + 0x60) - iVar3;
          }
          else {
            iVar3 = *(int *)((int)*(int **)(DAT_800ce324 + 0x60) +
                            **(int **)(DAT_800ce324 + 0x60) + uVar4 + 8) - iVar3;
          }
          iVar5 = iVar5 + iVar3;
          iVar6 = iVar6 + 1;
        }
        uVar4 = uVar4 + 8;
        DAT_800cfb70 = DAT_800cfb70 << 1;
      } while (uVar4 < (uint)(iVar7 << 3));
    }
    piVar1 = *(int **)(DAT_800ce324 + 0x60);
    FUN_80076a00((int)piVar1 + iVar5,(int)piVar1 + *piVar1,iVar6 * 8);
    **(int **)(DAT_800ce324 + 0x60) = iVar5;
    *(int *)(*(int *)(DAT_800ce324 + 0x60) + 4) = iVar6;
    iVar7 = *(int *)(DAT_800ce324 + 8);
    iVar5 = *(int *)(DAT_800ce324 + 0x60) + iVar5 + iVar6 * 8;
    iVar6 = iVar7 - iVar5;
    FUN_80076a00(iVar5,iVar7,*(int *)(DAT_800ce324 + 0x10) - iVar7);
    *(int *)(DAT_800ce324 + 8) = *(int *)(DAT_800ce324 + 8) - iVar6;
    *(int *)(DAT_800ce324 + 0xc) = *(int *)(DAT_800ce324 + 0xc) - iVar6;
    DAT_800ce32c = *(int *)(DAT_800ce324 + 0x10) - iVar6;
    FUN_8005a338();
  }
  return;
}


