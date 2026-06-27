int sub17(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Work_set(2, 0);
	Splc_sce();
	Aot_reset(2, 0, 49, 0, 0, 0x00);
	Aot_reset(0, 1, 49, 57521, 0, 0xC6A8);
	goto Sub03();
	goto Sub07();
	Work_set(1, 0);
	Plc_ret();
	Work_set(2, 0);
	Splc_ret();
	Cut_auto(1);
	Set(2, 7, 0);
	Set(1, 27, 0);
	return 0;
}