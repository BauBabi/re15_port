int DOOR1601(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32768, 4096, 28695, 6, 0, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	goto Sub03();
	Work_set(5, 0);
	nop();
	Speed_set(0, 12800);
	Speed_set(5, -3585);
	for(n = 0; n < 5120; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	Speed_set(0, -12545);
	Speed_set(6, 2560);
	Speed_set(1, 7680);
	Speed_set(5, 0);
	for(n = 0; n < 1024; n++) {
		for(n = 0; n < 2560; n++) {
			Add_speed();
			Evt_next();
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(0, 10240);
		Speed_set(1, 0);
		for(n = 0; n < 5120; n++) {
			Add_speed();
			Evt_next();
		}
		Speed_set(1, 7680);
		Speed_set(0, -12545);
		goto Sub04();
		Se_on(0, 0, 0, 0, 0, 0, 0, 0);
		Sleep(3840);
	}
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Sce_fade_set(0, 2, 7, 0, 4);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(0, 10240);
	Speed_set(1, 0);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}