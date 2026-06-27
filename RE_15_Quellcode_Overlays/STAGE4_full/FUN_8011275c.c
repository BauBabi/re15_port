/* FUN_8011275c @ 0x8011275c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011275c(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(3);
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_80115bec(0,0);
    if (((9 < *(byte *)(_DAT_800ac784 + 0x95)) && (DAT_800acae7 == 0)) &&
       (iVar2 = func_0x8001a804(0x898,0x180,&DAT_800aca88), iVar2 < 0)) {
      _DAT_800acaee = _DAT_800acaee + -10;
      if (_DAT_800acaee < 0x32) {
        cVar1 = func_0x8001a780(&DAT_800aca54);
        cVar1 = cVar1 * '\x04' + '\x05';
LAB_80112a34:
        *(char *)(_DAT_800ac784 + 5) = cVar1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      else {
        func_0x800453d0(9);
        *(undefined1 *)(_DAT_800ac784 + 6) = 2;
        DAT_800aca58 = 2;
        cVar1 = func_0x8001a780(&DAT_800aca54);
        DAT_800aca59 = cVar1 + 2;
        DAT_800aca5a = 0;
        DAT_800acae7 = DAT_800acae7 | 1;
        uVar3 = func_0x8001af20();
        func_0x80037edc(*(undefined1 *)
                         ((int)&PTR_FUN_8011a150 + (uVar3 & 1) + (uint)DAT_800aca59 * 2),0x32);
      }
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 10;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_80115bec(0,0);
    break;
  case 4:
    cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x2d;
    cVar1 = '\x03';
    goto LAB_80112a34;
  default:
    break;
  }
  return;
}


