long VectorNormalS(VECTOR *$2,SVECTOR *$3)

{
  long lVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  
  sVar2 = (short)$2->vx;
  sVar3 = (short)$2->vy;
  sVar4 = (short)$2->vz;
  lVar1 = MSC02_OBJ_FC();
  $3->vx = sVar2;
  $3->vy = sVar3;
  $3->vz = sVar4;
  return lVar1;
}
