void start(void)

{
  undefined4 *puVar1;
  undefined4 unaff_retaddr;
  
  puVar1 = &DAT_8008f5cc;
  do {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  } while (puVar1 < &UNK_800bee84);
  DAT_8008f5cc = unaff_retaddr;
  InitHeap((ulong *)&DAT_800bee88,0x13917c);
  main();
  trap(1);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}
