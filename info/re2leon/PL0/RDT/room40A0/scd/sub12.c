int sub12(void) {

	Work_set(4, 2);
	nop();
	while(Ck(5, 17, 0)) {
		Evt_next();
		nop();
	}
	Pos_set(0, 19466, -7445, -1364);
	Speed_set(1, 25600);
	Speed_set(7, 2560);
	Se_on(2, 18, 1, 4, 2, 0, 0, 0);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	goto Sub23();
	Aot_reset(0, 5, 49, 65280, 6173, 0x00);
	Aot_reset(1, 5, 49, 65280, 6174, 0x00);
	while(Cmp(26 != 512)) {
		Evt_next();
		nop();
	}
	Member_set(11, 11271);
	return 0;
}