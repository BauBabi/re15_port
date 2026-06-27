int DOOR0E01(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32896, 4096, -30470, -15856, 12288, 0, 0, 10753);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Work_set(5, 0);
	nop();
	Speed_set(4, 0);
	Speed_set(2, 0);
	Speed_set(8, 0);
	Speed_set(0, 7680);
	Speed_set(5, -257);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(5, 0);
	Speed_set(0, -9985);
	Speed_set(6, 2560);
	Speed_set(1, -1536);
	Speed_set(7, -4865);
	for(n = 0; n < 768; n++) {
		for(n = 0; n < 2560; n++) {
			Add_speed();
			Evt_next();
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(0, 25600);
		Speed_set(6, -2305);
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
		Speed_set(1, -1536);
		Speed_set(7, -4865);
		Speed_set(0, -9985);
		Speed_set(6, 2560);
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