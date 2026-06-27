/* FUN_80102c8c @ 0x80102c8c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102c8c(void)

{
  int iVar1;
  
  if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
      (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar1 == 0)) &&
     (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
    iVar1 = func_0x8001a780(&DAT_800aca54);
    *(uint *)(_DAT_800ac784 + 4) = (iVar1 + 1) * 0x100 | 1;
  }
  if (((*(ushort *)(_DAT_800ac784 + 0x1c4) & 0x2000) != 0) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x80) == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
  }
  return;
}


