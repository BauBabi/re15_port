/* FUN_8011af50 @ 0x8011af50  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011af50(uint param_1,undefined4 param_2,undefined1 param_3)

{
  char cVar1;
  uint in_v0;
  char in_v1;
  
  DAT_800acae9 = 0;
  _DAT_800aca3c = in_v0 | 0x40;
  DAT_800acae7 = DAT_800acae7 | 1;
  DAT_800acaf2 = in_v1;
  DAT_800acc0c = param_3;
  func_0x80045024(param_1 | 1);
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x400);
  DAT_800acaf2 = DAT_800acaf2 - cVar1;
  if (DAT_800acaf2 == '\0') {
    DAT_800aca5a = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    func_0x8001af5c(0,0,600,700);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 4;
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    }
    return;
  }
  return;
}


