int sub06(void) {

	Work_set(4, 0);
	nop();
	Member_set(12, -14130);
	Speed_set(1, 10240);
	for(n = 0; n < 46080; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}