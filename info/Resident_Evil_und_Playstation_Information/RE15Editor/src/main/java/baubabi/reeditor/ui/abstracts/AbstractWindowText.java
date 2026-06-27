package baubabi.reeditor.ui.abstracts;

import baubabi.reeditor.core.exception.ExceptionHandler;
import baubabi.reeditor.core.general.utils.DirectoryUtility;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

@Log4j2
public abstract class AbstractWindowText {
    public JPanel panel = new JPanel(false);

    //Group for the text fragments
    public JPanel textPane = new JPanel();

    public int fontSize;

    public AbstractWindowText(int fontSize) {
        panel.setLayout(new GridBagLayout());
        this.fontSize = fontSize;
    }

    public void addTextToTextPanel(ArrayList<String> textForTextPanel) {
        for(int indexOfTextElement=0; indexOfTextElement < textForTextPanel.size(); indexOfTextElement++) {
            textPane.add(createText(textForTextPanel.get(indexOfTextElement)));
        }
        textPane.setLayout(new GridLayout(textPane.getComponentCount(), 0));
        panel.add(textPane, JLabel.CENTER);
    }

    private Font initializeFont() {
        InputStream reFont = DirectoryUtility.getInstance().getResourcesElement("Fonts/CreepsterCaps-Regular.ttf");
        Font font = null;
        try {
            font = Font.createFont(Font.TRUETYPE_FONT, reFont);
            font = font.deriveFont(Font.BOLD, fontSize);
            reFont.close();
        } catch (FontFormatException e) {
            ExceptionHandler.getInstance().logException("FontFormatException at Loading Font!", e);
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at Loading Font!", e);
        }

        return font;
    }

    private JLabel createText(String text) {
        JLabel filler = new JLabel(text);
        filler.setHorizontalAlignment(JLabel.CENTER);
        filler.setFont(initializeFont());

        return filler;
    }

    public abstract JPanel getPanel();
}
