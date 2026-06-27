MATRIX * RotMatrix(SVECTOR *r,MATRIX *m)

{
  short sVar1;
  MATRIX *pMVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  short sVar8;
  int iVar9;
  
  if (r->vx < 0) {
    pMVar2 = (MATRIX *)FGO_01_OBJ_64();
    return pMVar2;
  }
  iVar6 = (int)(short)*(int *)(&DAT_800794c4 + ((int)r->vx & 0xfffU) * 4);
  iVar3 = *(int *)(&DAT_800794c4 + ((int)r->vx & 0xfffU) * 4) >> 0x10;
  if (-1 < r->vy) {
    sVar8 = (short)*(int *)(&DAT_800794c4 + ((int)r->vy & 0xfffU) * 4);
    iVar9 = -(int)sVar8;
    iVar4 = *(int *)(&DAT_800794c4 + ((int)r->vy & 0xfffU) * 4) >> 0x10;
    sVar1 = r->vz;
    m->m[0][2] = sVar8;
    m->m[1][2] = (short)(-(iVar4 * iVar6) >> 0xc);
    sVar8 = (short)(iVar4 * iVar3 >> 0xc);
    if (-1 < sVar1) {
      m->m[2][2] = sVar8;
      iVar7 = (int)(short)*(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4);
      iVar5 = *(int *)(&DAT_800794c4 + ((int)sVar1 & 0xfffU) * 4) >> 0x10;
      m->m[0][0] = (short)(iVar5 * iVar4 >> 0xc);
      m->m[0][1] = (short)(-(iVar7 * iVar4) >> 0xc);
      iVar4 = iVar5 * iVar9 >> 0xc;
      m->m[1][0] = (short)(iVar7 * iVar3 >> 0xc) - (short)(iVar4 * iVar6 >> 0xc);
      m->m[2][0] = (short)(iVar7 * iVar6 >> 0xc) + (short)(iVar4 * iVar3 >> 0xc);
      iVar9 = iVar7 * iVar9 >> 0xc;
      m->m[1][1] = (short)(iVar5 * iVar3 >> 0xc) + (short)(iVar9 * iVar6 >> 0xc);
      m->m[2][1] = (short)(iVar5 * iVar6 >> 0xc) - (short)(iVar9 * iVar3 >> 0xc);
      return m;
    }
    m->m[2][2] = sVar8;
    pMVar2 = (MATRIX *)FGO_01_OBJ_160();
    return pMVar2;
  }
  pMVar2 = (MATRIX *)FGO_01_OBJ_CC();
  return pMVar2;
}
