int sub06(void) {

	Set(2, 7, 1);
	Aot_on(10);
	Set(4, 114, 1);
	Evt_next();
	nop();
	Aot_reset(3, 0, 65, 0, 0, 0x00);
	Aot_reset(7, 0, 65, 0, 0, 0x00);
	Aot_reset(8, 0, 65, 0, 0, 0x00);
	Sleep(6400);
	Work_set(3, 0);
	nop();
	if(Ck(6, 79, 0)) {
		Set(4, 35, 1);
		Evt_exec(255, 0x1807);
	}
	Work_set(3, 1);
	nop();
	if(Ck(6, 80, 0)) {
		Set(4, 112, 1);
		Evt_exec(255, 0x1808);
	}
	Set(2, 7, 0);
	Sleep(2304);
	if(Ck(6, 79, 0)) {
		Sce_bgm_control(1, 1, 0, 0, 0);
	} else {
		if(Ck(6, 80, 0)) {
			Sce_bgm_control(1, 1, 0, 0, 0);
		} else {
			Set(5, 0, 1);
			nop();
			nop();
			nop();
			nop();
			goto Sub02();
			return 0;
}