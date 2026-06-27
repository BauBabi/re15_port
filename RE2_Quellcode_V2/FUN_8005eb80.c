/* FUN_8005eb80 @ 0x8005eb80  (Ghidra headless, raw MIPS overlay) */

void FUN_8005eb80(int param_1)

{
  if ((*(ushort *)(param_1 + 0x10e) & 0x100) != 0) {
    *(ushort *)(param_1 + 0x10e) = *(ushort *)(param_1 + 0x10e) & 0xfeff;
    FUN_80017054(*(int *)(param_1 + 0x198) + 4,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0xb0,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x15c,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x208,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x360,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x40c,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x610,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x6bc,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x814,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
    FUN_80017054(*(int *)(param_1 + 0x198) + 0x8c0,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
  }
  return;
}


