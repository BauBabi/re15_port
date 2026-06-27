/* FUN_8010ec44 @ 0x8010ec44  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ec44(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(200,0x708,0);
    FUN_80110988(0x2000,0xffffff38,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x03') {
    FUN_801108cc(400,0x708,0xfffffda8);
    FUN_80110988(0x2600,0xffffff9c,0);
    FUN_801108cc(400,0x960,0);
    FUN_80110988(0x2600,0xffffff9c,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\x06') {
    FUN_801108cc(600,0x708,0xfffffb50);
    FUN_80110988(0x2c00,0,0);
    FUN_801108cc(600,0x960,0xfffffda8);
    FUN_80110988(0x3000,0,0);
    FUN_801108cc(600,3000,0);
    FUN_80110988(0x2c00,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\t') {
    FUN_801108cc(800,0x960,0xfffffb50);
    FUN_80110988(0x3200,0,0);
    FUN_801108cc(800,3000,0xfffffda8);
    FUN_80110988(0x3200,0,0);
  }
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\f') {
    FUN_801108cc(1000,3000,0xfffffb50);
    FUN_80110988(0x3800,0,0);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (0x19 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  return;
}


