/* startIntr @ 0x8008621c  (Ghidra headless, raw MIPS overlay) */

undefined2 * startIntr(void)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined2 *puVar3;
  int iVar4;
  undefined4 uVar5;
  
  puVar2 = PTR_I_MASK_800accd4;
  puVar1 = PTR_I_STAT_800accd0;
  puVar3 = (undefined2 *)0x0;
  if (DAT_800abc44 == 0) {
    *(undefined2 *)PTR_I_MASK_800accd4 = 0;
    *(undefined2 *)puVar1 = *(undefined2 *)puVar2;
    *(undefined4 *)PTR_DMA_DPCR_800accd8 = 0x33333333;
    memclr(&DAT_800abc44,0x41a);
    iVar4 = setjmp(&DAT_800abc7c);
    if (iVar4 != 0) {
      trapIntr();
    }
    DAT_800abc80 = &DAT_800acc5c;
    HookEntryInt(&DAT_800abc7c);
    DAT_800abc44 = 1;
    uVar5 = startIntrVSync();
    *(undefined4 *)(PTR_PTR_800acccc + 0x14) = uVar5;
    uVar5 = startIntrDMA();
    *(undefined4 *)(PTR_PTR_800acccc + 4) = uVar5;
    FUN_800956bc();
    puVar3 = &DAT_800abc44;
    FUN_800957b4();
  }
  return puVar3;
}


