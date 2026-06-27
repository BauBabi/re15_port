/* FUN_800410cc @ 0x800410cc  (Ghidra headless, raw MIPS overlay) */

uint * FUN_800410cc(int param_1,uint param_2,int *param_3)

{
  short sVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  uint uVar7;
  int iVar8;
  uint *puVar9;
  int iVar10;
  uint unaff_s7;
  undefined4 *puVar11;
  uint local_a0;
  undefined4 local_9c;
  uint local_98;
  uint local_90;
  uint local_88;
  int local_80;
  int local_7c;
  int local_78;
  uint local_70;
  uint local_68;
  uint *local_60;
  uint *local_58;
  uint local_50;
  uint *local_48;
  uint local_40;
  uint local_38;
  int local_30;
  
  local_68 = 0;
  local_50 = 0x7fffffff;
  local_48 = (uint *)0x0;
  if (DAT_800cfbf3 != '\0') {
    if (*(char *)(param_1 + 0x1ec) == '\0') {
      bVar2 = *(char *)(param_1 + 0x1ed) + 1;
      *(byte *)(param_1 + 0x1ed) = bVar2;
      *(undefined1 *)(param_1 + 0x1ec) = *(undefined1 *)((uint)bVar2 * 2 + param_3[1] + 1);
    }
    uVar7 = (uint)*(byte *)((uint)*(byte *)(param_1 + 0x1ed) * 2 + param_3[1]);
    if (uVar7 == 0xff) {
      *(char *)(param_1 + 0x1ec) = *(char *)(param_1 + 0x1ec) + -1;
      return (uint *)0x0;
    }
    if (uVar7 == 0xfe) {
      *(undefined1 *)(param_1 + 0x1ed) = 0;
      *(undefined1 *)(param_1 + 0x1ec) = *(undefined1 *)(param_3[1] + 1);
      uVar7 = (uint)*(byte *)((uint)*(byte *)(param_1 + 0x1ed) * 2 + param_3[1]);
      if (uVar7 == 0xff) {
        return (uint *)0x0;
      }
    }
    if (*(char *)(param_1 + 0x1ec) != -1) {
      puVar11 = &DAT_800cfe1c;
      iVar10 = *param_3 + uVar7 * 0x1c;
      local_30 = *(int *)(param_1 + 0x198) + (uint)*(byte *)(param_1 + 0x1c1) * 0xac + 0x5c;
      local_78 = param_1;
      local_70 = param_2;
      do {
        puVar9 = (uint *)*puVar11;
        if (((((*puVar9 & 1) != 0) && (*(char *)((int)puVar9 + 0x1d3) == '\0')) &&
            (-1 < *(short *)((int)puVar9 + 0x156))) &&
           ((*(ushort *)((int)puVar9 + 0x10e) & 0xc000) == 0)) {
          local_a0 = puVar9[0x21];
          local_9c = *(undefined4 *)
                      ((uint)*(byte *)((int)puVar9 + 0x1c1) * 0xac + puVar9[0x66] + 0x60);
          local_98 = puVar9[0x23];
          iVar3 = FUN_80050858(local_30,&local_a0,0x80,1);
          uVar7 = 0;
          if (iVar3 == 0) {
            local_38 = (uint)*(ushort *)((int)puVar9 + 0x9a);
            sVar1 = (short)((int)local_38 >> 2);
            *(short *)(iVar10 + 10) = *(short *)(iVar10 + 10) + sVar1;
            *(short *)(iVar10 + 0x1a) = *(short *)(iVar10 + 0x1a) + sVar1;
            *(short *)(iVar10 + 0x12) = *(short *)(iVar10 + 0x12) + sVar1;
            *(short *)(iVar10 + 8) = *(short *)(iVar10 + 8) + (*(short *)((int)puVar9 + 0x1ee) >> 2)
            ;
            local_90 = puVar9[0xe];
            local_88 = puVar9[0x10];
            puVar9[0xe] = puVar9[0x21];
            puVar9[0x10] = puVar9[0x23];
            *(ushort *)(puVar9 + 0x74) = (ushort)puVar9[0x74] & 0xff00;
            local_60 = (uint *)((&PTR_DAT_800a6a88)[(byte)puVar9[2]] + (local_70 >> 0x10) * 0x14);
            uVar5 = *puVar9 >> 0x1a & 7;
            iVar3 = puVar9[0xf] - *(int *)(local_78 + 0x3c);
            if (uVar5 != 0) {
              if ((*(char *)(iVar10 + 1) != '\0') &&
                 (iVar4 = FUN_80041b20(local_60,*(char *)(iVar10 + 1),&local_80,&local_7c),
                 (uint)(iVar3 - local_80) <= (uint)(local_7c - local_80))) {
                bVar2 = *(byte *)(iVar10 + 1);
                iVar8 = 6;
                if (local_7c + iVar4 * 2 <= iVar3) {
                  iVar8 = 3;
                }
                if (local_7c + iVar4 <= iVar3) {
                  iVar8 = 0;
                }
                iVar4 = FUN_80041ce4(local_78 + 0x10,puVar9 + 4,iVar10 + 4);
                if (iVar4 != 0) {
                  pbVar6 = &DAT_800a6db4 + iVar8 + (uint)((bVar2 & 8) != 0) * 9;
                  if ((pbVar6[2] & uVar5) != 0) {
                    unaff_s7 = (uint)(pbVar6[2] >> 1);
                    uVar7 = unaff_s7 * 8 + 1;
                  }
                  if ((pbVar6[1] & uVar5) != 0) {
                    unaff_s7 = (uint)(pbVar6[1] >> 1);
                    uVar7 = unaff_s7 * 8 + 1;
                  }
                  if ((*pbVar6 & uVar5) != 0) {
                    unaff_s7 = (uint)(*pbVar6 >> 1);
                    uVar7 = unaff_s7 * 8 + 1;
                  }
                }
              }
              if ((*(char *)(iVar10 + 2) != '\0') &&
                 (iVar4 = FUN_80041b20(local_60,*(char *)(iVar10 + 2),&local_80,&local_7c),
                 (uint)(iVar3 - local_80) <= (uint)(local_7c - local_80))) {
                bVar2 = *(byte *)(iVar10 + 2);
                iVar8 = 6;
                if (local_7c + iVar4 * 2 <= iVar3) {
                  iVar8 = 3;
                }
                if (local_7c + iVar4 <= iVar3) {
                  iVar8 = 0;
                }
                iVar4 = FUN_80041ce4(local_78 + 0x10,puVar9 + 4,iVar10 + 0xc);
                if (iVar4 != 0) {
                  pbVar6 = &DAT_800a6db4 + iVar8 + (uint)((bVar2 & 8) != 0) * 9;
                  uVar7 = 0;
                  if ((pbVar6[2] & uVar5) != 0) {
                    unaff_s7 = (uint)(pbVar6[2] >> 1);
                    uVar7 = unaff_s7 * 8 + 2;
                  }
                  if ((pbVar6[1] & uVar5) != 0) {
                    unaff_s7 = (uint)(pbVar6[1] >> 1);
                    uVar7 = unaff_s7 * 8 + 2;
                  }
                  if ((*pbVar6 & uVar5) != 0) {
                    unaff_s7 = (uint)(*pbVar6 >> 1);
                    uVar7 = unaff_s7 * 8 + 2;
                  }
                }
              }
              if ((*(char *)(iVar10 + 3) != '\0') &&
                 (iVar4 = FUN_80041b20(local_60,*(char *)(iVar10 + 3),&local_80,&local_7c),
                 (uint)(iVar3 - local_80) <= (uint)(local_7c - local_80))) {
                bVar2 = *(byte *)(iVar10 + 3);
                iVar8 = 6;
                if (local_7c + iVar4 * 2 <= iVar3) {
                  iVar8 = 3;
                }
                if (local_7c + iVar4 <= iVar3) {
                  iVar8 = 0;
                }
                iVar3 = FUN_80041ce4(local_78 + 0x10,puVar9 + 4,iVar10 + 0x14);
                if (iVar3 != 0) {
                  pbVar6 = &DAT_800a6db4 + iVar8 + (uint)((bVar2 & 8) != 0) * 9;
                  uVar7 = 0;
                  if ((pbVar6[2] & uVar5) != 0) {
                    unaff_s7 = (uint)(pbVar6[2] >> 1);
                    uVar7 = unaff_s7 * 8 + 4;
                  }
                  if ((pbVar6[1] & uVar5) != 0) {
                    unaff_s7 = (uint)(pbVar6[1] >> 1);
                    uVar7 = unaff_s7 * 8 + 4;
                  }
                  if ((*pbVar6 & uVar5) != 0) {
                    unaff_s7 = (uint)(*pbVar6 >> 1);
                    uVar7 = unaff_s7 * 8 + 4;
                  }
                }
              }
              *(ushort *)(puVar9 + 0x74) = (ushort)puVar9[0x74] | (ushort)uVar7;
              if ((uVar7 != 0) &&
                 (iVar3 = *(int *)(local_78 + 0x38) - puVar9[0xe],
                 iVar4 = *(int *)(local_78 + 0x40) - puVar9[0x10],
                 uVar5 = SquareRoot0(iVar3 * iVar3 + iVar4 * iVar4), uVar5 < local_50)) {
                local_58 = local_60;
                local_68 = uVar7;
                local_50 = uVar5;
                local_48 = puVar9;
                local_40 = unaff_s7;
              }
            }
            sVar1 = (short)((int)local_38 >> 2);
            *(short *)(iVar10 + 10) = *(short *)(iVar10 + 10) - sVar1;
            *(short *)(iVar10 + 0x1a) = *(short *)(iVar10 + 0x1a) - sVar1;
            *(short *)(iVar10 + 0x12) = *(short *)(iVar10 + 0x12) - sVar1;
            *(short *)(iVar10 + 8) = *(short *)(iVar10 + 8) - (*(short *)((int)puVar9 + 0x1ee) >> 2)
            ;
            puVar9[0xe] = local_90;
            puVar9[0x10] = local_88;
          }
        }
        puVar11 = puVar11 + 1;
      } while (puVar11 != DAT_800ce334);
      *(char *)(local_78 + 0x1ec) = *(char *)(local_78 + 0x1ec) + -1;
      if ((local_70 & 0x8000) != 0) {
        return local_48;
      }
      if (local_48 == (uint *)0x0) {
        return (uint *)0xff;
      }
      uVar7 = (uint)((local_68 & 2) != 0);
      if ((local_68 & 4) != 0) {
        uVar7 = 2;
      }
      if (local_70 >> 0x10 == 0x12) {
        iVar10 = FUN_80015fe8();
        uVar5 = FUN_80015fe8();
        if ((iVar10 >> (uVar5 & 3) & 0xfU) == 0) {
          local_58 = local_58 + -0x46;
          local_70 = 0x50000;
        }
        else {
          local_58 = local_58 + -0x55;
          local_70 = 0x20000;
        }
      }
      uVar5 = local_48[1];
      *(ushort *)((int)local_48 + 0x156) =
           *(short *)((int)local_48 + 0x156) - ((ushort)(*local_58 >> (uVar7 * 10 & 0x1f)) & 0x3ff);
      local_48[1] = 2;
      local_48[0x7f] = uVar5;
      if (*(short *)((int)local_48 + 0x156) < 0) {
        local_48[1] = 3;
      }
      if ((local_40 == 0) &&
         (iVar10 = (uint)*(byte *)((int)local_48 + 0x153) - (local_58[1] >> uVar7 * 3 & 7),
         *(char *)((int)local_48 + 0x153) = (char)iVar10, iVar10 * 0x1000000 < 0)) {
        *(undefined1 *)((int)local_48 + 0x153) = 0xff;
      }
      if ((local_40 == 1) &&
         (iVar10 = (uint)*(byte *)((int)local_48 + 0x152) - (local_58[1] >> uVar7 * 3 & 7),
         *(char *)((int)local_48 + 0x152) = (char)iVar10, iVar10 * 0x1000000 < 0)) {
        *(undefined1 *)((int)local_48 + 0x152) = 0xff;
      }
      if ((local_40 == 2) &&
         (iVar10 = (uint)*(byte *)((int)local_48 + 0x151) - (local_58[1] >> uVar7 * 3 & 7),
         *(char *)((int)local_48 + 0x151) = (char)iVar10, iVar10 * 0x1000000 < 0)) {
        *(undefined1 *)((int)local_48 + 0x151) = 0xff;
      }
      iVar10 = FUN_800154ac(*(undefined4 *)(local_78 + 0x38),*(undefined4 *)(local_78 + 0x40),
                            local_48[0xe],local_48[0x10]);
      iVar10 = iVar10 - *(short *)((int)local_48 + 0x76);
      if ((iVar10 + 0x400U & 0xfff) < 0x800) {
        *(ushort *)(local_48 + 0x74) = (ushort)local_48[0x74] | 0x20;
      }
      if ((iVar10 + 0x600U & 0xfff) < 0x400) {
        *(ushort *)(local_48 + 0x74) = (ushort)local_48[0x74] | 0x40;
      }
      if ((iVar10 - 0x200U & 0xfff) < 0x400) {
        *(ushort *)(local_48 + 0x74) = (ushort)local_48[0x74] | 0x80;
      }
      *(char *)((int)local_48 + 0x1d2) = (char)uVar7 * '\x03' + (char)local_40;
      *(char *)((int)local_48 + 5) = (char)(local_70 >> 0x10) + '\x01';
      bVar2 = *(byte *)((int)local_48 + 0x1d3) & 0x80;
      *(byte *)((int)local_48 + 0x1d3) = bVar2;
      *(byte *)((int)local_48 + 0x1d3) =
           bVar2 | (byte)(local_58[1] >> (uVar7 * 7 + 9 & 0x1f)) & 0x7f;
      return (uint *)(unaff_s7 + 1);
    }
  }
  return (uint *)0x0;
}


