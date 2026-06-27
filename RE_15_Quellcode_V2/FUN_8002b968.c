void FUN_8002b968(void)

{
  byte bVar1;
  undefined1 uVar2;
  byte bVar3;
  char cVar4;
  u_short uVar5;
  int iVar6;
  char *pcVar7;
  undefined4 *puVar8;
  uint uVar9;
  DR_MOVE *p;
  uint uVar10;
  int iVar11;
  ushort uVar12;
  uint uVar13;
  int *piVar14;
  uint *puVar15;
  
  bVar1 = *(byte *)(DAT_800ac778 + 2);
  puVar15 = &DAT_800b3f98;
  piVar14 = &DAT_800b3fb4;
  if ((uint *)((int)&DAT_800b3f98 + 1) <= &DAT_800b3f98 + (uint)bVar1 * 0x25) {
    do {
      if ((*puVar15 & 1) != 0) {
        uVar13 = 0;
        pcVar7 = &DAT_80073c6c;
        cVar4 = '\x03';
code_r0x8002ba18:
        pcVar7 = pcVar7 + 3;
        if (cVar4 != (char)piVar14[-5]) goto code_r0x8002ba20;
        uVar9 = uVar13 / 3;
        uVar5 = GetClut(0x130,DAT_800b281e + 0x1e0);
        if (*(int *)(piVar14[-3] + 0x14) != 0) {
          uVar13 = *(uint *)(piVar14[-2] + 0xc) >> 0x16;
        }
        uVar12 = (ushort)uVar13;
        uVar13 = *(int *)(piVar14[-3] + 0x14) << 1;
        uVar10 = 0;
        if (uVar13 != 0) {
          puVar8 = (undefined4 *)(piVar14[-2] + 0xc);
          do {
            uVar10 = uVar10 + 1;
            *puVar8 = CONCAT22(uVar5,*(undefined2 *)puVar8);
            puVar8 = puVar8 + 10;
          } while (uVar10 < uVar13);
        }
        uVar13 = 0;
        if (*(int *)(piVar14[-1] + 0x14) != 0) {
          uVar12 = (ushort)((uint)*(undefined4 *)(*piVar14 + 0xc) >> 0x16);
        }
        uVar10 = *(int *)(piVar14[-1] + 0x14) << 1;
        if (uVar10 != 0) {
          puVar8 = (undefined4 *)(*piVar14 + 0xc);
          do {
            uVar13 = uVar13 + 1;
            *puVar8 = CONCAT22(uVar5,*(undefined2 *)puVar8);
            puVar8 = puVar8 + 0xd;
          } while (uVar13 < uVar10);
        }
        uVar13 = 0;
        iVar6 = (uint)DAT_800b281e * 0x34;
        (&DAT_800b267c)[iVar6] = 0;
        (&DAT_800b267e)[iVar6] = (&DAT_80073c6d)[uVar9];
        uVar2 = (&DAT_80073c6e)[uVar9];
        iVar11 = 4;
        (&DAT_800b267d)[iVar6] = 0;
        (&DAT_800b267f)[iVar6] = uVar2;
        do {
          p = (DR_MOVE *)(&DAT_800b267c + iVar6 + iVar11);
          SetDrawMove(p);
          iVar11 = iVar11 + 0x18;
          uVar13 = uVar13 + 1;
          p->sx = 0;
          p->sy = uVar12;
          p->x0 = 0x130;
          bVar3 = DAT_800b281e;
          p->h = 1;
          p->w = 0x10;
          p->y0 = bVar3 + 0x1e0;
        } while (uVar13 < 2);
        DAT_800b281e = DAT_800b281e + 1;
      }
LAB_8002bbbc:
      puVar15 = puVar15 + 0x25;
      piVar14 = piVar14 + 0x25;
    } while (puVar15 < &DAT_800b3f98 + (uint)bVar1 * 0x25);
  }
  return;
code_r0x8002ba20:
  cVar4 = *pcVar7;
  uVar13 = uVar13 + 3;
  if (cVar4 == -1) goto LAB_8002bbbc;
  goto code_r0x8002ba18;
}
