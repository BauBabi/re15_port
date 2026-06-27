/* FUN_80076498 @ 0x80076498  (Ghidra headless, raw MIPS overlay) */

void FUN_80076498(void)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar5 = 3;
  FUN_8008de04(0x72,DAT_800d5c3e + 0x46);
  iVar6 = 0x10;
  FUN_80076cb0(&DAT_800ab1dc,&DAT_800ab1e8);
  DAT_800ab1e0 = 0x226 - DAT_800d5c1e;
  iVar7 = 0xd98;
  iVar8 = 0x1e;
  do {
    iVar5 = iVar5 + -1;
    FUN_80076640(&UNK_800d57d0 + iVar7,0,iVar5);
    iVar4 = 8;
    iVar3 = iVar8 + -3;
    do {
      iVar4 = iVar4 + -1;
      if (iVar5 == DAT_800d5c01) {
        iVar1 = iVar3 * 8;
        if (iVar4 != DAT_800d5c02) goto LAB_80076598;
        iVar1 = iVar3 * 0x74;
        uVar2 = 0x1e9;
      }
      else {
        iVar1 = iVar3 << 3;
LAB_80076598:
        iVar1 = (iVar1 * 4 + iVar3 * -3) * 4;
        uVar2 = 0x1ea;
      }
      FUN_80017054(&UNK_800d5ccc + iVar1,0x17,0,uVar2);
      if ((&DAT_800d4b68)[iVar6 + iVar4] != -1) {
        FUN_80076640(&UNK_800d5ccc + iVar3 * 0x74,1,iVar4);
      }
      iVar3 = iVar3 + -1;
    } while (iVar4 != 0);
    iVar6 = iVar6 + -8;
    iVar7 = iVar7 + -0x488;
    iVar8 = iVar8 + -10;
    if (iVar5 == 0) {
      return;
    }
  } while( true );
}


