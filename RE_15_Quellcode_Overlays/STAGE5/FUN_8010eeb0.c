/* FUN_8010eeb0 @ 0x8010eeb0  (Ghidra headless overlay RE) */

/* WARNING: Removing unreachable block (ram,0x8010f1dc) */
/* WARNING: Removing unreachable block (ram,0x8010f1e0) */
/* WARNING: Removing unreachable block (ram,0x8010f1ec) */
/* WARNING: Removing unreachable block (ram,0x8010f1f8) */
/* WARNING: Removing unreachable block (ram,0x8010f204) */
/* WARNING: Removing unreachable block (ram,0x8010f264) */
/* WARNING: Removing unreachable block (ram,0x8010f274) */
/* WARNING: Removing unreachable block (ram,0x8010f168) */
/* WARNING: Removing unreachable block (ram,0x8010f194) */
/* WARNING: Removing unreachable block (ram,0x8010f1d8) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010eeb0(void)

{
  bool bVar1;
  byte bVar2;
  undefined1 uVar3;
  char cVar4;
  ushort uVar5;
  undefined2 uVar6;
  int iVar7;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 == 1) {
LAB_8010ef68:
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar5 & 0x3f) + 100;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x10) + 0x40;
    *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
    if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 0xd) ||
       (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x28 < 0xd)) {
      func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    }
    cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar4;
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x28) {
      FUN_8011145c(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
        func_0x800453d0(9);
      }
      if (DAT_800acae7 != '\0') {
        return;
      }
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      uStack_28 = _DAT_80072d60;
      uStack_24 = _DAT_80072d64;
      uStack_20 = _DAT_80072d68;
      uStack_1c = _DAT_80072d6c;
      bVar2 = 5;
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x8011faf4)) {
          uVar6 = func_0x8001bff8(iVar7 + 0x448,&uStack_28,800,&DAT_800aca88);
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
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
    return;
  }
  if (bVar2 < 2) {
    uVar3 = 1;
    if (bVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      func_0x800453d0(9);
      goto LAB_8010ef68;
    }
  }
  else {
    uVar3 = 0xd;
    if (bVar2 == 2) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
  }
  *(undefined1 *)(_DAT_800ac784 + 0x93) = uVar3;
  *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  iVar7 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
  uStack_28 = *(undefined4 *)(&LAB_8011f9fc + iVar7);
  uStack_24 = *(undefined4 *)(&LAB_8011fa00 + iVar7);
  uStack_20 = *(undefined4 *)(&LAB_8011fa04 + iVar7);
  uStack_1c = *(undefined4 *)(&LAB_8011fa08 + iVar7);
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,&uStack_28);
  func_0x800453d0(2);
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  if (*(char *)(_DAT_800ac784 + 0x94) == '\b') {
    FUN_80111388(0,0);
  }
  else {
    FUN_80111388(0,1);
  }
  return;
}


