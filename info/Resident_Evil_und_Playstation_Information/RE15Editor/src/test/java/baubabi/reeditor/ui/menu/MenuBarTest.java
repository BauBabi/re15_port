package baubabi.reeditor.ui.menu;

import org.junit.Test;

import javax.swing.*;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class MenuBarTest {

    @Test
    public void testMenuBarElements() {
        MenuBar menuBarElement = new MenuBar();
        JMenuBar menuBar = menuBarElement.getMenuBar();

        //Check Main Menu Count
        assertEquals(3, menuBar.getMenuCount());
        JMenu fileMenu = menuBar.getMenu(0);
        JMenu helpMenu = menuBar.getMenu(1);
        JMenu emulatorMenu = menuBar.getMenu(2);

        //Check Image Count
        JMenu imageMenu = (JMenu) fileMenu.getItem(0);
        assertEquals(2, imageMenu.getItemCount());

        JMenuItem fileImageSaveItem = imageMenu.getItem(1);
        fileImageSaveItem.doClick();

        //TODO SECOND MENU
    }

}