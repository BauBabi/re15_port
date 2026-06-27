package baubabi.reeditor.ui.main;

import baubabi.reeditor.core.general.utils.UserConfigurationUtility;
import baubabi.reeditor.ui.Utilities.InteractionCommunicationHelperUtility;
import baubabi.reeditor.ui.main.panels._00_firstlayer.RdtTabs;
import baubabi.reeditor.ui.main.start.StartPage;
import baubabi.reeditor.ui.menu.MenuBar;
import lombok.Getter;
import org.apache.commons.lang3.time.StopWatch;

import javax.swing.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

@Getter
public class StartAndRdtUI extends JFrame implements PropertyChangeListener {
    private StartPage startPage;

    public StartAndRdtUI() {
        initUI();
    }

    private void initUI() {
        //Add Menu Bar
        MenuBar menuBar = new MenuBar();
        setJMenuBar(menuBar.getMenuBar());
        menuBar.addPropertyChangeListener(this);

        startPage = new StartPage();
        setTitle("RE 1.5 Editor");
        setSize(1600, 600);
        add(startPage.getPanel());

        setVisible(true);
        setResizable(true);
        setExtendedState(MAXIMIZED_BOTH);
        setVisible(true);

        //Start maximized
        setExtendedState(JFrame.MAXIMIZED_BOTH);

        //Quit on close
        setDefaultCloseOperation(EXIT_ON_CLOSE);
    }

    public void propertyChange(PropertyChangeEvent evt) {

        if (evt.getPropertyName().equals("dataLoaded")) {
            StopWatch watch = new StopWatch();
            watch.start();

            //Data loaded via menu, start page can be removed
            if (getStartPage() != null) {
                this.remove(startPage.getPanel());
            }

            //Now we start with the Rdt tab panel
            RdtTabs rdtTabs = new RdtTabs();
            this.add(rdtTabs.getRdtPane());

            repaint();
            setVisible(true);
            setResizable(true);
            setExtendedState(MAXIMIZED_BOTH);
            setVisible(true);

            watch.stop();
            if(UserConfigurationUtility.getInstance().isDebug()) {
                System.out.println("Time Elapsed prepare UI: " + watch.getTime() + "ms");
            }
        } else if (evt.getPropertyName().equals("savePressed")) {
            InteractionCommunicationHelperUtility interactionCommunicationHelperUtility = InteractionCommunicationHelperUtility.getInstance();
            if (interactionCommunicationHelperUtility.getInfoDetailTabPropertyChangeListener() != null) {
                interactionCommunicationHelperUtility.getInfoDetailTabPropertyChangeListener().propertyChange(evt);
            }
        }
    }
}
