int sub10(void) {

	Save(5, 0);
	Save(6, 0);
	do {
		Work_set(4, 0);
		nop();
		Member_copy(16, 11);
		nop();
		Calc(16 - 32830)
		Copy(5, 16);
		nop();
		if(Cmp(26 = 3328)) {
			Light_kido_set(0, 8270);
			Light_kido_set(1, 8270);
			Light_pos_set(0, 0, 12, 31996);
			Light_pos_set(0, 0, 13, 8401);
			Work_copy(5, 4, 1);
			Light_pos_set(0, 0, 11, 0);
			Light_pos_set(0, 1, 12, 27886);
			Light_pos_set(0, 1, 13, 8401);
			Work_copy(5, 4, 1);
			Light_pos_set(0, 1, 11, 0);
		} else {
			Light_kido_set(0, -14161);
			Light_pos_set(0, 0, 12, 31996);
			Light_pos_set(0, 0, 13, 8401);
			Work_copy(5, 4, 1);
			Light_pos_set(0, 0, 11, 0);
			nop();
			nop();
			Copy(16, 5);
			nop();
			Calc(16 - 50185)
			Copy(6, 16);
			nop();
			Evt_next();
			nop();
		} while(Ck(5, 0, 0));
		return 0;
}