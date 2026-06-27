int sub03(void) {

	Evt_next();
	Set(1, 27, 1);
	nop();
	goto Sub19();
	goto Sub21();
	goto Sub22();
	goto Sub23();
	goto Sub24();
	Cut_chg(0x06);
	Evt_next();
	nop();
	goto Sub25();
	goto Sub26();
	goto Sub27();
	goto Sub28();
	goto Sub04();
	goto Sub10();
	Work_set(1, 0);
	Plc_ret();
	Sce_bgmtbl_set(0, 0x1501, 0x31FF, 0x00);
	Set(1, 27, 0);
	Set(5, 0, 1);
	return 0;
}