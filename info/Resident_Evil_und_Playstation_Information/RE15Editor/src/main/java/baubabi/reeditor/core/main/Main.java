package baubabi.reeditor.core.main;

import baubabi.reeditor.core.general.utils.UserConfigurationUtility;
import baubabi.reeditor.ui.main.StartAndRdtUI;

import java.awt.*;

public class Main {

    public static void main(String[] args) {
        //Start loading user Configurations
        UserConfigurationUtility userConfigurationUtility = UserConfigurationUtility.getInstance();
        userConfigurationUtility.loadEPSXeConfiguration();
        userConfigurationUtility.loadLoggerConfig();

        EventQueue.invokeLater(() -> {

            var mainWindow = new StartAndRdtUI();
            mainWindow.setVisible(true);
        });
    }
}
