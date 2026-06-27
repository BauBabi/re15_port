package baubabi.reeditor.ui.menu.thanks;

import baubabi.reeditor.ui.abstracts.AbstractWindowText;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.util.ArrayList;

@Log4j2
public class ThanksWindowText extends AbstractWindowText {
    public ThanksWindowText() {
        super(40);

        ArrayList<String> textForTextPane = new ArrayList<>();
        textForTextPane.add("This RE Editor wouldn't be possible without the work of many people, many thanks to:");
        textForTextPane.add("");
        textForTextPane.add("Cebix (Christian Bauer) for the PSXImager - http://www.cebix.net/");
        textForTextPane.add("MeganGrass especially for biofat - https://github.com/MeganGrass");
        textForTextPane.add("DarkBiohazard for dozens of informations -");
        textForTextPane.add("https://www.tapatalk.com/groups/residentevil123/memberlist.php?mode=viewprofile&u=2");
        textForTextPane.add("");
        textForTextPane.add("Leo2236 for dozens of informations -");
        textForTextPane.add("https://www.tapatalk.com/groups/residentevil123/memberlist.php?mode=viewprofile&u=872");
        textForTextPane.add("");
        textForTextPane.add("mortician for dozens of informations -");
        textForTextPane.add("https://www.tapatalk.com/groups/residentevil123/memberlist.php?mode=viewprofile&u=6");
        textForTextPane.add("");
        textForTextPane.add("And many many more who helped the Resident Evil modification scene");
        textForTextPane.add("");
        textForTextPane.add("");
        textForTextPane.add("Apache License for Creepster Caps: https://www.fontsc.com/font/creepster-caps");
        addTextToTextPanel(textForTextPane);
    }

    @Override
    public JPanel getPanel() {
        return panel;
    }
}
