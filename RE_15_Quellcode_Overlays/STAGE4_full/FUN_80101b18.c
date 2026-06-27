/* FUN_80101b18 @ 0x80101b18  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101b18(void)

{
  short sVar1;
  short sVar2;
  short sVar3;
  uint uVar4;
  
  sVar2 = func_0x8001a9cc(&DAT_800aca88,0x2c8);
  sVar3 = func_0x8001a9cc(&DAT_800aca88,0x5f4);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
  }
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) && (sVar2 != 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  if ((((sVar3 == 0) && ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x10) != 0)) &&
      (*(undefined4 *)(_DAT_800ac784 + 4) = 0x201, 10000 < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
     (uVar4 = func_0x8001af20(), (uVar4 & 7) == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x801;
  }
  return;
}


