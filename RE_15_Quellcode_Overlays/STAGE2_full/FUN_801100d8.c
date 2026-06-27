/* FUN_801100d8 @ 0x801100d8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801100d8(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x640,0xfffffb50);
    FUN_80110988(0x2000,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,1000,0xfffffb50);
    FUN_80110988(0x2400,0,0);
    FUN_801108cc(400,0x640,0xfffffda8);
    FUN_80110988(0x2400,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,400,0xfffffb50);
    FUN_80110988(0x2800,0,0);
    FUN_801108cc(600,1000,0xfffffda8);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(600,0x640,0);
    FUN_80110988(0x2800,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,400,0xfffffda8);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(800,1000,0);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,400,0);
    FUN_80110988(0x3000,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x14 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}


