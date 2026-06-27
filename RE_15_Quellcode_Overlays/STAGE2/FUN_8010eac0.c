/* FUN_8010eac0 @ 0x8010eac0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eac0(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d0) == '\0') {
    FUN_801108cc(0xfffff830,0x9c4,0xfffffb50);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffffc18,0x9c4,0x4b0);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffff830,0x9c4,0);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffffc18,0x44c,0xfffffb50);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffff830,0x44c,0x4b0);
    FUN_80110988(0x3000,0,7);
    FUN_801108cc(0xfffffc18,0x44c,0);
    FUN_80110988(0x3000,0,7);
  }
  *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
  if (10 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


