void FUN_800264e8(void)

{
  int iVar1;
  
  FUN_8004ee60(&DAT_801ff404,0x66);
  DAT_800b5456 = 0;
  FUN_80043850(0x140,0xf0,1);
  FUN_80013b60(0x23,&DAT_80198000,"MEMORY CARD BG");
  FUN_800217b0(0x200,0xffffe800,7,0);
  FUN_800216ec(0,0,0xffffff,0);
  while (iVar1 = FUN_8002178c(0), iVar1 == 0) {
    FUN_80029ac8(1);
  }
  FUN_80029ac8(2);
  return;
}
