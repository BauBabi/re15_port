int sub13(void) {

	Set(4, 50, 1);
	Aot_reset(7, 0, 0, 0, 0, 0x00);
	Work_set(3, 1);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	Work_set(3, 2);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	Work_set(3, 3);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	Work_set(3, 4);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	Work_set(3, 5);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	Work_set(3, 6);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	Work_set(3, 7);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	Work_set(3, 8);
	nop();
	if(Member_cmp(0 ? 256)) {
		Member_set(23, 256);
	}
	return 0;
}