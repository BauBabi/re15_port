int sub05(void) {

	Work_set(4, 0);
	nop();
	Pos_set(0, -24375, 0, -6998);
	Speed_set(1, -2305);
	Speed_set(7, -257);
	Se_on(2, 11, 1, 4, 0, 0, 0, 0);
	// unknown opcode (8C) found at 0x00000028
	// unknown opcode (8B) found at 0x0000002E
	// unknown opcode (8A) found at 0x00000034
	// unknown opcode (8C) found at 0x0000003C
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
	Sce_espr_kill(22, 0, 0, 0);
	Set(4, 95, 1);
	Set(32, 5, 1);
	nop();
	goto Sub04();
	for(n = 0; n < 2560; n++) {
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
	Sca_id_set(0, 0x0000);
	Sca_id_set(1, 0x0000);
	Sca_id_set(2, 0x0000);
	if(Ck(7, 21, 0)) {
		Work_set(3, 1);
		nop();
		Member_set(23, 256);
	}
	if(Ck(7, 22, 0)) {
		Work_set(3, 2);
		nop();
		Member_set(23, 256);
	}
	Set(5, 0, 1);
	Work_set(4, 0);
	nop();
	Speed_set(1, -7425);
	for(n = 0; n < 512; n++) {
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
	Pos_set(0, -12281, -12281, -12281);
	Aot_reset(5, 4, 49, 512, 0, 0xFF);
	return 0;
}