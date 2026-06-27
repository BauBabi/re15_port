/* FUN_80081774 @ 0x80081774  (Ghidra headless, raw MIPS overlay) */

void FUN_80081774(void)

{
  ushort uVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  byte bVar5;
  uint uVar6;
  byte bVar7;
  char cVar8;
  ushort uVar9;
  uint uVar10;
  byte bVar11;
  
  bVar2 = DAT_800d7854;
  uVar9 = 0xffff;
  cVar8 = '\0';
  uVar6 = 0;
  bVar7 = 99;
  uVar10 = (uint)DAT_800dcc3f;
  bVar5 = 0;
  bVar11 = 99;
  if (DAT_800d7854 != 0) {
    uVar3 = 0;
    do {
      if (((&DAT_800cbced)[uVar3 * 0x36] == '\0') &&
         (bVar11 = bVar5, (&DAT_800cbcd6)[uVar3 * 0x1b] == 0)) break;
      uVar3 = (uint)bVar5;
      uVar4 = (uint)(short)(&DAT_800cbcea)[uVar3 * 0x1b];
      if ((int)uVar4 < (int)(uVar10 & 0xffff)) {
        uVar9 = (&DAT_800cbcd6)[uVar3 * 0x1b];
        cVar8 = '\x01';
        uVar6 = (uint)(ushort)(&DAT_800cbcd2)[uVar3 * 0x1b];
        uVar10 = uVar4;
        bVar7 = bVar5;
      }
      else if (uVar4 == (uVar10 & 0xffff)) {
        uVar1 = (&DAT_800cbcd6)[uVar3 * 0x1b];
        cVar8 = cVar8 + '\x01';
        if (uVar1 < uVar9) {
          uVar3 = (uint)(ushort)(&DAT_800cbcd2)[uVar3 * 0x1b];
          uVar9 = uVar1;
        }
        else if ((uVar1 != uVar9) ||
                (uVar3 = (uint)(short)(&DAT_800cbcd2)[uVar3 * 0x1b], (int)uVar3 <= (int)uVar6))
        goto LAB_800818c4;
        uVar6 = uVar3;
        bVar7 = bVar5;
      }
LAB_800818c4:
      bVar5 = bVar5 + 1;
      uVar3 = (uint)bVar5;
      bVar11 = 99;
    } while (bVar5 < DAT_800d7854);
  }
  if ((bVar11 == 99) && (bVar11 = bVar7, cVar8 == '\0')) {
    bVar11 = DAT_800d7854;
  }
  if (bVar11 < DAT_800d7854) {
    bVar7 = 0;
    if (DAT_800d7854 != 0) {
      uVar6 = 0;
      do {
        bVar7 = bVar7 + 1;
        (&DAT_800cbcd2)[uVar6 * 0x1b] = (&DAT_800cbcd2)[uVar6 * 0x1b] + 1;
        uVar6 = (uint)bVar7;
      } while (bVar7 < bVar2);
    }
    uVar6 = (uint)bVar11;
    (&DAT_800cbcd2)[uVar6 * 0x1b] = 0;
    (&DAT_800cbcea)[uVar6 * 0x1b] = (ushort)DAT_800dcc3f;
    if ((&DAT_800cbced)[uVar6 * 0x36] == '\x02') {
      SpuSetNoiseVoice(0,0xffffff);
    }
  }
  (&DAT_800cbcfa)[(uint)bVar11 * 0x1b] = 0;
  return;
}


