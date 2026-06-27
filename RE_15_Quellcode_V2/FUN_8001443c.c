void FUN_8001443c(void)

{
  short sVar1;
  uint uVar2;
  uint uVar3;
  
  if ((DAT_800ac762 & 0x100) != 0) {
    if (DAT_800bbe5c == '\0') {
      DAT_800bbe5c = '\x01';
      FUN_80030640(0xf000,5,1);
      DAT_8008f618 = 1;
      DAT_800bbe5e = (byte)DAT_800b0fe0;
      DAT_800aca40 = DAT_800aca40 | 0x800000;
      (&DAT_800bbe5f)[(byte)DAT_800b0fe0] = DAT_800b0fe2;
      DAT_800bbe68 = 0x20;
      DAT_800bbe6a = 0x4c;
      DAT_800bbe6c = 0x100;
      DAT_800bbe6e = 0x38;
      _DAT_800bbe66 = 0xff;
    }
    else {
      DAT_8008f618 = 2;
    }
  }
  if (DAT_800bbe5c != '\0') {
    if (DAT_8008f618 == 1) {
      if (DAT_800aca38 < 0) {
        if (((DAT_800ac760 & 0x1000) != 0) && (DAT_800bbe5d != 0)) {
          DAT_800bbe5d = DAT_800bbe5d - 1;
        }
        if (((DAT_800ac760 & 0x4000) != 0) && (DAT_800bbe5d < 2)) {
          DAT_800bbe5d = DAT_800bbe5d + 1;
        }
      }
      if ((DAT_800ac762 & 0x40) == 0) {
        if (DAT_800bbe5d == 1) {
          if (DAT_800aca38 < 0) {
            if ((DAT_800ac760 & 0x2000) != 0) {
              do {
                (&DAT_800bbe5f)[DAT_800bbe5e] = (&DAT_800bbe5f)[DAT_800bbe5e] + '\x01';
                if (0x30 < (byte)(&DAT_800bbe5f)[DAT_800bbe5e]) {
                  (&DAT_800bbe5f)[DAT_800bbe5e] = 0;
                }
              } while ((&DAT_800c263a)
                       [(uint)(byte)(&DAT_800bbe5f)[DAT_800bbe5e] * 0xd + (uint)DAT_800bbe5e * 0x27d
                       ] == 0);
            }
            if ((DAT_800ac760 & 0x8000) != 0) {
              do {
                uVar2 = (uint)DAT_800bbe5e;
                if ((&DAT_800bbe5f)[uVar2] == '\0') {
                  (&DAT_800bbe5f)[uVar2] = 0x2f;
                  uVar2 = (uint)DAT_800bbe5e;
                }
                (&DAT_800bbe5f)[uVar2] = (&DAT_800bbe5f)[uVar2] + -1;
              } while ((&DAT_800c263a)
                       [(uint)(byte)(&DAT_800bbe5f)[DAT_800bbe5e] * 0xd + (uint)DAT_800bbe5e * 0x27d
                       ] == 0);
            }
          }
          if ((DAT_800ac762 & 0x10) != 0) {
            do {
              DAT_800bbe5e = DAT_800bbe5e + 1;
              if (5 < DAT_800bbe5e) {
                DAT_800bbe5e = 0;
              }
              uVar2 = (uint)DAT_800bbe5e;
            } while ((&DAT_800c263a)[uVar2 * 0x27d] == 2);
            uVar3 = (uint)(byte)(&DAT_800bbe5f)[uVar2];
            sVar1 = (&DAT_800c263a)[uVar2 * 0x27d + uVar3 * 0xd];
            while (sVar1 == 0) {
              (&DAT_800bbe5f)[uVar2] = (char)uVar3 + '\x01';
              if (0x30 < (byte)(&DAT_800bbe5f)[DAT_800bbe5e]) {
                (&DAT_800bbe5f)[DAT_800bbe5e] = 0;
              }
              uVar2 = (uint)DAT_800bbe5e;
              uVar3 = (uint)(byte)(&DAT_800bbe5f)[uVar2];
              sVar1 = (&DAT_800c263a)[uVar3 * 0xd + uVar2 * 0x27d];
            }
          }
          if ((DAT_800ac762 & 0x80) != 0) {
            DAT_800ac9a8 = 0;
            DAT_800b5359 = 1;
            DAT_800bbe5c = 0;
            return;
          }
        }
        else if (((1 < DAT_800bbe5d) && (DAT_800bbe5d == 2)) && ((DAT_800ac762 & 0x80) != 0)) {
          DAT_8008f618 = 3;
          DAT_8008f624 = (code *)&LAB_80014e78;
          DAT_800bbe68 = 0x20;
          DAT_800bbe6a = 0x38;
          DAT_800bbe6c = 0x100;
          DAT_800bbe6e = 0x68;
        }
        FUN_80014cc4();
        FUN_800279c8(0x540068,3,"- DEBUG MENU -",0);
        FUN_800279c8(0x640060,3,"UTILITY MENU",0);
        FUN_800279c8(0x6c0060,3,"JUMP %d",DAT_800bbe5e + 1);
        FUN_800279c8(0x6c0090,3,&DAT_80010554,(&DAT_800bbe5f)[DAT_800bbe5e]);
        FUN_800279c8(0x6c00a8,3,&DAT_80010558,
                     &UNK_800c2642 +
                     (uint)DAT_800bbe5e * 0x4fa + (uint)(byte)(&DAT_800bbe5f)[DAT_800bbe5e] * 0x1a);
        FUN_800279c8(0x740060,3,"MEMORY VIEWER",0);
        FUN_800279c8(((uint)DAT_800bbe5d * 8 + 100) * 0x10000 | 0x50,3,&DAT_8001056c,0);
      }
      else {
        DAT_8008f618 = 2;
      }
    }
    else if (DAT_8008f618 < 2) {
      if (DAT_8008f618 == 0) {
        FUN_80014cc4();
        _DAT_800bbe66 = _DAT_800bbe66 + 8;
        if (0x40 < _DAT_800bbe66) {
          DAT_8008f618 = 1;
        }
      }
    }
    else if (DAT_8008f618 == 2) {
      DAT_800bbe5c = '\0';
      DAT_800aca40 = DAT_800aca40 & 0xff7fffff;
    }
    else if (DAT_8008f618 == 3) {
      FUN_80014cc4();
      DAT_8008f618 = (*DAT_8008f624)(DAT_8008f618);
      if (((DAT_800ac762 & 0x40) != 0) || (DAT_8008f618 == 1)) {
        DAT_800bbe68 = 0x20;
        DAT_800bbe6a = 0x4c;
        DAT_800bbe6c = 0x100;
        DAT_8008f618 = 1;
        DAT_800bbe6e = 0x38;
      }
    }
  }
  return;
}
