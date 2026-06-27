int sub03(void) {

	Set(2, 7, 1);
	Evt_next();
	Cut_auto(0);
	nop();
	goto Sub14();
	goto Sub16();
	goto Sub17();
	goto Sub18();
	goto Sub19();
	goto Sub21();
	goto Sub22();
	goto Sub24();
	goto Sub25();
	Work_set(1, 0);
	Plc_ret();
	Cut_chg(0x01);
	Cut_auto(1);
	Sce_bgmtbl_set(0, 0x1301, 0x8FF, 0x00);
	Set(1, 27, 0);
	Set(2, 7, 0);
	return 0;
}