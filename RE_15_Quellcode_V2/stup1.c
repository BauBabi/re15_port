void stup1(void)

{
  undefined4 unaff_retaddr;
  
  DAT_8008f5cc = unaff_retaddr;
  InitHeap((ulong *)&DAT_800bee88,0x13917c);
  main();
  trap(1);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}
