int sub52(void) {

	Speed_set(1, 27391);
	for(n = 0; n < 5632; n++) {
		Add_speed();
		Evt_next();
	}
	Member_set(12, 131);
	return 0;
}