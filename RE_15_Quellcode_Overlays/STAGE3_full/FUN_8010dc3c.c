/* FUN_8010dc3c @ 0x8010dc3c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dc3c(void)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  uint uVar4;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    func_0x800453d0(5);
switchD_8010dc78_caseD_1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),
                            *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (0xc < *(byte *)(_DAT_800ac784 + 0x95)) {
      *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 6;
      func_0x800245d8(0);
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0xdc;
      if ((DAT_800acae7 == 0) && (iVar3 = func_0x8001a804(2000,0x180,&DAT_800aca88), iVar3 < 0)) {
        _DAT_800acaee = _DAT_800acaee - 10;
        if ((*(short *)(_DAT_800ac784 + 0x1e4) == 0) && (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
          func_0x800453d0(3);
          *(undefined1 *)(_DAT_800ac784 + 6) = 5;
          DAT_800aca58 = 2;
          cVar1 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar1 + 2;
          DAT_800aca5a = 0;
          DAT_800acae7 = DAT_800acae7 | 1;
          uVar4 = func_0x8001af20();
          func_0x80037edc(*(undefined1 *)
                           ((int)&PTR_LAB_8011eb28 + (uVar4 & 1) + (uint)DAT_800aca59 * 2),0x32);
        }
        else {
LAB_8010df48:
          cVar1 = func_0x8001a780(&DAT_800aca54);
          *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
          *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
        }
      }
    }
switchD_8010dc78_default:
    return;
  case 1:
    goto switchD_8010dc78_caseD_1;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x15;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  case 3:
    if (1 < *(byte *)(_DAT_800ac784 + 0x95)) {
      *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -3;
      func_0x800245d8(8);
      *(int *)(_DAT_800ac784 + 0x38) =
           (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xb) * 0x10000 >> 0x10) +
           *(int *)(_DAT_800ac784 + 0x38);
      if ((int)*(short *)(_DAT_800ac784 + 0x1ba) < *(int *)(_DAT_800ac784 + 0x38)) {
        *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
        *(undefined1 *)(_DAT_800ac784 + 6) = 4;
      }
LAB_8010e078:
      cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),
                              *(undefined1 *)(_DAT_800ac784 + 7),0x200);
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
      return;
    }
    func_0x800245d8(0);
    if ((DAT_800acae7 != 0) || (iVar3 = func_0x8001a804(2000,0x180,&DAT_800aca88), -1 < iVar3))
    goto LAB_8010e078;
    _DAT_800acaee = _DAT_800acaee - 10;
    if ((*(short *)(_DAT_800ac784 + 0x1e4) == 0) && (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
      func_0x800453d0(3);
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      DAT_800aca58 = 2;
      cVar1 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar1 + 2;
      DAT_800aca5a = 0;
      DAT_800acae7 = DAT_800acae7 | 1;
      goto LAB_8010e078;
    }
    goto LAB_8010df48;
  case 4:
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 10;
    uVar2 = 3;
code_r0x8010e2a4:
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
    goto LAB_8010e2a8;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
    *(short *)(_DAT_800ac784 + 0x9c) =
         *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  case 6:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),
                            *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (0x10 < *(byte *)(_DAT_800ac784 + 0x95)) {
      return;
    }
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -3;
    func_0x800245d8(0x800);
    *(int *)(_DAT_800ac784 + 0x38) =
         (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xe) * 0x10000 >> 0x10) +
         *(int *)(_DAT_800ac784 + 0x38);
    if (*(int *)(_DAT_800ac784 + 0x38) <= (int)*(short *)(_DAT_800ac784 + 0x1ba)) {
      return;
    }
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    return;
  case 7:
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    if ((short)_DAT_800acaee < 0x32) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    }
    uVar4 = func_0x8001af20();
    uVar2 = 6;
    if ((uVar4 & 1) != 0) goto code_r0x8010e2a4;
LAB_8010e2a8:
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  default:
    goto switchD_8010dc78_default;
  }
}


