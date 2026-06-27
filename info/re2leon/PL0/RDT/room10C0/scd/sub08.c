int sub08(void) {

	Work_set(1, 0);
	Plc_ret();
	Set(2, 7, 0);
	Set(1, 27, 0);
	Cut_auto(1);
	return 0;
}