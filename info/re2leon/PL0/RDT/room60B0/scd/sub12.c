int sub12(void) {

	Evt_next();
	nop();
	if(Ck(5, 3, 1)) {
		Work_set(3, 1);
		nop();
		Member_set(23, 256);
	}
	if(Ck(5, 4, 1)) {
		Work_set(3, 2);
		nop();
		Member_set(23, 256);
	}
	return 0;
}