/* FUN_80098640 @ 0x80098640  (Ghidra headless, raw MIPS overlay) */

void FUN_80098640(int param_1)

{
  byte bVar1;
  
  *(int *)(param_1 + 0x4c) = *(int *)(param_1 + 0x4c) + 1;
  if (*(char *)(param_1 + 0x46) == '\0') {
LAB_800986d8:
    if (**(char **)(param_1 + 0x3c) != -0xd) {
      **(undefined1 **)(param_1 + 0x30) = 0xff;
      *(undefined1 *)(*(int *)(param_1 + 0x30) + 1) = 0;
      *(undefined1 *)(param_1 + 0xe8) = 0;
    }
  }
  else {
    if (*(char *)(param_1 + 0x46) == '\x01') {
      bVar1 = *(byte *)(param_1 + 0x4a);
      if (1 < bVar1) {
        *(undefined1 *)(param_1 + 0x49) = 2;
        *(undefined1 *)(param_1 + 0x46) = 0xff;
        return;
      }
    }
    else {
      bVar1 = *(byte *)(param_1 + 0x4a);
      if (3 < bVar1) {
        if (*(char *)(param_1 + 0x49) != '\0') {
          (*DAT_800c39a0)(param_1);
        }
        goto LAB_800986d8;
      }
    }
    *(byte *)(param_1 + 0x4a) = bVar1 + 1;
  }
  return;
}


