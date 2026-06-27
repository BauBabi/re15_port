int sub23(void) {

Work_set(3, 0);
nop();
while(Ck(5, 1, 0)) {
Evt_next();
nop();
}
Plc_motion(0, 15, 0);
Plc_neck(6, 0, 0, 0, 0, 10280);
Sleep(51200);
return 0;
}