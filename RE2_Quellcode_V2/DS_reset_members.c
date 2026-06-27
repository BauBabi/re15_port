/* DS_reset_members @ 0x8008af04  (Ghidra headless, raw MIPS overlay) */

void DS_reset_members(void)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  
  iVar2 = 3;
  puVar3 = &DAT_800ad240;
  DAT_800ad238 = 0;
  DAT_800ad23c._0_1_ = 0;
  do {
    *(undefined1 *)puVar3 = 0;
    iVar2 = iVar2 + -1;
    puVar3 = (undefined4 *)((int)puVar3 + -1);
  } while (-1 < iVar2);
  DAT_800ad244 = 0;
  iVar2 = 7;
  puVar1 = &DAT_800ad24f;
  do {
    *puVar1 = 0;
    iVar2 = iVar2 + -1;
    puVar1 = puVar1 + -1;
  } while (-1 < iVar2);
  DAT_800ad250 = 0;
  DAT_800ad254 = 2;
  DAT_800ad258 = 0xe;
  DAT_800ad25c = 0x15;
  DAT_800ad260 = 0;
  DAT_800ad264._0_1_ = 0;
  DAT_800ad264._1_1_ = 0;
  DsIntToPos(0,(DslLOC *)((int)&DAT_800ad264 + 2));
  DAT_800ad268._2_1_ = 0;
  DAT_800ad268._3_1_ = 0;
  DAT_800ad26c = 0;
  DAT_800ad26d = 0;
  DAT_800ad26e = 0;
  DAT_800ad26f = 0;
  DAT_800ad270 = 0;
  DAT_800ad274 = 1;
  DAT_800ad278 = 0;
  DAT_800ad27c = 0;
  return;
}


