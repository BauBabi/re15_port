void _patch_gte(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  code *pcVar4;
  undefined4 unaff_retaddr;
  
  DAT_8008f5d0 = unaff_retaddr;
  FUN_8006e170();
  iVar1 = (*(code *)&SUB_000000b0)();
  pcVar4 = (code *)&PATCHGTE_OBJ_68;
  puVar2 = *(undefined4 **)(iVar1 + 0x18);
  do {
    uVar3 = *(undefined4 *)pcVar4;
    pcVar4 = pcVar4 + 4;
    *puVar2 = uVar3;
    puVar2 = puVar2 + 1;
  } while (pcVar4 != FUN_80066d60);
  FlushCache();
  FUN_8006e468();
  return;
}
