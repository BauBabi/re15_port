int sub06(void) {

	if(Ck(4, 93, 0)) {
		if(Ck(1, 1, 1)) {
			Cut_be_set(2, 3, 0);
		} else {
			Cut_be_set(2, 2, 0);
			nop();
			nop();
		} else {
			goto Sub05();
			nop();
			nop();
			return 0;
}