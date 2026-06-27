package baubabi.reeditor.core.image.utils;

import baubabi.reeditor.core.general.utils.DirectoryUtility;
import baubabi.reeditor.core.general.utils.FileUtilities;
import baubabi.reeditor.core.general.utils.ProcessUtilities;
import baubabi.reeditor.core.general.utils.TimeUtilities;
import baubabi.reeditor.core.image.data.Image;
import lombok.Getter;
import lombok.extern.log4j.Log4j2;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

@Getter
@Log4j2
public class ImageUtilities {

    private static ImageUtilities INSTANCE;

    private static Image image;

    private final FileUtilities fileUtilities;

    private final ProcessUtilities processUtilities;

    private final DirectoryUtility directoryUtility = DirectoryUtility.getInstance();

    private final String workDirectory = System.getProperty("java.io.tmpdir") + File.separator + TimeUtilities.getInstance().getCurrentDateAndTime();

    private final String PSXIMAGER_DIRECTORY = workDirectory + File.separator + "PsxImager";

    private final String BSS2BMP_DIRECTORY = workDirectory + File.separator + "BSS2BMP";


    private ImageUtilities(Image imageInstance) {
        image = imageInstance;
        fileUtilities = FileUtilities.getInstance();
        processUtilities = ProcessUtilities.getInstance();

        //Extracts the PsxImager directory to rip and rebuild the playstation image from the jar file
        fileUtilities.copyDirectoryFromJarOrIDE("PsxImager", PSXIMAGER_DIRECTORY);
        fileUtilities.copyDirectoryFromJarOrIDE("bss2bmpExecutor", BSS2BMP_DIRECTORY);

        //Sets the Permission for the PsxImager elements to make them executable
        if (this.processUtilities.getOperatingSystem().toLowerCase().contains("linux")) {
            List<String> arguments = new ArrayList<>();
            arguments.add("bash");
            arguments.add("-c");
            arguments.add("chmod -R 777 .");
            processUtilities.executeProcess(workDirectory, arguments);
        }

        File psxImagerDirectoryUnix = new File(PSXIMAGER_DIRECTORY + File.separator + "PsxImager");
        File bss2bmpDirectoryUnix = new File(BSS2BMP_DIRECTORY + File.separator + "bss2bmpExecutor");

        if(psxImagerDirectoryUnix.exists() && psxImagerDirectoryUnix.isDirectory()) {
            File[] psxImagerFiles = psxImagerDirectoryUnix.listFiles();
            for(int i = 0; i < psxImagerFiles.length; i++) {
                System.out.println("FILE TO COPY: " + psxImagerFiles[i].getAbsolutePath());
                if(psxImagerFiles[i].isFile()) {
                    FileUtilities.getInstance().copyFile(psxImagerFiles[i].getAbsolutePath(), PSXIMAGER_DIRECTORY + File.separator + psxImagerFiles[i].getName());
                    FileUtilities.getInstance().removeFileFromDirectory(psxImagerFiles[i]);
                }
            }
            psxImagerDirectoryUnix.delete();
        }

        if(bss2bmpDirectoryUnix.exists() && bss2bmpDirectoryUnix.isDirectory()) {
            File[] bss2bmpFiles = bss2bmpDirectoryUnix.listFiles();
            for(int i = 0; i < bss2bmpFiles.length; i++) {
                System.out.println("FILE TO COPY: " + bss2bmpFiles[i].getAbsolutePath());
                if(bss2bmpFiles[i].isFile()) {
                    FileUtilities.getInstance().copyFile(bss2bmpFiles[i].getAbsolutePath(), BSS2BMP_DIRECTORY + File.separator + bss2bmpFiles[i].getName());
                    FileUtilities.getInstance().removeFileFromDirectory(bss2bmpFiles[i]);
                }
            }
            bss2bmpDirectoryUnix.delete();
        }
    }

    public static ImageUtilities getInstance(Image image) {
        if (INSTANCE == null) {
            INSTANCE = new ImageUtilities(image);
        }

        return INSTANCE;
    }

    public static ImageUtilities getInstance() {
        if (INSTANCE == null) {
            return null;
        }

        return INSTANCE;
    }

    public void clearImageUtilityInstance() {
        INSTANCE = null;
    }

    public int extractImage() {
        int success;
        success = createWorkDirectoryIfNotExist();
        if (success != 0) {
            return success;
        }

        success = copyTheImageFileToWorkDirectory();
        if (success != 0) {
            return success;
        }

        success = checkForCueFileForImage_CopyToWorkingDirectory();
        if (success != 0) {
            return success;
        }

        success = extractTheImageInTheWorkingDirectory();
        if (success != 0) {
            return success;
        }

        success = renameExtractedImageDirectoryLikeCatFile();
        if (success != 0) {
            return success;
        }

        success = readOutRemainingNecessaryImageData();

        return success;
    }

    public int packImage() {
        return packTheImageInTheWorkingDirectory();
    }

