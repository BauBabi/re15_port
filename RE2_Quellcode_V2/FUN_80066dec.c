/* FUN_80066dec @ 0x80066dec  (Ghidra headless, raw MIPS overlay) */

void FUN_80066dec(int param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  int iVar2;
  
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
    if ((*(ushort *)(param_1 + 0x1cc) & 0x100) != 0) {
      FUN_80015e7c(param_1,param_2,param_3,(*(ushort *)(param_1 + 0x1cc) & 0x80) != 0);
    }
  }
  if ((**(uint **)(param_1 + 0x178) & 0x4000) != 0) {
    FUN_8005c040(0,((**(uint **)(param_1 + 0x178) & 0x2000) >> 0xd) * 3 + 4,param_1);
    DAT_800cfbf6 = DAT_800cfbf6 | 2;
  }
  FUN_80015e7c(param_1,param_2,param_3,(*(ushort *)(param_1 + 0x1cc) & 0x80) != 0);
  iVar2 = FUN_8002959c(param_1,param_2,param_3,
                       (uint)((*(ushort *)(param_1 + 0x1cc) & 0x80) != 0) << 0x10 | 0x200);
  if (iVar2 == 0) {
    FUN_80015350(param_1,0,0);
    if ((*(ushort *)(param_1 + 0x1cc) & 8) != 0) {
      iVar2 = FUN_8002959c(param_1,param_2,param_3,
                           (uint)((*(ushort *)(param_1 + 0x1cc) & 0x80) != 0) << 0x10 | 0x200);
      if (iVar2 != 0) {
        *(undefined1 *)(param_1 + 6) = 2;
        if ((*(ushort *)(param_1 + 0x1cc) & 4) != 0) {
          *(undefined1 *)(param_1 + 6) = 1;
        }
        *(short *)(param_1 + 0x76) = *(short *)(param_1 + 0x76) + *(short *)(param_1 + 0x1d6);
      }
      FUN_80015350(param_1,0,0);
    }
    *(short *)(param_1 + 0x76) = *(short *)(param_1 + 0x76) + *(short *)(param_1 + 0x1d4);
  }
  else {
    *(undefined1 *)(param_1 + 6) = 2;
    if ((*(ushort *)(param_1 + 0x1cc) & 4) != 0) {
      *(undefined1 *)(param_1 + 6) = 1;
    }
    *(short *)(param_1 + 0x76) = *(short *)(param_1 + 0x76) + *(short *)(param_1 + 0x1d6);
    FUN_80015350(param_1,0,0);
  }
  return;
}


