/* FUN_80113e48 @ 0x80113e48  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e48(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined2 uVar3;
  int iVar4;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x17;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(3);
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    local_1c = _DAT_80072d64;
    local_18 = _DAT_80072d68;
    local_14 = _DAT_80072d6c;
    local_20 = 100;
    uVar3 = func_0x8001bff8(iVar4 + 0x244,&local_20,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar3;
    uVar3 = func_0x8001bff8(iVar4 + 0x9a8,&local_20,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar3;
    if (((10 < *(byte *)(_DAT_800ac784 + 0x95)) && (DAT_800acae7 == '\0')) &&
       ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
      _DAT_800acaee = _DAT_800acaee + -10;
      uVar2 = 0xf;
LAB_801140e4:
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x17;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 10;
  case 3:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),1,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    break;
  case 4:
    cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x1e;
    uVar2 = 0xd;
    goto LAB_801140e4;
  default:
    break;
  }
  return;
}


