package baubabi.reeditor.ui.menu.thanks;

import baubabi.reeditor.ui.abstracts.AbstractWindow;

public class ThanksWindow extends AbstractWindow {
    public ThanksWindow() {
        super(1800, 1000, "Help", ThanksWindowText.class.getCanonicalName());
    }
}
