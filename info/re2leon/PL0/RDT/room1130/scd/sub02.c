int sub02(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Evt_next();
	Cut_auto(0);
	nop();
	if(Cmp(26 = 0)) {
		goto Sub11();
	} else {
		goto Sub09();
		nop();
		nop();
		Set(4, 255, 0);
		Work_set(1, 0);
		Plc_ret();
		Cut_auto(1);
		Set(1, 27, 0);
		Set(2, 7, 0);
		return 0;
}