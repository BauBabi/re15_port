package baubabi.reeditor.core.general.utils;

import baubabi.reeditor.core.exception.ExceptionHandler;
import com.google.common.base.Splitter;
import com.google.common.primitives.Bytes;
import lombok.extern.log4j.Log4j2;
import org.apache.commons.codec.binary.Hex;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.filefilter.WildcardFileFilter;

import javax.xml.bind.DatatypeConverter;
import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.io.InputStream;
import java.net.JarURLConnection;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

@Log4j2
public class FileUtilities {
    private static FileUtilities INSTANCE;

    public static FileUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new FileUtilities();
        }

        return INSTANCE;
    }

    public int createDirectory(String directory) {
        boolean success;
        try {
            File directoryFile = new File(directory);
            success = directoryFile.mkdir();
        } catch (SecurityException se) {
            ExceptionHandler.getInstance().logException("SecurityException Directory creation!", se);
            return se.hashCode();
        }
        if (success) {
            return 0;
        } else {
            return 1;
        }
    }

    public int copyDirectory(String source, String destination) {
        File sourceDirectory = new File(source);
        File destinationDirectory = new File(destination);

        try {
            FileUtils.copyDirectory(sourceDirectory, destinationDirectory, false);
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at Copying directories!", e);
            return e.hashCode();
        }

        return 0;
    }

    public int copyFile(String source, String destination) {
        File sourceDirectory = new File(source);
        File destinationDirectory = new File(destination);

        try {
            FileUtils.copyFile(sourceDirectory, destinationDirectory, false);
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at Copying File!", e);
            return e.hashCode();
        }

        return 0;
    }

    public int removeFileFromDirectory(File source) {
        try {
            FileUtils.forceDelete(source);
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at deleting File!", e);
        }

        return 0;
    }

    public void copyDirectoryFromJarOrIDE(String resourceDirectory, String destinationDirectory) {
        //Creates the destination directory
        File file = new File(destinationDirectory);
        file.mkdir();

        //Fetches the element to copy out from the resources directory
        final URL configFolderURL = getClass().getResource("/" + resourceDirectory);
        if (configFolderURL == null) {
            log.error("URL to read in was null: /" + resourceDirectory);
            return;
        }
        //Copy the PsxImager Tools if we are in the IDE
        try {
            if (configFolderURL.openConnection().toString().contains("FileURLConnection")) {
                String resourceDirectorySource = DirectoryUtility.getInstance().getRESOURCE_DIRECTORY() + File.separator + resourceDirectory;
                FileUtilities.getInstance().copyDirectory(resourceDirectorySource, destinationDirectory);
                if(UserConfigurationUtility.getInstance().isDebug()) {
                    System.out.println("We are in IDE. Copy directory is easy.");
                }
                return;
            } else {
                if(UserConfigurationUtility.getInstance().isDebug()) {
                    System.out.println("We are in Jar, let's continue");
                }
            }

            Path targetDir;
            targetDir = Path.of(destinationDirectory);

            //Copy the Image Tools if we are within the .jar
            copyJarResourcesRecursively(targetDir, (JarURLConnection) configFolderURL.openConnection(), resourceDirectory);
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at copying from the jar or IDE", e);
        }
    }

    private void copyJarResourcesRecursively(Path destination, JarURLConnection jarConnection, String resourceDirectoryName) {
        JarFile jarFile;
        try {
            //Gets the .jar file
            jarFile = jarConnection.getJarFile();

            //Iterate through the Entries
            for (Iterator<JarEntry> it = jarFile.entries().asIterator(); it.hasNext(); ) {

                //Fetch next Entry
                JarEntry entry = it.next();

                //Check for Directory, if it is, create it at destination
                if (entry.getName().startsWith(jarConnection.getEntryName())) {
                    if (!entry.isDirectory()) {

                        //Get the single File and Copy it to the destination directory
                        try (InputStream entryInputStream = jarFile.getInputStream(entry)) {
                            Files.copy(entryInputStream, Paths.get(destination.toString(), entry.getName()));
                        }
                    } else {
                        Files.createDirectories(Paths.get(destination.toString(), entry.getName()));
                    }
                }
            }
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at the Copy directory from jar", e);
        }
    }

    public List<String> readFileAsHex(Path path) {
        if (Files.notExists(path)) {
            log.error("File to Convert to Hex does not exist");
            log.error("File not found! " + path);
        }

        List<String> hex = new ArrayList<>();

        //Read in File as Stream
        try (InputStream inputStream = Files.newInputStream(path)) {

            byte[] bytes = inputStream.readAllBytes();
            Iterable<String> pieces = Splitter.fixedLength(2).split(Hex.encodeHexString( bytes ).toUpperCase());
            pieces.forEach(value -> {
                hex.add(value);
            });

        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at reading in the file and convert to hex!", e);
            return null;
        }

        return hex;
    }

    public int writeHexAsFile(String destinationPath, List<String> hexData) {
        int success = 0;
        List<Integer> byteListToWrite = new ArrayList<>();
        File outputFile = new File(destinationPath);


        for (int i = 0; i < hexData.size(); i++) {
            int hexByteInteger = toByteArray(hexData.get(i))[0] & 0xFF;
            byteListToWrite.add(hexByteInteger);
        }

        byte[] bytes = Bytes.toArray(byteListToWrite);
        try {
            Files.write(outputFile.toPath(),bytes);
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("RuntimeException at writing image data", e);
            success = 1;
        }

        return success;
    }

    private byte[] toByteArray(String hexStringToConvertToByteArray) {
        return DatatypeConverter.parseHexBinary(hexStringToConvertToByteArray);
    }

    public File[] searchForFilesInDirectoryWithWildcard(File directoryToSearchIn, String wildcardArgument) {
        File[] files = directoryToSearchIn.listFiles((FileFilter) new WildcardFileFilter(wildcardArgument));
        return files;
    }

    public int splitHexListToAbsoluteFileByByteSize(List<String> hexList, int byteSizeForSplit, String absoluteFileDestinationDirectory) {
        int success = 0;
        int counter = 0;

        String fileExtension = absoluteFileDestinationDirectory.split("\\.")[1];

        absoluteFileDestinationDirectory = absoluteFileDestinationDirectory.replace("." + fileExtension, "");

        List<Integer> byteListToWrite;

        File outputFile;

        int byteSizeInLoop = byteSizeForSplit;

        if(hexList.size() > 96) {
            for (int i = 0; i < hexList.size(); i++) {
                byteListToWrite = new ArrayList<>();
                outputFile = new File(absoluteFileDestinationDirectory + "_" + counter + "." + fileExtension);
                counter = counter + 1;

                for (int j = i; j < byteSizeInLoop; j++) {
                    int hexByteInteger = toByteArray(hexList.get(j))[0] & 0xFF;
                    byteListToWrite.add(hexByteInteger);
                }

                byte[] bytes = Bytes.toArray(byteListToWrite);
                try {
                    Files.write(outputFile.toPath(),bytes);
                } catch (IOException e) {
                    ExceptionHandler.getInstance().logException("RuntimeException at writing image data", e);
                    success = 1;
                }

                i = (i + byteSizeForSplit) - 1;
                byteSizeInLoop = (byteSizeInLoop + byteSizeForSplit) - 1;
            }
        }

        return success;
    }
}
