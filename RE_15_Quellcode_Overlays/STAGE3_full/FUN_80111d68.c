/* FUN_80111d68 @ 0x80111d68  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111d68(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  undefined2 uVar5;
  ushort uVar6;
  int iVar7;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar3 = false;
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 != 2) {
        return;
      }
      if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x14;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
  FUN_80115b68(0,0);
  iVar7 = *(int *)(_DAT_800ac784 + 0x188);
  local_24 = _DAT_80072d64;
  local_20 = _DAT_80072d68;
  local_1c = _DAT_80072d6c;
  local_28 = 100;
  if (*(char *)(_DAT_800ac784 + 0x95) == '\v') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 == 0) {
    uVar5 = func_0x8001bff8(iVar7 + 0x64c,&local_28,1000,&DAT_800aca88);
    uVar6 = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
    do {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar6 + 0x8011ed48)) {
        bVar3 = true;
      }
      bVar1 = uVar6 != 0;
      uVar6 = uVar6 - 1;
    } while (bVar1);
    if ((bVar3) && (*(short *)(_DAT_800ac784 + 0x1da) != 0)) {
      _DAT_800acaee = _DAT_800acaee + -5;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(5);
      DAT_800aca58 = 2;
      cVar4 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar4 + '\x02';
      DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      DAT_800acae7 = DAT_800acae7 | 1;
    }
  }
  return;
}


