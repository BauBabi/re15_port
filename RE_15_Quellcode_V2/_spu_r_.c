void _spu_r_(undefined4 param_1,word param_2)

{
  undefined4 local_8;
  
  local_8 = 0;
  do {
    local_8 = local_8 + 1;
  } while (local_8 < 0xf0);
  SOUND_RAM_DATA_TRANSFER_ADDR = param_2;
  SPU_OBJ_B24();
  return;
}
