package baubabi.reeditor.ui.menu.about;

import baubabi.reeditor.core.exception.ExceptionHandler;
import baubabi.reeditor.ui.abstracts.AbstractWindowText;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

@Log4j2
public class AboutWindowText extends AbstractWindowText {
    public AboutWindowText() {
        super(40);

        ArrayList<String> textForTextPane = new ArrayList<>();
        textForTextPane.add("Your Version of the RE 1.5 Editor is:");
        textForTextPane.add("");
        textForTextPane.add(getVersionNumber());
        addTextToTextPanel(textForTextPane);
    }

    private String getVersionNumber() {
        ClassLoader classLoader = getClass().getClassLoader();
        InputStream inputStream = classLoader.getResourceAsStream("version.txt");
        String version;
        version = readFromInputStream(inputStream);
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException e) {
                ExceptionHandler.getInstance().logException("IOException at Reading Version Data!", e);
            }
        }
        System.out.println(version);
        return version;
    }

    private String readFromInputStream(InputStream inputStream) {
        StringBuilder resultStringBuilder = new StringBuilder();
        try (BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream))) {
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                resultStringBuilder.append(line).append("\n");
            }
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at Reading Input Stream!", e);
        }
        return resultStringBuilder.toString();
    }

    @Override
    public JPanel getPanel() {
        return panel;
    }
}
