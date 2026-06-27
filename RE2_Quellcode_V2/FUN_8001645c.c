/* FUN_8001645c @ 0x8001645c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001645c(void)

{
  int iVar1;
  undefined *puVar2;
  
  puVar2 = &UNK_800ce60c;
  iVar1 = 5;
  do {
    puVar2[0xe] = 0;
    iVar1 = iVar1 + -1;
    puVar2 = puVar2 + 0x1c;
  } while (iVar1 != 0);
  return;
}


