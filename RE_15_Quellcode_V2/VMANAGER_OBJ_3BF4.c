int VMANAGER_OBJ_3BF4(uint param_1,uint param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  byte in_t0;
  uint in_t1;
  uint in_t2;
  int in_t3;
  int in_t4;
  int in_t5;
  int in_t6;
  int in_t7;
  int unaff_s1;
  int unaff_s2;
  uint unaff_s3;
  uint unaff_s4;
  short unaff_s5;
  short unaff_s6;
  int iVar4;
  
  while( true ) {
    uVar2 = param_1;
    if ((DAT_800b2678 == 1) && (uVar2 = param_3, param_3 < param_1)) {
      iVar1 = VMANAGER_OBJ_3C20(param_1,param_2,param_1);
      return iVar1;
    }
    uVar3 = (uint)in_t0;
    *(short *)(uVar3 * 0x10 + in_t6) = (short)((param_3 * param_3) / 0x3fff);
    *(short *)(uVar3 * 0x10 + in_t7) = (short)((uVar2 * uVar2) / 0x3fff);
    unaff_s2 = unaff_s2 + 1;
    (&DAT_8008f82c)[uVar3] = (&DAT_8008f82c)[uVar3] | 3;
    do {
      in_t0 = in_t0 + 1;
      uVar2 = (uint)in_t0;
      if (DAT_800b52a8 <= in_t0) {
        return unaff_s2;
      }
    } while ((((&DAT_8008f852)[uVar2 * 0x1a] != unaff_s5) ||
             ((short)(&DAT_8008f856)[uVar2 * 0x1a] != in_t4)) ||
            ((&DAT_8008f85a)[uVar2 * 0x1a] != unaff_s6));
    iVar1 = (uint)*(byte *)(unaff_s1 + 0x12) * 2 + unaff_s1;
    uVar2 = (uint)*(short *)(iVar1 + 0x4e);
    if ((uVar2 != (unaff_s4 & 0xffff)) && (uVar2 == 0)) {
      *(undefined2 *)(iVar1 + 0x4e) = 1;
    }
    uVar3 = (((int)(((int)((int)(short)(&DAT_8008f84c)[(uint)in_t0 * 0x1a] * (unaff_s4 & 0xffff)) /
                    0x7f) * (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) *
             (uint)*(byte *)(in_t3 + DAT_800b2b28 + 1) *
            (uint)*(byte *)((in_t3 + (short)(&DAT_8008f858)[(uint)in_t0 * 0x1a]) * 0x20 +
                            DAT_800b2b3c + 2)) / 0x3f01;
    uVar2 = uVar3 * *(ushort *)(unaff_s1 + 0x74);
    iVar1 = (int)((ulonglong)uVar2 * (ulonglong)in_t2 >> 0x20);
    uVar3 = uVar3 * *(ushort *)(unaff_s1 + 0x76);
    iVar4 = (int)((ulonglong)uVar3 * (ulonglong)in_t2 >> 0x20);
    param_2 = (uint)*(byte *)((short)(&DAT_8008f858)[(uint)in_t0 * 0x1a] * 0x20 + DAT_800b2b3c + 3);
    uVar2 = iVar1 + (uVar2 - iVar1 >> 1) >> 6;
    param_1 = iVar4 + (uVar3 - iVar4 >> 1) >> 6;
    if (param_2 < 0x40) break;
    uVar2 = uVar2 * (in_t5 - param_2);
    iVar1 = (int)((ulonglong)uVar2 * (ulonglong)in_t1 >> 0x20);
    uVar3 = (uint)*(byte *)((short)(&DAT_8008f854)[(uint)in_t0 * 0x1a] * 0x10 + DAT_800b2b28 + 4);
    if (uVar3 < 0x40) {
      iVar1 = (int)((ulonglong)(param_1 * uVar3) * (ulonglong)in_t1 >> 0x20);
      iVar1 = VMANAGER_OBJ_3B90(iVar1 + (param_1 * uVar3 - iVar1 >> 1) >> 5);
      return iVar1;
    }
    uVar3 = (iVar1 + (uVar2 - iVar1 >> 1) >> 5) * (in_t5 - uVar3);
    iVar1 = (int)((ulonglong)uVar3 * (ulonglong)in_t1 >> 0x20);
    uVar2 = unaff_s3 & 0xff;
    if (uVar2 < 0x40) {
      iVar1 = (int)((ulonglong)(param_1 * uVar2) * (ulonglong)in_t1 >> 0x20);
      iVar1 = VMANAGER_OBJ_3BF4(iVar1 + (param_1 * uVar2 - iVar1 >> 1) >> 5);
      return iVar1;
    }
    uVar2 = (iVar1 + (uVar3 - iVar1 >> 1) >> 5) * (in_t5 - uVar2);
    iVar1 = (int)((ulonglong)uVar2 * (ulonglong)in_t1 >> 0x20);
    param_3 = iVar1 + (uVar2 - iVar1 >> 1) >> 5;
  }
  iVar1 = (int)((ulonglong)(param_1 * param_2) * (ulonglong)in_t1 >> 0x20);
  iVar1 = VMANAGER_OBJ_3AF4(iVar1 + (param_1 * param_2 - iVar1 >> 1) >> 5,param_2,uVar2);
  return iVar1;
}
