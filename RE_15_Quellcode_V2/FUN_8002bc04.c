void FUN_8002bc04(void)

{
  uint uVar1;
  uint uVar2;
  undefined *puVar3;
  
  uVar2 = 0;
  if (DAT_800b281e != 0) {
    puVar3 = &DAT_800b267c;
    do {
      if ((byte)puVar3[2] == 0) {
        trap(0x1c00);
      }
      *(short *)(puVar3 + (uint)DAT_800aca34 * 0x18 + 0x10) =
           (short)((uint)(byte)puVar3[1] / (uint)(byte)puVar3[2] << 4);
      puVar3[1] = puVar3[1] + '\x01';
      uVar2 = uVar2 + 1;
      if ((uint)(byte)puVar3[1] == (uint)(byte)puVar3[3] * (uint)(byte)puVar3[2]) {
        puVar3[1] = 0;
      }
      uVar1 = (uint)DAT_800aca34;
                    /* Probable PsyQ macro: addPrim(). */
      *(uint *)(puVar3 + uVar1 * 0x18 + 4) =
           *(uint *)(puVar3 + uVar1 * 0x18 + 4) & 0xff000000 |
           *(uint *)(&DAT_800ac6d8 + uVar1 * 0x40) & 0xffffff;
      *(uint *)(&DAT_800ac6d8 + (uint)DAT_800aca34 * 0x40) =
           *(uint *)(&DAT_800ac6d8 + (uint)DAT_800aca34 * 0x40) & 0xff000000 |
           (uint)(puVar3 + (uint)DAT_800aca34 * 0x18 + 4) & 0xffffff;
      puVar3 = puVar3 + 0x34;
    } while (uVar2 < DAT_800b281e);
  }
  return;
}
