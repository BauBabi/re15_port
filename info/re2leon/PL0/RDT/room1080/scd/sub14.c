int sub14(void) {

	Set(1, 27, 1);
	Set(2, 7, 1);
	Set(4, 181, 1);
	Sleep(2048);
	Work_set(1, 0);
	nop();
	Sleep(2560);
	Plc_dest(0, 2308, -13610, -30025);
	while(Ck(5, 4, 0)) {
		Evt_next();
		nop();
	}
	Plc_motion(0, 18, 0);
	Sleep(2304);
	for(n = 0; n < 7680; n++) {
		Plc_cnt(10);
		Evt_next();
		nop();
	}
	Plc_neck(2, 0, 0, -7940, -7940, 2570);
	for(n = 0; n < 9216; n++) {
		Plc_cnt(10);
		Evt_next();
		nop();
	}
	Sce_bgm_control(0, 4, 0, 1, 65);
	Sce_fade_set(2, 2, 7, 0, 248);
	Movie_on(9);
	Sleep(256);
	Sce_bgm_control(0, 3, 0, 128, 65);
	Plc_motion(0, 18, 192);
	Plc_cnt(10);
	Sleep(2304);
	Plc_neck(0, 0, 0, 0, 0, 2570);
	Sleep(7680);
	Set(1, 27, 0);
	Set(2, 7, 0);
	Aot_reset(1, 1, 49, 43245, 0, 0x9CFA);
	Plc_ret();
	nop();
	return 0;
}