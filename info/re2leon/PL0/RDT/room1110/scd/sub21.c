int sub21(void) {

	Work_set(4, 10);
	nop();
	Pos_set(0, 131, 131, 131);
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	Cut_be_set(5, 3, 1);
	return 0;
}