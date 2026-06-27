undefined4 FUN_80027e68(undefined4 param_1,ushort param_2,int param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  ushort uVar3;
  
  uVar1 = DAT_800aca40;
  if ((DAT_800b8520 & 0x80) != 0) {
    return 0xffffffff;
  }
  DAT_800b8520 = 0x80;
  DAT_800b8521 = 0;
  uVar3 = param_2 & 0x300;
  DAT_800b8538 = 3;
  DAT_800b853c = DAT_800aca40;
  uVar2 = param_4 | DAT_800aca40;
  if (uVar3 == 0x200) {
    DAT_800b8534 = 0x22;
    DAT_800b8536 = 0xba;
    DAT_800b8522 = 0x80;
    DAT_800b8528 = &DAT_800c50de + *(ushort *)(&DAT_800c50de + param_3 * 2);
  }
  else {
    if (uVar3 < 0x201) {
      if (uVar3 == 0x100) {
        DAT_800aca40 = uVar2;
        DAT_800b8520 = 0x80;
        DAT_800b8521 = 0;
        DAT_800b8522 = 0x80;
        DAT_800b8528 = &DAT_800c4fc6 + *(ushort *)(&DAT_800c4fc6 + param_3 * 2);
        DAT_800b8534 = 0x22;
        DAT_800b8536 = 0xb4;
        DAT_800b8538 = 3;
        DAT_800b853c = uVar1;
        DAT_800b8540 = param_4;
        return 0;
      }
    }
    else if (uVar3 == 0x300) {
      DAT_800b8534 = 0x22;
      DAT_800b8536 = 0xb4;
      DAT_800b8522 = 0x80;
      DAT_800aca40 = uVar2;
      DAT_800b8540 = param_4;
      DAT_800b8528 = (undefined *)FUN_800c004c(param_3);
      return 0;
    }
    DAT_800b8536 = (undefined2)((uint)param_1 >> 0x10);
    DAT_800b8522 = (byte)param_2 & 0x80;
    DAT_800b8538 = param_2 & 7;
    param_2 = param_2 & 0xc00;
    DAT_800b8534 = (undefined2)param_1;
    if (param_2 == 0x400) {
      DAT_800b8528 = &DAT_800c50de + *(ushort *)(&DAT_800c50de + param_3 * 2);
    }
    else if (param_2 < 0x401) {
      if (param_2 != 0) {
        DAT_800aca40 = uVar2;
        DAT_800b8520 = 0x80;
        DAT_800b8521 = 0;
        DAT_800b853c = uVar1;
        DAT_800b8540 = param_4;
        return 0;
      }
      DAT_800b8528 = &DAT_800c4fc6 + *(ushort *)(&DAT_800c4fc6 + param_3 * 2);
    }
    else if (param_2 != 0x800) {
      if (param_2 != 0xc00) {
        DAT_800aca40 = uVar2;
        DAT_800b8520 = 0x80;
        DAT_800b8521 = 0;
        DAT_800b853c = uVar1;
        DAT_800b8540 = param_4;
        return 0;
      }
      DAT_800b8528 = &DAT_800c5ea0 + *(ushort *)(&DAT_800c5ea0 + param_3 * 2);
    }
  }
  DAT_800b8540 = param_4;
  DAT_800b8521 = 0;
  DAT_800b8520 = 0x80;
  DAT_800aca40 = uVar2;
  return 0;
}
