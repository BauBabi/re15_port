int sub16(void) {

	Flr_set(1, 1);
	Set(14, 4, 0);
	Set(15, 5, 0);
	Set(14, 3, 1);
	Set(15, 4, 1);
	nop();
	goto Sub06();
	Set(5, 2, 1);
	return 0;
}