/* FUN_80042644 @ 0x80042644  (Ghidra headless, raw MIPS overlay) */

void FUN_80042644(int param_1)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  
  if (((*(ushort *)(param_1 + 0x10e) & 0xfff) != 0) &&
     ((byte)(&DAT_800a6e77)[*(ushort *)(param_1 + 0x10e) & 0xfff] != 0)) {
    puVar4 = (uint *)(*(int *)(param_1 + 0x198) +
                     (uint)(byte)(&DAT_800a6e90)
                                 [((byte)(&DAT_800a6e77)[*(ushort *)(param_1 + 0x10e) & 0xfff] - 1)
                                  * 6] * 0xac);
    if (((*(byte *)(param_1 + 8) & 1) != 0) && (*(byte *)(param_1 + 8) < 10)) {
      puVar4 = puVar4 + 0xac;
    }
    if ((*(char *)(param_1 + 8) == '\x0e') || (*(char *)(param_1 + 8) == 'A')) {
      puVar4 = puVar4 + 0x2b;
    }
    if ((*puVar4 & 1) != 0) {
      sVar1 = (short)puVar4[0x21] + 0x14;
      *(short *)(puVar4 + 0x21) = sVar1;
      uVar2 = puVar4[0x18] + (int)sVar1;
      puVar4[0x18] = uVar2;
      if ((int)(short)puVar4[0xe] < (int)uVar2) {
        if (*(char *)((int)puVar4 + 0x79) == '\0') {
          *puVar4 = *puVar4 & 0xfffffffe;
        }
        else {
          puVar4[0x18] = (int)(short)puVar4[0xe];
          iVar3 = (uint)(ushort)puVar4[0x21] << 0x10;
          *(char *)((int)puVar4 + 0x79) = *(char *)((int)puVar4 + 0x79) + -1;
          *(short *)(puVar4 + 0x21) = -(short)((iVar3 >> 0x10) - (iVar3 >> 0x1f) >> 1);
          if (((*(ushort *)((int)puVar4 + 0x86) & 2) != 0) &&
             (uVar2 = FUN_80015fe8(), (uVar2 & 1) != 0)) {
            *(short *)((int)puVar4 + 0x3a) = -*(short *)((int)puVar4 + 0x3a);
            *(short *)(puVar4 + 0xf) = -(short)puVar4[0xf];
          }
          FUN_8005ba28(0x10e0001,puVar4 + 0x17);
        }
      }
      if (*(char *)((int)puVar4 + 0x79) != '\0') {
        if ((*(ushort *)((int)puVar4 + 0x86) & 1) != 0) {
          *(undefined2 *)(puVar4 + 0x1a) = 0x100;
          RotMatrix((SVECTOR *)(puVar4 + 0x1a),(MATRIX *)(puVar4 + 6));
          MulMatrix2((MATRIX *)(puVar4 + 6),(MATRIX *)(puVar4 + 0x12));
          puVar4[0x17] = puVar4[0x17] + (int)*(short *)((int)puVar4 + 0x3a);
          puVar4[0x19] = puVar4[0x19] + (int)(short)puVar4[0xf];
        }
        if (*(char *)((int)puVar4 + 0x79) != '\0') {
          return;
        }
      }
      ((SVECTOR *)(puVar4 + 0x1a))->vx = 0x100;
      RotMatrix((SVECTOR *)(puVar4 + 0x1a),(MATRIX *)(puVar4 + 6));
      MulMatrix2((MATRIX *)(puVar4 + 6),(MATRIX *)(puVar4 + 0x12));
      puVar4[0x17] = puVar4[0x17] + (int)*(short *)((int)puVar4 + 0x3a);
      puVar4[0x19] = puVar4[0x19] + (int)(short)puVar4[0xf];
    }
  }
  return;
}


