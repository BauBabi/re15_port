/* FUN_80053138 @ 0x80053138  (Ghidra headless, raw MIPS overlay) */

void FUN_80053138(uint param_1)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 *puVar5;
  
  if (DAT_800d8cc8 == '\0') {
    if (9 < param_1) {
      param_1 = 2;
      puVar4 = &UNK_800d7b48;
      if (DAT_800d7b49 != '\0') {
        do {
          if (param_1 == 9) break;
          param_1 = param_1 + 1;
          if (9 < param_1) {
            return;
          }
          pcVar1 = puVar4 + 0x175;
          puVar4 = puVar4 + 0x174;
        } while (*pcVar1 != '\0');
      }
    }
  }
  else if (0xd < param_1) {
    param_1 = 10;
    puVar4 = &DAT_800d86e8;
    if (DAT_800d86e9 != '\0') {
      do {
        if (param_1 == 0xd) break;
        param_1 = param_1 + 1;
        if (0xd < param_1) {
          return;
        }
        pcVar1 = puVar4 + 0x175;
        puVar4 = puVar4 + 0x174;
      } while (*pcVar1 != '\0');
    }
  }
  iVar2 = param_1 * 0x174;
  (&DAT_800d7862)[iVar2] = 0;
  puVar5 = (undefined4 *)(&DAT_800d79b8 + iVar2);
  iVar3 = 6;
  do {
    *puVar5 = 0;
    iVar3 = iVar3 + -1;
    puVar5 = puVar5 + 1;
  } while (iVar3 != 0);
  FUN_800530ec(&DAT_800d7860 + iVar2);
  return;
}


