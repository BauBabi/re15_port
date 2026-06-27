void _ctl(uint param_1)

{
  GPU_REG1 = param_1;
  (&DAT_8008fe68)[param_1 >> 0x18] = (char)param_1;
  return;
}