    public List<String> getPsxRipExecutor() {
        List<String> arguments = new ArrayList<>();
        if (this.processUtilities.getOperatingSystem().toLowerCase().contains("windows")) {
            arguments.add("cmd");
            arguments.add("/c");
            arguments.add("psxrip.exe");
        } else {
            arguments.add("bash");
            arguments.add("-c");
            arguments.add("./psxrip");
        }
        return arguments;
    }

    private int createWorkDirectoryIfNotExist() {
        int success = 0;
        File workDirectory = new File(image.getWorkDirectory());
        if (!workDirectory.exists()) {
            success = this.fileUtilities.createDirectory(image.getWorkDirectory());
            if (success != 0) {
                return success;
            }
        }
        return success;
    }

    private int copyTheImageFileToWorkDirectory() {
        int success;
        success = this.fileUtilities.copyFile(image.getImageSourceDirectory() + File.separator + image.getImageName(),
                image.getWorkDirectory() + File.separator + image.getImageName());
        if (success != 0) {
            return success;
        }
        return success;
    }

    private int checkForCueFileForImage_CopyToWorkingDirectory() {
        int success = 0;
        File imageDirectory = new File(image.getImageSourceDirectory());
        File[] cueFiles = fileUtilities.searchForFilesInDirectoryWithWildcard(imageDirectory, "*.cue");
        for (int i = 0; i < Objects.requireNonNull(cueFiles).length; i++) {
            File cueFile = cueFiles[i];
            System.out.println("Cue file found: " + cueFile.getAbsolutePath());
            success = this.fileUtilities.copyFile(cueFile.getAbsolutePath(), image.getWorkDirectory() + File.separator + cueFile.getName());
            if (success != 0) {
                return success;
            }
        }
        return success;
    }

    private int extractTheImageInTheWorkingDirectory() {
        int success;

        List<String> arguments = getPsxRipExecutor();

        arguments.set(2, arguments.get(2) + " '" + image.getWorkDirectory() +
                File.separator +
                image.getImageName() + "'");

        success = this.processUtilities.executeProcess(this.getPSXIMAGER_DIRECTORY(), arguments);

        return success;
    }

    private int renameExtractedImageDirectoryLikeCatFile() {
        int success;
        String absoluteExtractedImageDirectory = image.getWorkDirectory() + File.separator + image.getImageName().replace(".bin", "");
        File extractedImageDirectoryFile = new File(absoluteExtractedImageDirectory);

        File workDirectoryForImageFile = new File(image.getWorkDirectory());

        File[] files = FileUtilities.getInstance().searchForFilesInDirectoryWithWildcard(workDirectoryForImageFile, "*.cat");

        File catName = files[0];

        System.out.println("IMAGE DIRECTORY: " + extractedImageDirectoryFile.getAbsolutePath());
        System.out.println("CAT FILE NAME: " + catName.getAbsolutePath());

        boolean successful = extractedImageDirectoryFile.renameTo(new File(catName.getAbsolutePath().replace(".cat", "")));

        image.setCatFile(catName);
        image.setImageExtractionDirectory(new File(catName.getAbsolutePath().replace(".cat","")));

        if (successful) {
            success = 0;
        } else {
            success = 1;
        }

        return success;
    }

    private int readOutRemainingNecessaryImageData() {
        int success = 0;

        File imagePsxSubDirectory = new File(image.getImageExtractionDirectory() + File.separator + "PSX");

        File[] rdtStageDirectories = fileUtilities.searchForFilesInDirectoryWithWildcard(imagePsxSubDirectory, "STAGE*");

        ArrayList<File> rdtFileList = new ArrayList<>();
        for(int i = 0; i < rdtStageDirectories.length; i++) {
            File[] rdtFilesArray = fileUtilities.searchForFilesInDirectoryWithWildcard(rdtStageDirectories[i], "*.RDT");
            Collections.addAll(rdtFileList, rdtFilesArray);
        }
        Collections.sort(rdtFileList);

        image.setImageStageDirectories(rdtStageDirectories);

        image.setImageRdtDirectories(rdtFileList);

        return success;
    }

    public List<String> getPsxBuildExecutor() {
        List<String> arguments = new ArrayList<>();
        if (this.processUtilities.getOperatingSystem().toLowerCase().contains("windows")) {
            arguments.add("cmd");
            arguments.add("/c");
            arguments.add("psxbuild.exe");
        } else {
            arguments.add("bash");
            arguments.add("-c");
            arguments.add("./psxbuild");
        }
        return arguments;
    }

    private int packTheImageInTheWorkingDirectory() {
        int success;

        //Prepare command for build execution
        List<String> arguments = getPsxBuildExecutor();

        arguments.set(2, arguments.get(2) + " '" + image.getCatFile().getAbsolutePath() + "' '" + image.getWorkDirectory() + File.separator + image.getImageName().replace(".bin", "") + "_modified.bin'");

        //Execute command
        success = this.processUtilities.executeProcess(this.getPSXIMAGER_DIRECTORY(), arguments);

        return success;
    }
}
