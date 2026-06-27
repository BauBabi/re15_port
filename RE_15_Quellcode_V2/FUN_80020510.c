void FUN_80020510(SVECTOR *param_1,SVECTOR *param_2,SVECTOR *param_3,long param_4)

{
  short sVar1;
  ushort uVar2;
  
  uVar2 = param_1->vx & 0xfff;
  param_1->vx = uVar2;
  uVar2 = (param_2->vx - uVar2) + 0x800;
  if (0x1000 < uVar2) {
    sVar1 = 0x1000;
    if ((uVar2 & 0x8000) == 0) {
      sVar1 = -0x1000;
    }
    param_2->vx = param_2->vx + sVar1;
  }
  uVar2 = param_1->vy & 0xfff;
  param_1->vy = uVar2;
  uVar2 = (param_2->vy - uVar2) + 0x800;
  if (0x1000 < uVar2) {
    sVar1 = 0x1000;
    if ((uVar2 & 0x8000) == 0) {
      sVar1 = -0x1000;
    }
    param_2->vy = param_2->vy + sVar1;
  }
  uVar2 = param_1->vz & 0xfff;
  param_1->vz = uVar2;
  uVar2 = (param_2->vz - uVar2) + 0x800;
  if (0x1000 < uVar2) {
    sVar1 = 0x1000;
    if ((uVar2 & 0x8000) == 0) {
      sVar1 = -0x1000;
    }
    param_2->vz = param_2->vz + sVar1;
  }
  LoadAverageShort12(param_1,param_2,0x1000 - param_4,param_4,param_3);
  return;
}
