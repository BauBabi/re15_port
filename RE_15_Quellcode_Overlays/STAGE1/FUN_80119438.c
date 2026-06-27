/* FUN_80119438 @ 0x80119438  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119438(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  undefined2 uVar5;
  undefined1 *puVar6;
  char unaff_s0;
  byte unaff_s1;
  int iVar7;
  char unaff_s3;
  char unaff_s4;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar2 & 2) != 0) {
    if ((bVar2 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      iVar7 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(iVar7 + 0x9c) = 0;
      func_0x800453d0();
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
        FUN_8011a5f8(0,0);
        return;
      }
      FUN_8011c024(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) == '(') {
        func_0x800453d0(9);
      }
      if (DAT_800acae7 == 0) {
        iVar7 = *(int *)(_DAT_800ac784 + 0x188);
        local_28 = _DAT_80072d60;
        local_24 = _DAT_80072d64;
        local_20 = _DAT_80072d68;
        local_1c = _DAT_80072d6c;
        bVar2 = unaff_s0 - 1;
        if (unaff_s0 != '\0') {
          do {
            if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(bVar2 + 0x80121480)) {
              uVar5 = func_0x8001bff8(iVar7 + 0x448,&local_28,800,&DAT_800aca88);
              *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
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
        while (bVar1 = unaff_s1 != 0, unaff_s1 = unaff_s1 - 1, bVar1) {
          if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(unaff_s1 + 0x80121488)) {
            uVar5 = func_0x8001bff8(iVar7 + 0x6f8,&local_28,800,&DAT_800aca88);
            *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar5;
            if (*(short *)(_DAT_800ac784 + 0x1da) != 0) {
              unaff_s4 = '\x01';
            }
          }
        }
        if ((unaff_s3 != '\0') || (unaff_s4 != '\0')) {
          _DAT_800acaee = _DAT_800acaee - 0xc;
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
          func_0x800453d0(6);
          DAT_800aca58 = 2;
          puVar6 = &DAT_800aca54;
          cVar3 = func_0x8001a780();
          DAT_800aca59 = cVar3 + '\x02';
          DAT_800aca5a = 0;
          if ((short)_DAT_800acaee < 0) {
            DAT_800aca58 = 3;
            DAT_800aca59 = '\0';
          }
          uVar4 = (ushort)DAT_800acae7;
          DAT_800acae7 = (byte)(uVar4 | 1);
          if (_DAT_800acaee == (uVar4 | 1)) {
            puVar6[0x93] = 0;
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
      }
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar2 & 0xfd;
    local_28 = _DAT_801213a8;
    local_24 = _DAT_801213ac;
    local_20 = _DAT_801213b0;
    local_1c = _DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}


