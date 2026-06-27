/* FUN_80119c4c @ 0x80119c4c  (Ghidra headless overlay RE) */

/* WARNING: Removing unreachable block (ram,0x80119f78) */
/* WARNING: Removing unreachable block (ram,0x80119f7c) */
/* WARNING: Removing unreachable block (ram,0x80119f88) */
/* WARNING: Removing unreachable block (ram,0x80119f94) */
/* WARNING: Removing unreachable block (ram,0x80119fa0) */
/* WARNING: Removing unreachable block (ram,0x8011a000) */
/* WARNING: Removing unreachable block (ram,0x8011a010) */
/* WARNING: Removing unreachable block (ram,0x80119f04) */
/* WARNING: Removing unreachable block (ram,0x80119f30) */
/* WARNING: Removing unreachable block (ram,0x80119f74) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119c4c(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  byte bVar4;
  ushort uVar5;
  undefined2 uVar6;
  int iVar7;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 == 1) {
LAB_80119d04:
    uVar5 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar5 & 0x3f) + 100;
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
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x28) {
      FUN_8011c024(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
        func_0x800453d0(9);
      }
      if (DAT_800acae7 != '\0') {
        return;
      }
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      local_28 = _DAT_80072d60;
      local_24 = _DAT_80072d64;
      local_20 = _DAT_80072d68;
      local_1c = _DAT_80072d6c;
      bVar2 = 5;
      do {
        if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x80121480)) {
          uVar6 = func_0x8001bff8(iVar7 + 0x448,&local_28,800,&DAT_800aca88);
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
        }
        bVar1 = bVar2 != 0;
        bVar2 = bVar2 - 1;
      } while (bVar1);
      iVar7 = func_0x8001c1a4();
      if (iVar7 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
        *(undefined4 *)(_DAT_800ac784 + 0x78) =
             *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
        func_0x800453d0(2);
      }
      (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb58))();
      return;
    }
    FUN_8011a5f8(0,0);
    return;
  }
  if (bVar2 < 2) {
    bVar4 = 1;
    if (bVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1a;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      func_0x800453d0(9);
      goto LAB_80119d04;
    }
  }
  else {
    bVar4 = 0xd;
    if (bVar2 == 2) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
  }
  if (bVar2 == bVar4) {
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    }
    return;
  }
  FUN_8011bb8c();
  return;
}


