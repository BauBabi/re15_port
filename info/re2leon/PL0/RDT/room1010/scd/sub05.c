int sub05(void) {

Evt_next();
Work_set(3, 1);
goto Sub06();
Work_set(3, 2);
nop();
goto Sub06();
Work_set(3, 3);
nop();
goto Sub06();
Work_set(3, 4);
nop();
goto Sub06();
return 0;
}