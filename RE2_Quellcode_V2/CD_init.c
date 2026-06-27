/* CD_init @ 0x800885c8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int CD_init(void)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  
  FUN_8009881c("CD_init:");
  printf("addr=%08x\n",&PTR_DAT_800ad0d0);
  DAT_800ace0d = 0;
  DAT_800ace0c = 0;
  DAT_800acdf0 = 0;
  DAT_800acdec = 0;
  DAT_800ace00 = 0;
  _DAT_800acdfc = 0;
  ResetCallback();
  InterruptCallback(2,callback);
  *PTR_CDROM_REG0_800ad0b4 = 1;
  bVar1 = *PTR_CDROM_REG3_800ad0c0;
  while ((bVar1 & 7) != 0) {
    *PTR_CDROM_REG0_800ad0b4 = 1;
    *PTR_CDROM_REG3_800ad0c0 = 7;
    *PTR_CDROM_REG2_800ad0bc = 7;
    bVar1 = *PTR_CDROM_REG3_800ad0c0;
  }
  DAT_800ad0ce = 0;
  DAT_800ad0cd = 0;
  DAT_800ad0cc = 2;
  *PTR_CDROM_REG0_800ad0b4 = 0;
  *PTR_CDROM_REG3_800ad0c0 = 0;
  *(undefined4 *)PTR_COMMON_DELAY_800ad0c4 = 0x1325;
  CD_cw(1,0,0,0);
  if ((_DAT_800acdfc & 0x10) != 0) {
    CD_cw(1,0,0,0);
  }
  iVar2 = CD_cw(10,0,0,0);
  iVar3 = -1;
  if (iVar2 == 0) {
    iVar2 = CD_cw(0xc,0,0,0);
    iVar3 = -1;
    if (iVar2 == 0) {
      iVar3 = CD_sync(0,0);
      iVar3 = -(uint)(iVar3 != 2);
    }
  }
  return iVar3;
}


