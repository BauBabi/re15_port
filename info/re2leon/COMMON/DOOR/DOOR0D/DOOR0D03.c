int DOOR0D03(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -24562, -4083, 0, 0, 0);
	Obj_model_move(1, 0, 229, 1, 1, 53252, 4096, -32256, 30963, -32526, 0, 0, 0);
	Obj_model_move(2, 0, 229, 1, 1, 20484, 4096, 32511, 30963, -32526, 8, 8, 0);
	Obj_model_move(3, 0, 0, 1, 0, 40962, 4096, -12281, -24562, -24335, 0, 8, 0);
	Obj_model_move(4, 0, 229, 1, 1, 54020, 4096, 32511, 30963, -26382, 0, 0, 8);
	Obj_model_move(5, 0, 229, 1, 1, 21252, 4096, -32256, 30963, -26382, 8, 8, 8);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	goto Sub13();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(12, 0x1806);
	Sleep(12800);
	Evt_exec(11, 0x1808);
	Work_set(5, 0);
	nop();
	Speed_set(4, -1281);
	Speed_set(10, 256);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_aspeed();
		nop();
	}
	Sleep(4608);
	Speed_set(0, 0);
	Speed_set(10, -1);
	Speed_set(4, 0);
	for(n = 0; n < 1024; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Evt_next();
	}
	for(n = 0; n < 3584; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	Speed_set(10, 512);
	for(n = 0; n < 1280; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 1024; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Evt_next();
	}
	Evt_exec(11, 0x1805);
	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(6, 512);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	Sce_fade_set(0, 2, 7, 0, 4);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}