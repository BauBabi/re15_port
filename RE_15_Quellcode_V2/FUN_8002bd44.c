void FUN_8002bd44(void)

{
  int iVar1;
  uint *puVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar6;
  short sVar7;
  short sVar8;
  uint *puVar5;
  
  uVar6 = (uint)*(byte *)(DAT_800ac778 + 2);
  if (uVar6 != 0) {
    DAT_800aca3c = DAT_800aca3c & 0xffffdfff;
    FUN_8002bc04();
    puVar4 = &DAT_800b3f98 + uVar6 * 0x25;
    do {
      puVar5 = puVar4 + -0x25;
      uVar6 = uVar6 - 1;
      if ((*puVar5 & 1) != 0) {
        if ((byte)puVar4[-0x23] != 0) {
          (**(code **)(&DAT_80073c70 + (uint)(byte)puVar4[-0x23] * 4))(puVar5);
        }
        uVar3 = (uint)DAT_800aca4e;
        puVar2 = &DAT_800acc2c;
        while (uVar3 != 0) {
          if ((*puVar2 & 1) != 0) {
            uVar3 = uVar3 - 1;
            FUN_8002cabc(puVar2,puVar5,0);
          }
          puVar2 = puVar2 + 0x7d;
        }
        uVar3 = 0;
        puVar2 = &DAT_800b3f98;
        if (*(char *)(DAT_800ac778 + 2) != '\0') {
          do {
            if ((puVar5 != puVar2) && ((*puVar2 & 1) != 0)) {
              FUN_8002cdf4(puVar2,puVar5);
            }
            uVar3 = uVar3 + 1;
            puVar2 = puVar2 + 0x25;
          } while (uVar3 < *(byte *)(DAT_800ac778 + 2));
        }
        if ((*puVar5 & 0x200) != 0) {
          FUN_8002cabc(&DAT_800aca54,puVar5,1);
          sVar7 = (short)puVar4[-0x18];
          sVar8 = (short)puVar4[-0x16];
          goto LAB_8002c0b8;
        }
        sVar7 = (short)puVar4[-0x18];
        sVar8 = (short)puVar4[-0x16];
        if (puVar5 == DAT_800ac788) {
          FUN_8002cfd4(&DAT_800aca54,puVar5);
        }
        else {
          if (((((char)puVar4[-0x23] == '\x04') && ((DAT_800ac768 & 1) != 0)) &&
              (DAT_800acad6 == *(char *)((int)puVar4 + -0x12))) &&
             (iVar1 = FUN_8002d1e8(&DAT_800aca54,puVar5,0), iVar1 != 0)) {
            *(char *)(puVar4 + -2) = (char)puVar4[-2] + '\x01';
          }
          else {
            *(undefined1 *)(puVar4 + -2) = 0;
          }
          if (((*puVar5 & 0x80) == 0) && ((char)puVar4[-2] == '\t')) {
            FUN_8002cabc(&DAT_800aca54,puVar5,1);
            iVar1 = FUN_8003b558(puVar5,2);
            if (iVar1 != 0) {
LAB_8002c0b0:
              *(undefined1 *)(puVar4 + -2) = 10;
              goto LAB_8002c0b8;
            }
            *(undefined1 *)(puVar4 + -2) = 8;
            DAT_800aca3c = DAT_800aca3c | 0x2000;
            if (DAT_800aca59 != '\b') goto LAB_8002c0b8;
            uVar3 = (uint)DAT_800aca4e;
            puVar2 = &DAT_800acc2c;
            while (uVar3 != 0) {
              if ((*puVar2 & 1) != 0) {
                uVar3 = uVar3 - 1;
                iVar1 = FUN_8002cabc(puVar2,puVar5,1);
                if (iVar1 != 0) {
                  *(undefined1 *)(puVar4 + -2) = 10;
                }
              }
              puVar2 = puVar2 + 0x7d;
            }
            uVar3 = 0;
            puVar2 = &DAT_800b3f98;
            if (*(char *)(DAT_800ac778 + 2) != '\0') {
              do {
                uVar3 = uVar3 + 1;
                if (((puVar5 != puVar2) && ((*puVar2 & 1) != 0)) &&
                   (iVar1 = FUN_8002cdf4(puVar2,puVar5), iVar1 != 0)) goto LAB_8002c0b0;
                puVar2 = puVar2 + 0x25;
              } while (uVar3 < *(byte *)(DAT_800ac778 + 2));
            }
          }
          else {
LAB_8002c0b8:
            puVar4[-0x18] = (int)sVar7;
            puVar4[-0x16] = (int)sVar8;
            FUN_8003b558(puVar5,2);
          }
          FUN_8002cabc(&DAT_800aca54,puVar5,0);
        }
        *(short *)((int)puVar4 + -0x52) = (short)puVar4[-0x17];
        *(short *)(puVar4 + -0x15) = (short)puVar4[-0x18];
        *(short *)(puVar4 + -0x14) = (short)puVar4[-0x16];
        if ((((char)puVar4[-0x23] == '\x04') && ((char)puVar4[-0x24] == '\0')) &&
           (uVar3 = FUN_8003b7f0(puVar4 + -0x18,0x1c2,*(undefined1 *)((int)puVar4 + -0x12)),
           (uVar3 & 2) != 0)) {
          FUN_8002d3d8(puVar5);
          *(undefined1 *)(puVar4 + -0x24) = 1;
          *(undefined1 *)((int)puVar4 + -0x8f) = 0;
        }
      }
      *(undefined2 *)(puVar4 + -3) = 0;
      puVar4 = puVar5;
    } while (uVar6 != 0);
  }
  return;
}
