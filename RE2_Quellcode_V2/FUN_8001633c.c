/* FUN_8001633c @ 0x8001633c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001633c(void)

{
  u_short uVar1;
  POLY_FT4 *p;
  undefined *puVar2;
  undefined *puVar3;
  int iVar4;
  
  puVar3 = &UNK_800ce698;
  iVar4 = 0x32;
  puVar2 = &DAT_800ce6d8;
  do {
    p = (POLY_FT4 *)(puVar3 + 0x18);
    puVar2[-0x32] = 0;
    *(undefined4 *)(puVar2 + -0x24) = 0x808080;
    SetPolyFT4(p);
    SetSemiTrans(p,1);
    puVar3 = puVar3 + 0x68;
    iVar4 = iVar4 + -1;
    uVar1 = GetTPage(1,2,(DAT_800cfcfc - 0xe) * 0x40,0x100);
    *(u_short *)(puVar2 + -0x12) = uVar1;
    uVar1 = GetClut(0,DAT_800cfcfd + 0x1e2);
    *(u_short *)(puVar2 + -0x1a) = uVar1;
    puVar2[-0x1c] = 0x62;
    puVar2[-0x1b] = 0xe1;
    puVar2[-0x14] = 0x7e;
    puVar2[-0x13] = 0xe1;
    puVar2[-0xc] = 0x62;
    puVar2[-0xb] = 0xff;
    puVar2[-4] = 0x7e;
    puVar2[-3] = 0xff;
    FUN_80076a00(puVar2,p,0x28);
    puVar2 = puVar2 + 0x68;
  } while (iVar4 != 0);
  return;
}


