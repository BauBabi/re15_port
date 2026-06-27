/* FUN_80031af4 @ 0x80031af4  (Ghidra headless, raw MIPS overlay) */

void FUN_80031af4(void)

{
  u_short uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int iVar4;
  DR_MODE *p;
  int iVar5;
  
  p = (DR_MODE *)&DAT_800e2ab0;
  iVar5 = 0x10;
  do {
    uVar1 = GetTPage(0,0,0x300,0x100);
    SetDrawMode(p,0,0,(uint)uVar1,(RECT *)0x0);
    uVar1 = GetTPage(0,0,0x340,0x100);
    SetDrawMode(p + 1,0,0,(uint)uVar1,(RECT *)0x0);
    iVar5 = iVar5 + -1;
    p = p + 2;
  } while (iVar5 != 0);
  puVar2 = &DAT_800e2c30;
  puVar3 = &DAT_800e5430;
  iVar5 = 0x200;
  iVar4 = 0x300;
  do {
    *puVar2 = 0x4000000;
    puVar2[1] = 0x64808080;
    iVar5 = iVar5 + -1;
    puVar2 = puVar2 + 5;
  } while (iVar5 != 0);
  do {
    *puVar3 = 0x3000000;
    puVar3[1] = 0x74808080;
    iVar4 = iVar4 + -1;
    puVar3 = puVar3 + 4;
  } while (iVar4 != 0);
  FUN_80031ac4();
  FUN_80031bf0();
  return;
}


