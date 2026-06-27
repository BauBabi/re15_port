/* FUN_80117d10 @ 0x80117d10  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117d10(void)

{
  undefined2 uVar1;
  char cVar2;
  undefined1 uVar3;
  short sVar4;
  int iVar5;
  uint uVar6;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  int local_28;
  int local_24;
  int local_20;
  char local_18 [8];
  
  local_38 = DAT_801003d8;
  local_34 = DAT_801003dc;
  local_30 = DAT_801003e0;
  local_2c = DAT_801003e4;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
switchD_80117dbc_caseD_1:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    iVar5 = func_0x8001a9cc(&DAT_800aca88,0x400);
    if (((iVar5 == 0) && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0xfa1)) ||
       (0x1d < *(byte *)(_DAT_800ac784 + 0x95))) {
      if (*(byte *)(_DAT_800ac784 + 0x95) - 0x24 < 8) {
        iVar5 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_38,&local_30,
                                &DAT_800aca54);
        if ((iVar5 != 0) && (DAT_800acae7 == '\0')) {
          sVar4 = func_0x8001a6d4(*(undefined4 *)(_DAT_800ac784 + 0x34),
                                  *(undefined4 *)(_DAT_800ac784 + 0x3c),_DAT_800aca88,_DAT_800aca90)
          ;
          uVar1 = *(undefined2 *)(_DAT_800ac784 + 0x6a);
          *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + -0x400;
          uVar6 = func_0x8001a780(&DAT_800aca54);
          *(undefined2 *)(_DAT_800ac784 + 0x6a) = uVar1;
          _DAT_800acabe = sVar4 - ((short)(uVar6 & 0xff) * 0x800 + -0x400);
          _DAT_800acbfc = _DAT_800ac784;
          _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
          _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
          _DAT_800aca58 = (uVar6 & 0xff) << 8 | 5;
          func_0x800453d0(7);
          _DAT_800acaee = _DAT_800acaee - 10;
          if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
            _DAT_800acaee = 1;
          }
        }
        local_28 = (int)(short)local_30;
        local_24 = (int)local_30._2_2_;
        local_20 = (int)(short)local_2c;
        func_0x8001c2dc(&local_28,400,local_18);
        if (local_18[0] != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        }
      }
      if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
        func_0x800453d0(4);
      }
switchD_80117dbc_default:
      FUN_8011bdd8(0,0);
    }
    else {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    }
    return;
  case 1:
    goto switchD_80117dbc_caseD_1;
  case 2:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    uVar3 = 0x1e;
    goto LAB_801181bc;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x800453d0(6);
  case 4:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x400);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    if (((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) && (*(char *)(_DAT_800ac784 + 0x95) == '\x1e'))
    {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    }
    goto switchD_80117dbc_default;
  case 5:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    uVar3 = 0x3c;
LAB_801181bc:
    *(undefined1 *)(_DAT_800ac784 + 0x1df) = uVar3;
  default:
    goto switchD_80117dbc_default;
  }
}


