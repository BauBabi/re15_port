/* stup1 @ 0x8007842c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Control flow encountered bad instruction data */

void stup1(void)

{
  undefined4 unaff_retaddr;
  
  DAT_800c3a48 = unaff_retaddr;
  InitHeap((ulong *)&DAT_800eaeac,(DAT_800784b8 - DAT_800abbcc) - 0xeaea8);
  main();
  trap(1);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


