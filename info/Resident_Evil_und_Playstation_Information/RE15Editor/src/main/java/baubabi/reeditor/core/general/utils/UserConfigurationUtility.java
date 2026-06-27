package baubabi.reeditor.core.general.utils;

import baubabi.reeditor.core.exception.ExceptionHandler;
import lombok.Getter;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.core.LoggerContext;

import java.io.*;

@Getter
public class UserConfigurationUtility {

    private static UserConfigurationUtility INSTANCE;

    private File absoluteEpsxePathFile;

    private boolean isDebug = false;

    private boolean isStateAsTextField = false;

    private boolean isEpsxeNoSound = false;

    public static UserConfigurationUtility getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new UserConfigurationUtility();
        }

        return INSTANCE;
    }

    public void loadLoggerConfig() {
        String loggerConfigurationString = System.getenv("loggerConfigurationFile");
        File logConfigurationFile = null;
        if (loggerConfigurationString != null) {
            if (loggerConfigurationString.length() > 0) {
                logConfigurationFile = new File(loggerConfigurationString);
            }
        } else {
            logConfigurationFile = new File("log4j2.xml");
        }

        if (logConfigurationFile != null) {
            if (logConfigurationFile.exists()) {
                LoggerContext context = (LoggerContext) LogManager.getContext(false);
                context.setConfigLocation(logConfigurationFile.toURI());
            }
        }
    }

    public void saveEPSXeConfiguration(File EPSXeDirectory) {
        FileReader fileReader;
        BufferedReader bufferedReader;
        FileWriter fileWriter;
        BufferedWriter bufferedWriter;
        File inputFile = new File("config.ini");
        File tempFile = new File("tempConfig.ini");
        if(!inputFile.exists()) {
            try {
                fileWriter = new FileWriter("config.ini", true);
                bufferedWriter = new BufferedWriter(fileWriter);
                bufferedWriter.write("absoluteEpsxeDirectory=" + EPSXeDirectory.getAbsolutePath());
                bufferedWriter.close();
                absoluteEpsxePathFile = new File(EPSXeDirectory.getAbsolutePath());
            } catch (IOException e) {
                ExceptionHandler.getInstance().logException("RuntimeException at saving the EPSXE configuration", e);
            }
        }
        else {
            //Save to temp file
            try {
                fileReader = new FileReader(inputFile);
                bufferedReader = new BufferedReader(fileReader);
                fileWriter = new FileWriter(tempFile, true);
                bufferedWriter = new BufferedWriter(fileWriter);
                boolean written = false;

                String configLine;
                while ((configLine = bufferedReader.readLine()) != null) {
                    if(configLine.contains("absoluteEpsxeDirectory")) {
                        bufferedWriter.write("absoluteEpsxeDirectory=" + EPSXeDirectory.getAbsolutePath());
                        written = true;
                    }
                    else {
                        bufferedWriter.write(configLine);
                    }
                    bufferedWriter.newLine();
                }
                if(!written) {
                    bufferedWriter.write("absoluteEpsxeDirectory=" + EPSXeDirectory.getAbsolutePath());
                }

                bufferedReader.close();
                bufferedWriter.close();
                inputFile.delete();
                boolean successful = tempFile.renameTo(inputFile);
                absoluteEpsxePathFile = new File(EPSXeDirectory.getAbsolutePath());
            } catch (IOException e) {
                ExceptionHandler.getInstance().logException("RuntimeException at loading the EPSXE configuration", e);
            }
        }
    }

    public void loadEPSXeConfiguration() {
        File configFile = new File("config.ini");
        if (configFile.exists()) {
            try {
                FileReader fileReader = new FileReader("config.ini");
                BufferedReader bufferedReader = new BufferedReader(fileReader);
                String configLine;
                while ((configLine = bufferedReader.readLine()) != null) {
                    if (configLine.contains("absoluteEpsxeDirectory")) {
                        absoluteEpsxePathFile = new File(configLine.split("=")[1]);
                    }
                    else if(configLine.contains("DEBUG")) {
                        isDebug = Boolean.parseBoolean(configLine.split("=")[1]);
                    }
                    else if(configLine.contains("stateAsTextField")) {
                        isStateAsTextField = Boolean.parseBoolean(configLine.split("=")[1]);
                    }
                    else if(configLine.contains("epsxeNoSound")) {
                        isEpsxeNoSound = Boolean.parseBoolean(configLine.split("=")[1]);
                    }
                }
                bufferedReader.close();
            } catch (IOException e) {
                ExceptionHandler.getInstance().logException("RuntimeException at loading the EPSXE configuration", e);
            }
        }
    }
}
