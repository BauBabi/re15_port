uint GetODE(void)

{
  uint uVar1;
  
  uVar1 = _status();
  return uVar1 >> 0x1f;
}
