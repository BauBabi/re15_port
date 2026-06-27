void def_cbready(void)

{
  DeliverEvent(0xf0000003,0x40);
  return;
}
