undefined * FUN_80047648(byte param_1)

{
  short sVar1;
  char cVar2;
  undefined1 uVar3;
  short sVar4;
  int iVar5;
  uint uVar6;
  short *psVar7;
  short *psVar8;
  short *psVar9;
  undefined *p;
  ushort uVar10;
  ushort local_30;
  
  uVar6 = (uint)param_1;
  local_30 = 0;
  psVar9 = (short *)(&PTR_DAT_80074a90)[uVar6 * 3];
  p = (&PTR_DAT_80074a94)[uVar6 * 3];
  if (DAT_800aca34 == 0) {
    p = p + 0x14;
  }
  if ((uVar6 == 8) || (uVar6 == 0x11)) {
    if ((&DAT_800b10ae)[(uint)DAT_800b25bd * 4] == '\x01') {
      local_30 = 8;
    }
    if ((&DAT_800b10ae)[(uint)DAT_800b25bd * 4] == '\x02') {
      local_30 = 0x10;
    }
  }
  if ((param_1 == 9) || (param_1 == 0x12)) {
    if ((&DAT_800b10ae)[(uint)DAT_800b25be * 4] == '\x01') {
      local_30 = 2;
    }
    if ((&DAT_800b10ae)[(uint)DAT_800b25be * 4] == '\x02') {
      local_30 = 4;
    }
  }
  uVar10 = 0;
  if (*(short *)(&DAT_80074a8e + (uint)param_1 * 0xc) != 0) {
    psVar8 = psVar9 + 1;
    psVar7 = (short *)(p + 10);
    do {
      switch(param_1) {
      case 0:
        break;
      case 1:
        psVar7[-1] = *psVar9 + DAT_800b25e0;
        *psVar7 = *psVar8 + DAT_800b25e2;
        break;
      case 2:
        if (uVar10 == 0) {
          *(char *)(psVar7 + 1) = (char)psVar8[3];
          cVar2 = FUN_8004ed6c();
          *(char *)((int)psVar7 + 3) = (char)psVar8[4] + cVar2 * '\x10';
          uVar6 = FUN_8004ed6c();
          psVar7[2] = (&DAT_800b2610)
                      [(uint)*(ushort *)(&DAT_80074a8c + (uint)param_1 * 0xc) + (uVar6 & 0xff)];
          psVar7[-1] = *psVar9 + DAT_800b25e4;
          sVar4 = *psVar8 + DAT_800b25e6;
          goto LAB_80048668;
        }
        psVar7[2] = DAT_800b261c;
        psVar7[-1] = *psVar9 + DAT_800b25e4;
        *psVar7 = *psVar8 + DAT_800b25e6;
        *(undefined1 *)(psVar7 + -3) = DAT_800b2602;
        *(undefined1 *)((int)psVar7 + -5) = DAT_800b2602;
        *(undefined1 *)(psVar7 + -2) = DAT_800b2602;
        break;
      case 3:
        if (uVar10 < 4) {
          psVar7[-1] = *psVar9 + DAT_800b25d8;
          sVar4 = *psVar8 + DAT_800b25da;
          goto LAB_80048668;
        }
        psVar7[-1] = *psVar9 + DAT_800b25d8;
        sVar4 = *psVar8 + DAT_800b25da;
        sVar1 = DAT_800b2608;
        goto LAB_80048664;
      case 4:
        psVar7[-1] = *psVar9 + DAT_800b25f0;
        *psVar7 = *psVar8 + DAT_800b25f2;
        goto LAB_80048684;
      case 5:
        if (uVar10 == 0) {
          psVar7[-1] = *(short *)(&DAT_80075390 + (uint)DAT_800b25bc * 0xc) + DAT_800b25e8;
          *psVar7 = *(short *)(&DAT_80075392 + (uint)DAT_800b25bc * 0xc) + DAT_800b25ea;
          if (DAT_800b25ca == '\x01') {
            psVar7[2] = DAT_800b261a;
            break;
          }
        }
        else {
          psVar7[-1] = *(short *)(&DAT_80075390 + (uint)DAT_800b25d6 * 0xc) + DAT_800b25ec;
          *psVar7 = *(short *)(&DAT_80075392 + (uint)DAT_800b25d6 * 0xc) + DAT_800b25ee;
        }
        psVar7[2] = DAT_800b261c;
        break;
      case 6:
        uVar3 = 0x80;
        if ((DAT_800b25ca == '\x01') && (uVar3 = 0x40, uVar10 == DAT_800b25bc)) {
          uVar3 = 0x80;
        }
        *(undefined1 *)(psVar7 + -3) = uVar3;
        *(undefined1 *)((int)psVar7 + -5) = uVar3;
        *(undefined1 *)(psVar7 + -2) = uVar3;
        psVar7[-1] = *psVar9 + DAT_800b25e8;
        *psVar7 = *psVar8 + DAT_800b25ea;
        if (((uVar10 != 9) || (DAT_800b25bc != 1)) && ((uVar10 != 10 || (DAT_800b25bc != 3))))
        break;
        goto LAB_80048684;
      case 7:
        if ((uVar10 == 0) && (1 < (DAT_800b25c0 >> 0x10 & 0xff) - 5)) {
          if ((byte)(&DAT_800b10ac)[(uint)DAT_800b25bd * 4] < 0x15) {
            *(undefined1 *)(psVar7 + 1) = 0x10;
            uVar3 = 0x90;
          }
          else {
            *(undefined1 *)(psVar7 + 1) = 0x68;
            uVar3 = 0xc0;
          }
          *(undefined1 *)((int)psVar7 + 3) = uVar3;
          if (DAT_800b25bd == DAT_800b25c8) {
            *(undefined1 *)(psVar7 + 1) = 0x10;
            *(undefined1 *)((int)psVar7 + 3) = 0xa0;
          }
        }
        psVar7[-1] = *psVar9 + DAT_800b25ec;
        sVar4 = *psVar8 + DAT_800b25ee;
        goto LAB_80048668;
      case 8:
        psVar7[-1] = (&DAT_80076274)[(uint)DAT_800b25bd * 2] +
                     *(short *)(&DAT_800754c8 + (uint)local_30 * 0xc) + DAT_800b25e0;
        *psVar7 = (&DAT_80076276)[(uint)DAT_800b25bd * 2] +
                  *(short *)(&DAT_800754ca + (uint)local_30 * 0xc) + DAT_800b25e2;
        goto joined_r0x80047fd4;
      case 9:
        if (uVar10 == 0) {
          iVar5 = (uint)local_30 * 0xc;
          psVar7[-1] = (short)DAT_800b25d0 + (&DAT_80076274)[(uint)DAT_800b25be * 2] +
                       *(short *)(&DAT_800755e8 + iVar5) + DAT_800b25e0;
          cVar2 = DAT_800b25d1;
        }
        else {
          iVar5 = (uint)local_30 * 0xc;
          psVar7[-1] = (short)DAT_800b25d2 + (&DAT_80076274)[(uint)DAT_800b25be * 2] +
                       *(short *)(&DAT_800755e8 + iVar5) + DAT_800b25e0;
          cVar2 = DAT_800b25d3;
        }
        *psVar7 = (short)cVar2 + (&DAT_80076276)[(uint)DAT_800b25be * 2] +
                  *(short *)(&DAT_800755ea + iVar5) + DAT_800b25e2;
joined_r0x80047fd4:
        if (DAT_800b25ca == '\x01') break;
        goto LAB_80048684;
      case 10:
        psVar7[-1] = *psVar9 + DAT_800b25e4;
        sVar4 = *psVar8 + DAT_800b25e6;
        goto LAB_80048668;
      case 0xb:
        if ((DAT_800b25c0 & 0xffffff) == 0x10100) break;
        goto LAB_80048684;
      case 0xc:
        psVar7[-1] = *psVar9;
        *psVar7 = *psVar8;
        break;
      case 0xd:
        psVar7[-1] = *psVar9 + DAT_800b25e0;
        *psVar7 = *psVar8 + DAT_800b25e2;
        break;
      case 0xe:
        if (DAT_800b25ca == '\x01') {
          psVar7[-1] = *psVar9 + DAT_800b25f8;
          sVar4 = *psVar8 + DAT_800b25fa;
          goto LAB_80048668;
        }
        goto LAB_80048684;
      case 0xf:
        psVar7[-1] = *(short *)(&DAT_80075390 + (uint)DAT_800b25bc * 0xc) + DAT_800b25e8;
        sVar4 = *(short *)(&DAT_80075392 + (uint)DAT_800b25bc * 0xc) + DAT_800b25ea;
        goto LAB_80048668;
      case 0x10:
        psVar7[-1] = *psVar9 + DAT_800b25e8;
        sVar4 = *psVar8 + DAT_800b25ea;
        goto LAB_80048668;
      case 0x11:
        psVar7[-1] = (&DAT_80076274)[(uint)DAT_800b25bd * 2] +
                     *(short *)(&DAT_800754c8 + (uint)local_30 * 0xc) + DAT_800b25e0;
        sVar4 = *(short *)(&DAT_800754ca + (uint)local_30 * 0xc);
        iVar5 = (uint)DAT_800b25bd << 2;
        goto LAB_80048648;
      case 0x12:
      case 0x18:
        psVar7[-1] = (&DAT_80076274)[(uint)DAT_800b25be * 2] +
                     *(short *)(&DAT_800755e8 + (uint)local_30 * 0xc) + DAT_800b25e0;
        sVar4 = *(short *)(&DAT_800755ea + (uint)local_30 * 0xc);
        iVar5 = (uint)DAT_800b25be << 2;
        goto LAB_80048648;
      case 0x13:
        psVar7[-1] = *psVar9;
        *psVar7 = *psVar8;
        break;
      case 0x14:
        psVar7[-1] = *psVar9 + DAT_800b25e0;
        *psVar7 = *psVar8 + DAT_800b25e2;
        break;
      case 0x15:
        psVar7[-1] = *(short *)(&DAT_80075390 + (uint)DAT_800b25bc * 0xc) + DAT_800b25e8;
        sVar4 = *(short *)(&DAT_80075392 + (uint)DAT_800b25bc * 0xc) + DAT_800b25ea;
        goto LAB_80048668;
      case 0x16:
        psVar7[-1] = *psVar9 + DAT_800b25e8;
        sVar4 = *psVar8 + DAT_800b25ea;
        goto LAB_80048668;
      case 0x17:
        psVar7[-1] = (&DAT_80076274)[(uint)DAT_800b25bd * 2] +
                     *(short *)(&DAT_800754c8 + (uint)local_30 * 0xc) + DAT_800b25e0;
        sVar4 = *(short *)(&DAT_800754ca + (uint)local_30 * 0xc);
        iVar5 = (uint)DAT_800b25bd << 2;
LAB_80048648:
        sVar4 = *(short *)((int)&DAT_80076276 + iVar5) + sVar4;
        sVar1 = DAT_800b25e2;
LAB_80048664:
        sVar4 = sVar4 + sVar1;
LAB_80048668:
        *psVar7 = sVar4;
        break;
      default:
        goto LAB_80048684;
      }
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p);
LAB_80048684:
      psVar7 = psVar7 + 0x14;
      p = p + 0x28;
      psVar8 = psVar8 + 6;
      uVar10 = uVar10 + 1;
      local_30 = local_30 + 1;
      psVar9 = psVar9 + 6;
    } while (uVar10 < *(ushort *)(&DAT_80074a8e + (uint)param_1 * 0xc));
  }
  return p;
}
