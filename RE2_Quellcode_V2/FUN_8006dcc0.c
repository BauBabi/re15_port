/* FUN_8006dcc0 @ 0x8006dcc0  (Ghidra headless, raw MIPS overlay) */

void FUN_8006dcc0(void)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  undefined *puVar4;
  int iVar5;
  undefined *p;
  int iVar6;
  
  p = &DAT_8019c000;
  iVar6 = 0xe;
  uVar3 = (uint)DAT_800ce5e0;
  if (uVar3 != 0) {
    p = &DAT_8019c014;
  }
  puVar4 = p + 6;
  iVar5 = 0x70;
  do {
    iVar6 = iVar6 + -1;
    if (DAT_800d5c19 == '\0') {
      puVar4[-2] = 0x50;
      puVar4[-1] = 0x50;
      *puVar4 = 0x50;
    }
    else {
      puVar4[-2] = 0x80;
      puVar4[-1] = 0x80;
      *puVar4 = 0x80;
    }
    iVar2 = FUN_80077360(&DAT_800d4924,DAT_800d5c0a);
    if (((iVar2 != 0) && ((&UNK_800a9b15)[iVar5] == DAT_800d5c0a)) &&
       (iVar2 = FUN_80077360(&DAT_800d4a34,(&UNK_800a9b14)[iVar5]), iVar2 == 0)) {
      if ((DAT_800cfbd8 & 0x40000000) == 0) {
        bVar1 = (&UNK_800a9b16)[iVar5] & 0x40;
      }
      else {
        if (((int)DAT_800cfbd8 < 0) && (((&UNK_800a9b16)[iVar5] & 0x40) != 0)) goto LAB_8006de38;
        bVar1 = (&UNK_800a9b16)[iVar5] & 0x80;
      }
      if (bVar1 == 0) {
        AddPrim(&UNK_800cc1f8 + uVar3 * 0x20,p);
      }
    }
LAB_8006de38:
    puVar4 = puVar4 + 0x28;
    p = p + 0x28;
    iVar5 = iVar5 + -8;
    if (iVar6 == 0) {
      AddPrim(&UNK_800cc1f8 + uVar3 * 0x20,&DAT_800d6bc0 + (uint)DAT_800ce5e0 * 0xc);
      return;
    }
  } while( true );
}


