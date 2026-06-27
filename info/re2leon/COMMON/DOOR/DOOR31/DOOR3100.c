int DOOR3100(void) {

	switch(12) {
	case 0:
		goto Sub03();
	break;
	case 256:
		goto Sub04();
	break;
	case 512:
		goto Sub02();
	break;
	case 768:
		goto Sub01();
	break;
	}
	return 0;
}