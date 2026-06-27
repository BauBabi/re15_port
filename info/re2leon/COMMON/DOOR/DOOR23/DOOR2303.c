int DOOR2303(void) {

	Work_set(5, 1);
	nop();
	Speed_set(3, -3585);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}