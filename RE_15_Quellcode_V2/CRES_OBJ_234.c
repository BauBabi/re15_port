void CRES_OBJ_234(void)

{
  int iVar1;
  int unaff_s0;
  short unaff_s3;
  ushort unaff_s4;
  
  if ((*(int *)(unaff_s0 + 0x98) == 0) || (*(short *)(unaff_s0 + 0x40) == 0)) {
    iVar1 = unaff_s3 * 0xac + *(int *)((int)&DAT_800bb500 + ((int)((uint)unaff_s4 << 0x10) >> 0xe));
    *(uint *)(iVar1 + 0x90) = *(uint *)(iVar1 + 0x90) & 0xffffffef;
  }
  SpuVmGetSeqVol((int)(short)(unaff_s4 | unaff_s3 << 8),unaff_s0 + 0x78,unaff_s0 + 0x7a);
  return;
}
