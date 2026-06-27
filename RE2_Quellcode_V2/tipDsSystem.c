/* tipDsSystem @ 0x8008bdd0  (Ghidra headless, raw MIPS overlay) */

void tipDsSystem(void)

{
  printf("*** STATUS of DS system ***\n");
  printf("\tcommand status: %d %d %d\n",DAT_800ad254,DAT_800ad258,DAT_800ad25c);
  printf("\tlast command: %02x %02x%02x%02x%02x\n",(uint)(byte)DAT_800ad23c,(uint)DAT_800ad23c._1_1_
         ,(uint)DAT_800ad23c._2_1_,(uint)DAT_800ad23c._3_1_,(uint)(byte)DAT_800ad240);
  printf("\tcommand execution: %d\n",DAT_800ad260);
  printf("\tsystem timer: %d %d\n",DAT_800ad278,DAT_800ad27c);
  printf("\tcallbacks vsync: %08x sync: %08x ready: %08x\n",DAT_800c3f08,DAT_800c3f0c,DAT_800c3f10);
  return;
}


