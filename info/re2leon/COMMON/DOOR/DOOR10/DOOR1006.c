int DOOR1006(void) {

	Save(6, 35330);
	Save(7, 30461);
	for(n = 0; n < 25600; n++) {
		Save(5, 256);
		Work_copy(5, 2, 0);
		Work_set(5, 0);
		nop();
		Work_copy(6, 2, 1);
		Member_set(13, 0);
		Copy(16, 5);
		nop();
		Calc(16 + 256)
		Copy(5, 16);
		nop();
		for(n = 0; n < 2048; n++) {
			Work_copy(5, 2, 0);
			Work_set(5, 0);
			nop();
			Work_copy(7, 2, 1);
			Member_set(13, 0);
			Copy(16, 5);
			nop();
			Calc(16 + 256)
			Copy(5, 16);
			nop();
		}
		Copy(16, 6);
		nop();
		Calc(16 - 1280)
		Copy(6, 16);
		nop();
		Copy(16, 7);
		nop();
		Calc(16 + 1280)
		Copy(7, 16);
		nop();
		Evt_next();
		nop();
	}
	return 0;
}