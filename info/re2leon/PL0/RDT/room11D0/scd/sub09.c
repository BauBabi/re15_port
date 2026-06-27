int sub09(void) {

	Aot_reset(5, 0, 0, 0, 0, 0x00);
	Aot_reset(0, 1, 49, 49316, 0, 0x61A2);
	Set(4, 27, 1);
	Set(1, 27, 1);
	Set(2, 7, 1);
	Sleep(7680);
	Se_on(2, 33, 1, 0, 0, -22110, 6396, -19888);
	Evt_exec(255, 0x180A);
	Sleep(2560);
	if(Ck(1, 0, 0)) {
		Xa_vol(70);
		Xa_on(0, 2816);
	} else {
		Xa_vol(50);
		Xa_on(0, 2304);
		nop();
		nop();
		Sleep(33280);
		Set(2, 7, 0);
		Set(1, 27, 0);
		return 0;
}