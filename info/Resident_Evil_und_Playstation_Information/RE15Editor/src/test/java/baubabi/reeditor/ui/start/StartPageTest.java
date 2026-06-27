package baubabi.reeditor.ui.start;

import baubabi.reeditor.ui.main.start.StartPage;
import org.junit.Assert;
import org.junit.Test;

import javax.swing.*;
import java.awt.*;

public class StartPageTest {

    @Test
    public void testStartPageComponents() {
        StartPage startPage = new StartPage();
        Component[] mainComponents = startPage.getPanel().getComponents();
        JPanel jPanel = (JPanel) mainComponents[0];
        Component[] components = jPanel.getComponents();
        JLabel labelComponent = (JLabel) components[0];
        Assert.assertEquals(labelComponent.getFont().getFamily(),"Creepster Caps");
    }
}