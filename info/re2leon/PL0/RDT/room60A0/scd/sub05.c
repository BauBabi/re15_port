int sub05(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Evt_exec(255, 0x1808);
	Cut_chg(0x02);
	Aot_reset(4, 0, 0, 0, 0, 0x00);
	Aot_reset(5, 0, 0, 0, 0, 0x00);
	Aot_reset(6, 0, 0, 0, 0, 0x00);
	Aot_reset(7, 0, 0, 0, 0, 0x00);
	Aot_reset(30, 0, 0, 0, 0, 0x00);
	while(Ck(5, 1, 0)) {
		Evt_next();
		nop();
	}
	Sleep(18944);
	Evt_exec(255, 0x180C);
	Sleep(5120);
	Work_set(3, 1);
	nop();
	Member_set(23, 256);
	Sleep(25600);
	Evt_exec(255, 0x180A);
	Se_on(2, 12, 1, 1, 0, 0, 0, 0);
	Sleep(4096);
	goto Sub11();
	Sleep(3072);
	Sce_fade_set(1, 2, 7, 255, 7);
	Set(5, 3, 1);
	Sleep(3840);
	for(n = 0; n < 7680; n++) {
		Sce_fade_adjust(1, 255, 127);
		Evt_next();
		nop();
	}
	Sce_fade_set(1, 2, 7, 255, 247);
	Cut_auto(1);
	Sleep(11776);
	Set(1, 27, 0);
	Sleep(2560);
	Set(2, 7, 0);
	return 0;
}