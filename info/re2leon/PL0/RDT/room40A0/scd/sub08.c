int sub08(void) {

	Work_set(4, 3);
	nop();
	Speed_set(0, -2305);
	Speed_set(6, -2049);
	Sca_id_set(12, 0x0080);
	Se_on(2, 12, 1, 4, 3, 0, 0, 0);
	for(n = 0; n < 3072; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(6, 2048);
	for(n = 0; n < 768; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Sca_id_set(12, 0x0040);
	Sca_id_set(14, 0x0000);
	return 0;
}