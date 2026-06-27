void BIOS_OBJ_428(void)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  
  puVar3 = &DAT_8008fe08;
  puVar4 = &stack0x00000018;
  DAT_80078b82 = 4;
  DAT_80078b81 = 4;
  iVar2 = 7;
  do {
    uVar1 = *puVar4;
    puVar4 = puVar4 + 1;
    iVar2 = iVar2 + -1;
    *puVar3 = uVar1;
    puVar3 = puVar3 + 1;
  } while (iVar2 != -1);
  puVar3 = &DAT_8008fe00;
  puVar4 = &stack0x00000018;
  iVar2 = 7;
  do {
    uVar1 = *puVar4;
    puVar4 = puVar4 + 1;
    iVar2 = iVar2 + -1;
    *puVar3 = uVar1;
    puVar3 = puVar3 + 1;
  } while (iVar2 != -1);
  BIOS_OBJ_560();
  return;
}
