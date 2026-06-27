int sub04(void) {

	if(Ck(5, 10, 0)) {
		Message_on(0, 512, 0xFFFF);
	} else {
		Message_on(0, 768, 0xFFFF);
		nop();
		nop();
		return 0;
}