int DOOR0E02(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32896, 4096, 181, -2311, 0, 0, 8, 10766);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Work_set(5, 0);
	nop();
	Speed_set(4, 0);
	Speed_set(2, 0);
	Speed_set(8, 0);
	Speed_set(0, 2560);
	Speed_set(1, -2817);
	Speed_set(5, -257);
	for(n = 0; n < 4096; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(5, 0);
	Sleep(1280);
	Speed_set(0, -25345);
	Speed_set(6, 10240);
	Speed_set(1, 25600);
	Speed_set(7, -2305);
	for(n = 0; n < 768; n++) {
		for(n = 0; n < 2560; n++) {
			Add_speed();
			Evt_next();
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(0, 30720);
		Speed_set(6, -7425);
		Speed_set(1, 0);
		Speed_set(7, 0);
		for(n = 0; n < 1280; n++) {
			Add_speed();
			Evt_next();
			Add_speed();
			Evt_next();
			Add_speed();
			Evt_next();
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(0, -25345);
		Speed_set(6, 10240);
		Speed_set(1, 25600);
		Speed_set(7, -2305);
		goto Sub03();
		Se_on(0, 0, 0, 0, 0, 0, 0, 0);
		Sleep(768);
	}
	Sce_fade_set(0, 2, 7, 0, 8);
	for(n = 0; n < 1792; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}