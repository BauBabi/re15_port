package baubabi.reeditor.ui.menu.about;

import baubabi.reeditor.ui.abstracts.AbstractWindow;

public class AboutWindow extends AbstractWindow {

    public AboutWindow() {
        super(1200, 600, "About", AboutWindowText.class.getCanonicalName());
    }
}
