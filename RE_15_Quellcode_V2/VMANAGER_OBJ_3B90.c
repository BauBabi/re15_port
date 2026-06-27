int VMANAGER_OBJ_3B90(uint param_1,uint param_2,uint param_3)

{
  uint uVar1;
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
  int iVar5;
  
  while( true ) {
    uVar1 = unaff_s3 & 0xff;
    if (uVar1 < 0x40) {
      iVar5 = (int)((ulonglong)(param_1 * uVar1) * (ulonglong)in_t1 >> 0x20);
      iVar5 = VMANAGER_OBJ_3BF4(iVar5 + (param_1 * uVar1 - iVar5 >> 1) >> 5);
      return iVar5;
    }
    param_3 = param_3 * (in_t5 - uVar1);
    iVar5 = (int)((ulonglong)param_3 * (ulonglong)in_t1 >> 0x20);
    uVar3 = iVar5 + (param_3 - iVar5 >> 1) >> 5;
    uVar1 = param_1;
    if ((DAT_800b2678 == 1) && (uVar1 = uVar3, uVar3 < param_1)) {
      iVar5 = VMANAGER_OBJ_3C20(param_1,param_2,param_1);
      return iVar5;
    }
    uVar2 = (uint)in_t0;
    *(short *)(uVar2 * 0x10 + in_t6) = (short)((uVar3 * uVar3) / 0x3fff);
    *(short *)(uVar2 * 0x10 + in_t7) = (short)((uVar1 * uVar1) / 0x3fff);
    unaff_s2 = unaff_s2 + 1;
    (&DAT_8008f82c)[uVar2] = (&DAT_8008f82c)[uVar2] | 3;
    do {
      in_t0 = in_t0 + 1;
      uVar1 = (uint)in_t0;
      if (DAT_800b52a8 <= in_t0) {
        return unaff_s2;
      }
    } while ((((&DAT_8008f852)[uVar1 * 0x1a] != unaff_s5) ||
             ((short)(&DAT_8008f856)[uVar1 * 0x1a] != in_t4)) ||
            ((&DAT_8008f85a)[uVar1 * 0x1a] != unaff_s6));
    iVar5 = (uint)*(byte *)(unaff_s1 + 0x12) * 2 + unaff_s1;
    uVar1 = (uint)*(short *)(iVar5 + 0x4e);
    if ((uVar1 != (unaff_s4 & 0xffff)) && (uVar1 == 0)) {
      *(undefined2 *)(iVar5 + 0x4e) = 1;
    }
    uVar3 = (((int)(((int)((int)(short)(&DAT_8008f84c)[(uint)in_t0 * 0x1a] * (unaff_s4 & 0xffff)) /
                    0x7f) * (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) *
             (uint)*(byte *)(in_t3 + DAT_800b2b28 + 1) *
            (uint)*(byte *)((in_t3 + (short)(&DAT_8008f858)[(uint)in_t0 * 0x1a]) * 0x20 +
                            DAT_800b2b3c + 2)) / 0x3f01;
    uVar1 = uVar3 * *(ushort *)(unaff_s1 + 0x74);
    iVar5 = (int)((ulonglong)uVar1 * (ulonglong)in_t2 >> 0x20);
    uVar3 = uVar3 * *(ushort *)(unaff_s1 + 0x76);
    iVar4 = (int)((ulonglong)uVar3 * (ulonglong)in_t2 >> 0x20);
    param_2 = (uint)*(byte *)((short)(&DAT_8008f858)[(uint)in_t0 * 0x1a] * 0x20 + DAT_800b2b3c + 3);
    uVar1 = iVar5 + (uVar1 - iVar5 >> 1) >> 6;
    param_1 = iVar4 + (uVar3 - iVar4 >> 1) >> 6;
    if (param_2 < 0x40) break;
    uVar1 = uVar1 * (in_t5 - param_2);
    iVar5 = (int)((ulonglong)uVar1 * (ulonglong)in_t1 >> 0x20);
    uVar3 = (uint)*(byte *)((short)(&DAT_8008f854)[(uint)in_t0 * 0x1a] * 0x10 + DAT_800b2b28 + 4);
    if (uVar3 < 0x40) {
      iVar5 = (int)((ulonglong)(param_1 * uVar3) * (ulonglong)in_t1 >> 0x20);
      iVar5 = VMANAGER_OBJ_3B90(iVar5 + (param_1 * uVar3 - iVar5 >> 1) >> 5);
      return iVar5;
    }
    uVar1 = (iVar5 + (uVar1 - iVar5 >> 1) >> 5) * (in_t5 - uVar3);
    iVar5 = (int)((ulonglong)uVar1 * (ulonglong)in_t1 >> 0x20);
    param_3 = iVar5 + (uVar1 - iVar5 >> 1) >> 5;
  }
  iVar5 = (int)((ulonglong)(param_1 * param_2) * (ulonglong)in_t1 >> 0x20);
  iVar5 = VMANAGER_OBJ_3AF4(iVar5 + (param_1 * param_2 - iVar5 >> 1) >> 5,param_2,uVar1);
  return iVar5;
}
