int CD_init(void)

{
  int iVar1;
  int iVar2;
  
  puts("CD_init:");
  printf("addr=%08x\n",&PTR_DAT_80078b84);
  DAT_800788c5 = 0;
  DAT_800788c4 = 0;
  DAT_800788a8 = 0;
  DAT_800788a4 = 0;
  DAT_800788b8 = 0;
  _DAT_800788b4 = 0;
  ResetCallback();
  InterruptCallback(2,callback);
  if ((CDROM_REG3 & 7) != 0) {
    do {
    } while( true );
  }
  DAT_80078b82 = 0;
  DAT_80078b81 = 0;
  DAT_80078b80 = 2;
  CDROM_REG0 = 0;
  CDROM_REG3 = 0;
  COMMON_DELAY = 0x1325;
  CD_cw(1,0,0,0);
  if ((_DAT_800788b4 & 0x10) != 0) {
    CD_cw(1,0,0,0);
  }
  iVar1 = CD_cw(10,0,0,0);
  iVar2 = -1;
  if (iVar1 == 0) {
    iVar1 = CD_cw(0xc,0,0,0);
    iVar2 = -1;
    if (iVar1 == 0) {
      iVar2 = CD_sync(0,0);
      iVar2 = -(uint)(iVar2 != 2);
    }
  }
  return iVar2;
}
