/* stup0 @ 0x800784a8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Control flow encountered bad instruction data */

void stup0(void)

{
  main();
  trap(1);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


