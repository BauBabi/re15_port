int sub09(void) {

Set(2, 7, 1);
Work_set(1, 0);
nop();
if(Ck(1, 0, 0)) {
Plc_motion(0, 26, 128);
Plc_cnt(70);
Sleep(5120);
} else {
Plc_motion(0, 15, 128);
Plc_cnt(70);
Sleep(5120);
nop();
nop();
Set(2, 7, 0);
return 0;
}