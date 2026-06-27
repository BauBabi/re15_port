long SpuSetReverbDepth(SpuReverbAttr *attr)

{
  uint uVar1;
  
  uVar1 = attr->mask;
  if ((uVar1 == 0) || ((uVar1 & 2) != 0)) {
    SPU_REVERB_OUT_L = (attr->depth).left;
    DAT_80076ddc = (attr->depth).left;
  }
  if ((uVar1 == 0) || ((uVar1 & 4) != 0)) {
    SPU_REVERB_OUT_R = (attr->depth).right;
    DAT_80076dde = (attr->depth).right;
  }
  return 0;
}
