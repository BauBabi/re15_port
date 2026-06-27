void FUN_8004c058(void)

{
  RECT local_20;
  
  if (DAT_800b25c0._2_1_ == 1) {
    if (((DAT_800ac76c & 0x8000) == 0) && ((DAT_800ac762 & 4) == 0)) {
      return;
    }
code_r0x8004c208:
    DAT_800b25c0._2_1_ = DAT_800b25c0._2_1_ + 1;
  }
  else {
    if (DAT_800b25c0._2_1_ < 2) {
      if (DAT_800b25c0._2_1_ != 0) {
        return;
      }
      if (0x18 < DAT_800b25c0._3_1_) {
        local_20.x = 0x1c0;
        local_20.y = 0x100;
        local_20.w = 0x40;
        local_20.h = 0x100;
        StoreImage(&local_20,(u_long *)&DAT_801a0000);
        DrawSync(0);
        local_20.x = 0x1c0;
        local_20.y = 0x100;
        local_20.w = 0x40;
        local_20.h = 0x100;
        LoadImage(&local_20,(u_long *)&DAT_801a8000);
        DrawSync(0);
        FUN_80046fd8();
        DAT_800b25c0._3_1_ = 0;
        goto code_r0x8004c208;
      }
      DAT_800b25e0 = DAT_800b25e0 + 0xf;
      DAT_800b25e6 = DAT_800b25e6 + 9;
      DAT_800b25e4 = DAT_800b25e4 + -9;
      DAT_800b25de = DAT_800b25de + -7;
      DAT_800b25da = DAT_800b25da + -7;
      DAT_800b25f0 = DAT_800b25f0 + -8;
      DAT_800b25ea = DAT_800b25ea + 7;
    }
    else {
      if (DAT_800b25c0._2_1_ != 2) {
        return;
      }
      if (0x18 < DAT_800b25c0._3_1_) {
        local_20.x = 0x1c0;
        local_20.y = 0x100;
        local_20.w = 0x40;
        local_20.h = 0x100;
        LoadImage(&local_20,(u_long *)&DAT_801a0000);
        DrawSync(0);
        DAT_800b25c0._1_1_ = 0;
        DAT_800b25c0._2_1_ = 0;
        DAT_800b25c0._3_1_ = 0;
        DAT_800b25ca = 0;
        return;
      }
      DAT_800b25e0 = DAT_800b25e0 + -0xf;
      DAT_800b25e6 = DAT_800b25e6 + -9;
      DAT_800b25e4 = DAT_800b25e4 + 9;
      DAT_800b25de = DAT_800b25de + 7;
      DAT_800b25da = DAT_800b25da + 7;
      DAT_800b25f0 = DAT_800b25f0 + 8;
      DAT_800b25ea = DAT_800b25ea + -7;
    }
    DAT_800b25c0._3_1_ = DAT_800b25c0._3_1_ + 1;
  }
  return;
}
