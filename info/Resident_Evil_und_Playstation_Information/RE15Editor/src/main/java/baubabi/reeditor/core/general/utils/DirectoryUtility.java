package baubabi.reeditor.core.general.utils;

import baubabi.reeditor.core.exception.ExceptionHandler;
import lombok.Getter;
import lombok.extern.log4j.Log4j2;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;

@Getter
@Log4j2
public class DirectoryUtility {
    private static DirectoryUtility INSTANCE;

    private final String PROJECT_DIRECTORY = System.getProperty("user.dir");

    private String RESOURCE_DIRECTORY;

    public DirectoryUtility() {
        final URL configFolderURL = getClass().getResource("/bss2bmpExecutor");
        if (configFolderURL == null) {
            log.error("URL to read in was null: /bss2bmpExecutor");
            return;
        }
        //Copy the PsxImager Tools if we are in the IDE
        try {
            if (configFolderURL.openConnection().toString().contains("FileURLConnection")) {
                RESOURCE_DIRECTORY = PROJECT_DIRECTORY + File.separator + "src" + File.separator + "main" + File.separator + "resources";
            }
            else {
                RESOURCE_DIRECTORY = "";
            }
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException at getting the Resource Directory", e);
        }
    }

    public static DirectoryUtility getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new DirectoryUtility();
        }

        return INSTANCE;
    }

    public InputStream getResourcesElement(String path) {
        ClassLoader classLoader = getClass().getClassLoader();
        return classLoader.getResourceAsStream(path);
    }
}
