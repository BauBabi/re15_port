int sub27(void) {

	Work_set(4, 2);
	nop();
	Speed_set(1, 25600);
	Speed_set(7, 5120);
	Speed_set(5, 16384);
	Speed_set(4, 16384);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	return 0;
}