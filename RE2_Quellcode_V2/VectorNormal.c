/* VectorNormal @ 0x8008d424  (Ghidra headless, raw MIPS overlay) */

long VectorNormal(VECTOR *_2,VECTOR *_3)

{
  long lVar1;
  long lVar2;
  long lVar3;
  long lVar4;
  
  lVar2 = _2->vx;
  lVar3 = _2->vy;
  lVar4 = _2->vz;
  lVar1 = MSC02_OBJ_FC();
  _3->vx = lVar2;
  _3->vy = lVar3;
  _3->vz = lVar4;
  return lVar1;
}


