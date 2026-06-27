/* FUN_801136d8 @ 0x801136d8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801136d8(undefined1 *param_1)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  undefined2 uVar5;
  int iVar6;
  int in_v1;
  char unaff_s0;
  byte unaff_s1;
  char unaff_s3;
  char unaff_s4;
  undefined4 in_stack_00000010;
  undefined4 in_stack_00000014;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  
  bVar2 = 1;
  if (in_v1 != 0) {
LAB_80114298:
    param_1[0x93] = bVar2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar6 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    in_stack_00000010 = *(undefined4 *)(&LAB_8011ec64 + iVar6);
    in_stack_00000014 = *(undefined4 *)(&LAB_8011ec68 + iVar6);
    in_stack_00000018 = *(undefined4 *)(&LAB_8011ec6c + iVar6);
    in_stack_0000001c = *(undefined4 *)(&LAB_8011ec70 + iVar6);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0x00000010);
    func_0x800453d0(2);
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
      FUN_80115b68(0,0);
    }
    else {
      FUN_80115b68(0,1);
    }
    return;
  }
  param_1[6] = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  func_0x800453d0(9);
  uVar4 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x3f) + 100;
  bVar2 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x10) + 0x40;
  *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 0xd) ||
     (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x28 < 0xd)) {
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
  if (0x27 < *(byte *)(_DAT_800ac784 + 0x95)) {
    return;
  }
  FUN_80115c3c(0,1);
  if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
    func_0x800453d0(9);
  }
  if (DAT_800acae7 == 0) {
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    in_stack_00000010 = _DAT_80072d60;
    in_stack_00000014 = _DAT_80072d64;
    in_stack_00000018 = _DAT_80072d68;
    in_stack_0000001c = _DAT_80072d6c;
    bVar2 = unaff_s0 - 1;
    if (unaff_s0 != '\0') {
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x8011ed5c)) {
          uVar5 = func_0x8001bff8(iVar6 + 0x448,&stack0x00000010,800,&DAT_800aca88);
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
        }
        bVar1 = bVar2 != 0;
        bVar2 = bVar2 - 1;
      } while (bVar1);
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      }
      if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      }
      return;
    }
    while (bVar1 = unaff_s1 != 0, unaff_s1 = unaff_s1 - 1, bVar1) {
      if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s1 + 0x8011ed64)) {
        uVar5 = func_0x8001bff8(iVar6 + 0x6f8,&stack0x00000010,800,&DAT_800aca88);
        *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
        if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
          unaff_s4 = '\x01';
        }
      }
    }
    if ((unaff_s3 != '\0') || (unaff_s4 != '\0')) {
      _DAT_800acaee = _DAT_800acaee + -10;
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
      func_0x800453d0(5);
      DAT_800aca58 = 2;
      param_1 = &DAT_800aca54;
      cVar3 = func_0x8001a780();
      DAT_800aca59 = cVar3 + '\x02';
      DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      bVar2 = DAT_800acae7 | 1;
      DAT_800acae7 = bVar2;
      goto LAB_80114298;
    }
  }
  return;
}


