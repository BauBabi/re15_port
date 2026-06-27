int DOOR2404(void) {

	Work_set(5, 2);
	nop();
	Speed_set(3, -2305);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}