void stup0(void)

{
  main();
  trap(1);
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}
