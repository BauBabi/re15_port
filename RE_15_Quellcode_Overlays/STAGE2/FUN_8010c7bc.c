/* FUN_8010c7bc @ 0x8010c7bc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c7bc(void)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  undefined1 uVar5;
  undefined2 uVar6;
  ushort uVar7;
  int iVar8;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  bVar3 = false;
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 == 2) {
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        iVar8 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),100,0xfffffff6,
                                (int)*(short *)(_DAT_800ac784 + 0x1ba));
        if (iVar8 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
          *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x21;
          FUN_8010d390();
          return;
        }
        if (DAT_800acae7 == 0) {
          local_14 = _DAT_80072d64;
          local_10 = _DAT_80072d68;
          local_c = _DAT_80072d6c;
          local_18 = 700;
          uVar6 = func_0x8001bff8(*(int *)(_DAT_800ac784 + 0x188) + 0xa54,&local_18,800,
                                  &DAT_800aca88);
          uVar7 = 2;
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar6;
          do {
            if (*(char *)(_DAT_800ac784 + 0x95) == *(char *)(uVar7 + 0x80118c68)) {
              bVar3 = true;
            }
            bVar1 = uVar7 != 0;
            uVar7 = uVar7 - 1;
          } while (bVar1);
          if ((bVar3) && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
            *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
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
          if (DAT_800acae7 == 0) {
            return;
          }
        }
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0x80118bb8);
        FUN_8010d390();
        return;
      }
      if (bVar2 != 3) {
        FUN_8010d390();
        return;
      }
      iVar8 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar8 != 0) {
        if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
          *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
        }
        uVar5 = 4;
        if (*(char *)(_DAT_800ac784 + 0x1e0) == '\0') {
          uVar5 = 2;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = uVar5;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        FUN_8010d390();
        return;
      }
      goto LAB_8010cb88;
    }
    if (bVar2 != 0) {
      FUN_8010d390();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    uVar6 = func_0x8001a9cc(&DAT_800aca88,0x20);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar6;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x1da),0);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\f') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar7 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar7 & 0x1f) + 0x78;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
    FUN_8010d388();
    return;
  }
LAB_8010cb88:
  func_0x800245d8(0);
  return;
}


