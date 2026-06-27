int sub00(void) {

Evt_exec(255, 0x180A);
Evt_exec(255, 0x180B);
if(Ck(4, 27, 0)) {
Aot_reset(0, 5, 49, 65280, 6146, 0x00);
Aot_set(6, 5, 65, 0, 0, 15272, -7356, -32756, 28695, 255, 0, 24);
}
goto Sub04();
goto Sub19();
Evt_exec(255, 0x1811);
return 0;
}