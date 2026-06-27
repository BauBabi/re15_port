int sub04(void) {

	Work_set(4, 0);
	nop();
	Speed_set(1, -2305);
	Speed_set(7, -257);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 768; n++) {
		Speed_set(7, -257);
		for(n = 0; n < 1280; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(7, 512);
		for(n = 0; n < 1280; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
	}
	Set(4, 96, 1);
	Set(32, 6, 1);
	for(n = 0; n < 1792; n++) {
		Speed_set(7, -257);
		for(n = 0; n < 1280; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(7, 512);
		for(n = 0; n < 1280; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
	}
	Work_set(4, 0);
	nop();
	Sca_id_set(0, 0x0000);
	Sca_id_set(1, 0x0000);
	Speed_set(1, -7425);
	for(n = 0; n < 1280; n++) {
		Speed_set(7, -257);
		for(n = 0; n < 1280; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(7, 512);
		for(n = 0; n < 1280; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
	}
	Set(5, 0, 1);
	Pos_set(0, -12281, -12281, -12281);
	return 0;
}