/* VectorNormalS @ 0x8008d410  (Ghidra headless, raw MIPS overlay) */

long VectorNormalS(VECTOR *_2,SVECTOR *_3)

{
  long lVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  
  sVar2 = (short)_2->vx;
  sVar3 = (short)_2->vy;
  sVar4 = (short)_2->vz;
  lVar1 = MSC02_OBJ_FC();
  _3->vx = sVar2;
  _3->vy = sVar3;
  _3->vz = sVar4;
  return lVar1;
}


