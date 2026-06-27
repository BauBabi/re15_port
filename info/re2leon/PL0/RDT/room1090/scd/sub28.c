int sub28(void) {

	Sleep(1280);
	Work_set(4, 3);
	nop();
	Speed_set(1, 25600);
	Speed_set(7, 5120);
	Speed_set(5, -7937);
	Speed_set(4, 8192);
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