int sub02(void) {

Aot_reset(6, 0, 0, 0, 0, 0x00);
Aot_reset(0, 1, 49, 49316, 0, 0x61A2);
Set(4, 27, 1);
Set(1, 27, 1);
Set(2, 7, 1);
Sleep(7680);
Se_on(2, 33, 1, 0, 0, -22110, 6396, -19888);
Evt_exec(255, 0x1803);
Sleep(2560);
Xa_vol(100);
Xa_on(0, 2816);
Sleep(33280);
Set(2, 7, 0);
Set(1, 27, 0);
return 0;
}