int sub06(void) {

Aot_reset(3, 0, 0, 0, 0, 0x00);
while(Ck(5, 10, 0)) {
Evt_next();
nop();
if(Cmp(26 = 1024)) {
if(Ck(5, 4, 0)) {
Set(5, 10, 1);
}
}
if(Cmp(26 = 1280)) {
if(Ck(5, 4, 0)) {
Set(5, 10, 1);
}
}
}
Aot_reset(4, 0, 0, 0, 0, 0x00);
Aot_reset(5, 0, 0, 0, 0, 0x00);
Set(4, 135, 1);
Set(1, 27, 1);
Set(2, 7, 1);
Cut_chg(0x02);
Evt_exec(255, 0x1807);
Evt_exec(255, 0x1808);
Sleep(7680);
Set(2, 7, 0);
Set(1, 27, 0);
Cut_chg(0x04);
Cut_auto(1);
return 0;
}