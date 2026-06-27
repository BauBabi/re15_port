/* FUN_8003a92c @ 0x8003a92c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a92c(void)

{
  int iVar1;
  int iVar2;
  undefined *puVar3;
  int iVar4;
  
  iVar1 = DAT_800ea23c;
  iVar4 = 8;
  iVar2 = 0x60;
  puVar3 = &UNK_800cc248;
  do {
    iVar2 = iVar2 + -0xc;
    SetDrawMode((DR_MODE *)(iVar1 + (uint)DAT_800ce5e0 * 0x60 + 0x19c0 + iVar2),0,0,0x85,(RECT *)0x0
               );
    iVar4 = iVar4 + -1;
    AddPrim(puVar3 + (uint)DAT_800ce5e0 * 0x1000,
            (void *)(iVar1 + (uint)DAT_800ce5e0 * 0x60 + 0x19c0 + iVar2));
    puVar3 = puVar3 + -4;
  } while (iVar4 != 0);
  return;
}


