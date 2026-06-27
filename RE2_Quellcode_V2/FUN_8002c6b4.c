/* FUN_8002c6b4 @ 0x8002c6b4  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8002c6b4(int param_1)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  undefined4 uVar4;
  char *pcVar5;
  char *pcVar6;
  
  if (DAT_800ce33c[0x16] == DAT_800cfbf2) {
    pcVar5 = DAT_800ce33c + 0x16;
    pcVar2 = DAT_800ce33c;
    do {
      pcVar6 = pcVar2 + 0x14;
      if ((*pcVar6 != '\0') &&
         (((pcVar5[-1] == -1 || (pcVar5[-1] == DAT_800cfcfe)) &&
          (iVar3 = FUN_8002c820(&DAT_800cfc30,pcVar6), iVar3 != 0)))) {
        cVar1 = pcVar2[0x17];
        goto LAB_8002c77c;
      }
      pcVar5 = pcVar5 + 0x14;
      pcVar2 = pcVar6;
    } while (*pcVar5 == DAT_800cfbf2);
  }
  uVar4 = 0;
  cVar1 = DAT_800cfbf2;
  if (param_1 != 0) {
LAB_8002c77c:
    FUN_8002c7ac(cVar1);
    uVar4 = 1;
  }
  return uVar4;
}


