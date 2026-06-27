/* FUN_800470c0 @ 0x800470c0  (Ghidra headless, raw MIPS overlay) */

uint * FUN_800470c0(undefined4 *param_1,short param_2,int param_3,uint param_4)

{
  byte bVar1;
  int iVar2;
  byte bVar3;
  undefined *puVar4;
  uint *puVar5;
  uint *puVar6;
  undefined4 *puVar7;
  uint uVar8;
  
  if (DAT_800cfbf3 != '\0') {
    uVar8 = param_4 >> 0x1c;
    puVar7 = &DAT_800cfe1c;
    puVar6 = (uint *)0x0;
    do {
      puVar5 = (uint *)*puVar7;
      bVar1 = (byte)(param_4 >> 0x18);
      if (((((*puVar5 & 1) != 0) && (*(char *)((int)puVar5 + 0x1d3) == '\0')) &&
          (-1 < *(short *)((int)puVar5 + 0x156))) &&
         (((*(ushort *)((int)puVar5 + 0x10e) & 0xc000) == 0 &&
          (*(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] & 0xff00,
          (puVar5[0xf] + (int)(short)puVar5[0x26] + 100 + (uint)*(ushort *)((int)puVar5 + 0x9e)) -
          param_1[1] < (*(ushort *)((int)puVar5 + 0x9e) + 100) * 2)))) {
        *(short *)(param_3 + 6) = *(short *)(param_3 + 6) + (*(short *)((int)puVar5 + 0x1ee) >> 2);
        *(short *)(param_3 + 4) = *(short *)(param_3 + 4) + (*(short *)((int)puVar5 + 0x1ee) >> 2);
        iVar2 = FUN_80041ef8(param_1,(int)param_2,puVar5 + 0x21,param_3);
        if (iVar2 == 0) {
          *(short *)(param_3 + 6) = *(short *)(param_3 + 6) - (*(short *)((int)puVar5 + 0x1ee) >> 2)
          ;
          *(short *)(param_3 + 4) = *(short *)(param_3 + 4) - (*(short *)((int)puVar5 + 0x1ee) >> 2)
          ;
        }
        else {
          *(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] | 1;
          if ((param_4 & 0x10000) == 0) goto LAB_80047434;
          puVar4 = (&PTR_DAT_800a6a88)[(byte)puVar5[2]];
          *(ushort *)((int)puVar5 + 0x156) =
               *(short *)((int)puVar5 + 0x156) -
               ((ushort)(*(uint *)(puVar4 + (param_4 & 0xffff) * 0x14 + -0x14) >>
                        (uVar8 * 10 & 0x1f)) & 0x3ff);
          if (puVar5[1] != 0xc02) {
            puVar5[0x7f] = puVar5[1];
          }
          puVar5[1] = 2;
          if (*(short *)((int)puVar5 + 0x156) < 0) {
            puVar5[1] = 3;
          }
          *(undefined1 *)((int)puVar5 + 0x1d2) = 1;
          if ((param_4 & 0x20000) != 0) {
            iVar2 = (int)((uint)(ushort)puVar5[0x26] << 0x10) >> 0x11;
            if ((int)(puVar5[0xf] + iVar2) < (int)param_1[1]) {
              *(undefined1 *)((int)puVar5 + 0x1d2) = 0;
            }
            if (((*puVar5 & 0x10000000) != 0) && ((int)param_1[1] < (int)(puVar5[0xf] + iVar2 * 3)))
            {
              *(undefined1 *)((int)puVar5 + 0x1d2) = 2;
            }
          }
          *(char *)((int)puVar5 + 5) = (char)param_4;
          bVar3 = *(byte *)((int)puVar5 + 0x1d3) & 0x80;
          *(byte *)((int)puVar5 + 0x1d2) =
               *(char *)((int)puVar5 + 0x1d2) + (char)(uVar8 << 1) + (bVar1 >> 4);
          *(byte *)((int)puVar5 + 0x1d3) = bVar3;
          *(byte *)((int)puVar5 + 0x1d3) =
               bVar3 | (byte)(*(uint *)((int)(puVar4 + (param_4 & 0xffff) * 0x14 + -0x14) + 4) >> 9)
                       & 0x7f;
          iVar2 = FUN_800154ac(*param_1,param_1[2],puVar5[0xe],puVar5[0x10]);
          iVar2 = iVar2 - *(short *)((int)puVar5 + 0x76);
          if ((iVar2 + 0x400U & 0xfff) < 0x800) {
            *(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] | 0x20;
          }
          if ((iVar2 + 0x600U & 0xfff) < 0x400) {
            *(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] | 0x40;
          }
          puVar6 = puVar5;
          if ((iVar2 - 0x200U & 0xfff) < 0x400) {
            *(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] | 0x80;
          }
        }
      }
      puVar5 = puVar6;
      puVar7 = puVar7 + 1;
      puVar6 = puVar5;
    } while (puVar7 != DAT_800ce334);
    if ((param_4 & 0x10000) != 0) {
      return puVar5;
    }
LAB_80047434:
    if (puVar5 != (uint *)0x0) {
      puVar4 = (&PTR_DAT_800a6a88)[(byte)puVar5[2]];
      *(ushort *)((int)puVar5 + 0x156) =
           *(short *)((int)puVar5 + 0x156) -
           ((ushort)(*(uint *)(puVar4 + (param_4 & 0xffff) * 0x14 + -0x14) >> (uVar8 * 10 & 0x1f)) &
           0x3ff);
      if (1 < puVar5[1] - 0xc02) {
        puVar5[0x7f] = puVar5[1];
      }
      puVar5[1] = 2;
      if (*(short *)((int)puVar5 + 0x156) < 0) {
        puVar5[1] = 3;
      }
      *(undefined1 *)((int)puVar5 + 0x1d2) = 1;
      if ((param_4 & 0x20000) != 0) {
        iVar2 = (int)((uint)(ushort)puVar5[0x26] << 0x10) >> 0x11;
        if ((int)(puVar5[0xf] + iVar2) < (int)param_1[1]) {
          *(undefined1 *)((int)puVar5 + 0x1d2) = 0;
        }
        if (((*puVar5 & 0x10000000) != 0) && ((int)param_1[1] < (int)(puVar5[0xf] + iVar2 * 3))) {
          *(undefined1 *)((int)puVar5 + 0x1d2) = 2;
        }
      }
      if (((param_4 & 0x40000) != 0) &&
         (*(char *)((int)puVar5 + 0x106) == *(char *)(DAT_800ce330 + 0x106))) {
        *(undefined1 *)((int)puVar5 + 0x1d2) = 0;
      }
      *(char *)((int)puVar5 + 5) = (char)param_4;
      bVar3 = *(byte *)((int)puVar5 + 0x1d3) & 0x80;
      *(byte *)((int)puVar5 + 0x1d2) = *(char *)((int)puVar5 + 0x1d2) + (bVar1 >> 4) * '\x03';
      *(byte *)((int)puVar5 + 0x1d3) = bVar3;
      *(byte *)((int)puVar5 + 0x1d3) =
           bVar3 | (byte)(*(uint *)((int)(puVar4 + (param_4 & 0xffff) * 0x14 + -0x14) + 4) >> 9) &
                   0x7f;
      iVar2 = FUN_800154ac(*param_1,param_1[2],puVar5[0xe],puVar5[0x10]);
      iVar2 = iVar2 - *(short *)((int)puVar5 + 0x76);
      if ((iVar2 + 0x400U & 0xfff) < 0x800) {
        *(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] | 0x20;
      }
      if ((iVar2 + 0x600U & 0xfff) < 0x400) {
        *(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] | 0x40;
      }
      if (0x3ff < (iVar2 - 0x200U & 0xfff)) {
        return puVar5;
      }
      *(ushort *)(puVar5 + 0x74) = (ushort)puVar5[0x74] | 0x80;
      return puVar5;
    }
  }
  return (uint *)0x0;
}


