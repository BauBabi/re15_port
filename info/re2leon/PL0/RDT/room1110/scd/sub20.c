int sub20(void) {

	Work_set(4, 0);
	nop();
	Member_set(14, 0);
	Sca_id_set(3, 0x10FC);
	Sca_id_set(18, 0x10FC);
	Sca_id_set(10, 0x0000);
	Cut_be_set(2, 2, 1);
	return 0;
}