/* FUN_80075e98 @ 0x80075e98  (Ghidra headless, raw MIPS overlay) */

void FUN_80075e98(void)

{
  uint uVar1;
  short *psVar2;
  int iVar3;
  int iVar4;
  undefined *p;
  char cVar5;
  char cVar6;
  short sVar7;
  
  p = &DAT_8019c000;
  uVar1 = (uint)DAT_800ce5e0;
  if (uVar1 != 0) {
    p = &DAT_8019c014;
  }
  psVar2 = (short *)(p + 10);
  sVar7 = 0x187;
  cVar6 = '8';
  cVar5 = '0';
  iVar3 = 2;
  do {
    sVar7 = sVar7 + -0xba;
    cVar6 = cVar6 + -0x10;
    iVar4 = iVar3 + -1;
    cVar5 = cVar5 + -0x10;
    if ((uint)DAT_800d5bf2 == iVar3 + 3U) {
      *(char *)(psVar2 + 1) = cVar5;
    }
    else {
      *(char *)(psVar2 + 1) = cVar6;
    }
    *(undefined1 *)((int)psVar2 + 3) = 0x88;
    psVar2[-1] = DAT_800d5c3c + sVar7 + -10;
    *psVar2 = DAT_800d5c3e + 0x36;
    AddPrim(&UNK_800cc1fc + uVar1 * 0x20,p);
    psVar2 = psVar2 + 0x14;
    iVar3 = iVar4;
    p = p + 0x28;
  } while (iVar4 != 0);
  AddPrim(&UNK_800cc1fc + uVar1 * 0x20,&UNK_800d6ba8 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


