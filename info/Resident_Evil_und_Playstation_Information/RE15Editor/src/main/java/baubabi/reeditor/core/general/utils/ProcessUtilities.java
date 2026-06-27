package baubabi.reeditor.core.general.utils;

import baubabi.reeditor.core.exception.ExceptionHandler;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.extern.log4j.Log4j2;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

@NoArgsConstructor
@Log4j2
@Getter
public class ProcessUtilities {

    private static ProcessUtilities INSTANCE;

    public static ProcessUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new ProcessUtilities();
        }

        return INSTANCE;
    }

    public String getOperatingSystem() {

        return System.getProperty("os.name");
    }

    public int executeProcess(String workDirectory, List<String> arguments) {
        ProcessBuilder extractionProcessBuilder;
        File workDirectoryFile = new File(workDirectory);

        extractionProcessBuilder = new ProcessBuilder(arguments);

        extractionProcessBuilder.directory(workDirectoryFile);

        extractionProcessBuilder.environment().put("PYTHONUNBUFFERED", "TRUE");
        extractionProcessBuilder.redirectErrorStream(true);
        extractionProcessBuilder.redirectInput(ProcessBuilder.Redirect.INHERIT)
                .redirectOutput(ProcessBuilder.Redirect.INHERIT)
                .redirectError(ProcessBuilder.Redirect.INHERIT);


        Process extractionProcess;
        try {
            extractionProcess = extractionProcessBuilder.start();
            if(UserConfigurationUtility.getInstance().isDebug()) {
                System.out.println("ARUGMENTS: " + arguments);
            }

            BufferedReader output = getOutput(extractionProcess);
            BufferedReader error = getError(extractionProcess);
            String line;

            while ((line = output.readLine()) != null) {
                if(UserConfigurationUtility.getInstance().isDebug()) {
                    System.out.println(line);
                }
            }

            while ((line = error.readLine()) != null) {
                if(UserConfigurationUtility.getInstance().isDebug()) {
                    System.out.println(line);
                }
            }
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at Image Ripping Process!", e);
            return e.hashCode();
        }

        try {
            extractionProcess.waitFor();
        } catch (InterruptedException e) {
            ExceptionHandler.getInstance().logException("InterruptedException at Image Ripping Process!", e);
            return e.hashCode();
        }

        return extractionProcess.exitValue();
    }

    private static BufferedReader getOutput(Process p) {
        return new BufferedReader(new InputStreamReader(p.getInputStream()));
    }

    private static BufferedReader getError(Process p) {
        return new BufferedReader(new InputStreamReader(p.getErrorStream()));
    }

    public int ObjLoader(String objectPath, String objectTexturePath) {
        List<String> arguments = new ArrayList<>();
        arguments.add("python");
        arguments.add("./3dLoader.py");
        arguments.add(objectPath);
        arguments.add(objectTexturePath);

        //TODO - When we start to use 3D models, we have to copy it outside of the jar and replace the direct directory
        return executeProcess(DirectoryUtility.getInstance().getRESOURCE_DIRECTORY() + File.separator + "PythonHelperTools", arguments);
    }

    public void startEmulator(File absoluteImageDirectory) {
        if(UserConfigurationUtility.getInstance().getAbsoluteEpsxePathFile() != null) {
            List<String> arguments = new ArrayList<>();
            if (this.getOperatingSystem().toLowerCase().contains("windows")) {
                arguments.add("cmd");
                arguments.add("/c");
                arguments.add(UserConfigurationUtility.getInstance().getAbsoluteEpsxePathFile().getAbsolutePath());
            } else {
                arguments.add("bash");
                arguments.add("-c");
                arguments.add(UserConfigurationUtility.getInstance().getAbsoluteEpsxePathFile().getAbsolutePath());
            }

            if(!UserConfigurationUtility.getInstance().isEpsxeNoSound()) {
                arguments.set(2, arguments.get(2) + " -nogui -loadbin " + absoluteImageDirectory.getAbsolutePath());
            }
            else {
                arguments.set(2, arguments.get(2) + " -nogui -nosound -loadbin " + absoluteImageDirectory.getAbsolutePath());
            }

            int success = this.executeProcess(UserConfigurationUtility.getInstance().getAbsoluteEpsxePathFile().getParent(), arguments);

            if (success != 0) {
                log.error("Error starting Emulator. Return Code: " + success);
            }
        }
        else {
            System.out.println("EPSXE Path has not been set");
        }
    }

    public void extractBackgroundImages(File[] imageDirectories, File bss2bmpDirectory) {
        List<String> arguments;
        int success;

        System.out.println("EXTRACT THE BACKGROUND IMAGES:");
        System.out.println();
        arguments = new ArrayList<>();
        if (this.getOperatingSystem().toLowerCase().contains("windows")) {
            for (int i = 0; i < imageDirectories.length; i++) {
                if (!imageDirectories[i].isDirectory()) {
                    arguments.add("cmd");
                    arguments.add("/c");
                    arguments.add(bss2bmpDirectory.getAbsolutePath() + File.separator + "bss2bmp.exe");
                    arguments.add("./" + imageDirectories[i].getName());
                    arguments.add("&&");
                }
            }
            if (arguments.size() > 0) {
                arguments.remove(arguments.size() - 1);

                success = this.executeProcess(bss2bmpDirectory.getAbsolutePath(), arguments);
                if (success != 0) {
                    log.error("Error Create BMPs. Return Code: " + success);
                }

                System.out.println();
            }
        }
        else {
            for (int i = 0; i < imageDirectories.length; i++) {
                arguments = new ArrayList<>();
                if (!imageDirectories[i].isDirectory()) {
                    arguments.add("bash");
                    arguments.add("-c");
                    arguments.add(bss2bmpDirectory.getAbsolutePath() + File.separator + "bss2bmp ./" + imageDirectories[i].getName());
                    success = this.executeProcess(bss2bmpDirectory.getAbsolutePath(), arguments);
                    if (success != 0) {
                        log.error("Error Create BMPs. Return Code: " + success);
                    }

                    System.out.println();
                }
            }
        }
    }
}
