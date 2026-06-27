/* start @ 0x80078408  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Control flow encountered bad instruction data */
/* Possible 2MBYTE.OBJ/stup2
   Possible 2MBYTE.OBJ/__SN_ENTRY_POINT */

void start(void)

{
  undefined4 *puVar1;
  undefined4 unaff_retaddr;
  
  puVar1 = &DAT_800c3a48;
  do {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  } while (puVar1 < &UNK_800eaea8);
  DAT_800c3a48 = unaff_retaddr;
  InitHeap((ulong *)&DAT_800eaeac,(DAT_800784b8 - DAT_800abbcc) - 0xeaea8);
  main();
  trap(1);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


