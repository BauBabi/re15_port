package baubabi.reeditor.ui.main.panels._02_thirdlayer._04_log_panel;

import lombok.Getter;

import javax.swing.*;
import java.awt.*;

import static javax.swing.BorderFactory.createEmptyBorder;

@Getter
public class LogPanel {
    private final JPanel logPanel = new JPanel(false);

    private final JTextArea logArea;

    public LogPanel() {
        logPanel.setBorder(BorderFactory.createMatteBorder(1, 0, 1, 1, Color.gray));
        logPanel.setBackground(Color.LIGHT_GRAY);
        logPanel.setOpaque(true);
        logPanel.setName("LogPanel");

        logArea = new JTextArea("Logs", 55, 41);
        logArea.setAlignmentX(JTextField.CENTER);
        logArea.setBackground(Color.LIGHT_GRAY);

        JScrollPane scrollPane = new JScrollPane(logArea);
        scrollPane.setBorder(createEmptyBorder());

        logPanel.add(scrollPane);
    }

    public void writeLog(String log) {
        if(logArea.getText().equals("Logs")) {
            logPanel.setLayout(new FlowLayout(FlowLayout.LEADING));
            logArea.setAlignmentX(JTextField.LEFT);
            logArea.setText(log);
        }
        else {
            logArea.setText(logArea.getText() + "\n" + log);
        }
    }
}
