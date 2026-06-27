/* FUN_800669b0 @ 0x800669b0  (Ghidra headless, raw MIPS overlay) */

void FUN_800669b0(int param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  
  bVar1 = *(byte *)(param_1 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(param_1 + 6) = 1;
    *(undefined1 *)(param_1 + 0x14e) = 7;
    if ((*(ushort *)(param_1 + 0x1cc) & 0x40) != 0) {
      *(undefined1 *)(param_1 + 0x14e) = 0;
    }
  }
  iVar3 = FUN_8002959c(param_1,param_2,param_3,
                       (uint)((*(ushort *)(param_1 + 0x1cc) & 0x80) != 0) << 0x10 | 0x200);
  if (iVar3 == 0) {
    if (((*(ushort *)(param_1 + 0x1cc) & 8) != 0) &&
       (iVar3 = FUN_8002959c(param_1,param_2,param_3,
                             (uint)((*(ushort *)(param_1 + 0x1cc) & 0x80) != 0) << 0x10 | 0x200),
       iVar3 != 0)) {
      *(undefined1 *)(param_1 + 6) = 2;
      if ((*(ushort *)(param_1 + 0x1cc) & 4) != 0) {
        *(undefined1 *)(param_1 + 6) = 1;
      }
      *(short *)(param_1 + 0x76) = *(short *)(param_1 + 0x76) + *(short *)(param_1 + 0x1d6);
    }
    sVar2 = *(short *)(param_1 + 0x76) + *(short *)(param_1 + 0x1d4);
  }
  else {
    *(undefined1 *)(param_1 + 6) = 2;
    if ((*(ushort *)(param_1 + 0x1cc) & 4) != 0) {
      *(undefined1 *)(param_1 + 6) = 1;
    }
    sVar2 = *(short *)(param_1 + 0x76) + *(short *)(param_1 + 0x1d6);
  }
  *(short *)(param_1 + 0x76) = sVar2;
  return;
}


