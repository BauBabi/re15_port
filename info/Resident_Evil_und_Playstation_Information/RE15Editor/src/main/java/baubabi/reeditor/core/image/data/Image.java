package baubabi.reeditor.core.image.data;

import baubabi.reeditor.core.general.utils.TimeUtilities;
import lombok.Getter;

import java.io.File;
import java.util.ArrayList;

@Getter
public class Image {

    //Everything that describes the Image
    private static Image INSTANCE;

    private final String imageSourceDirectory;

    private final String imageName;

    private final String workDirectory;

    private File catFile;

    private File imageExtractionDirectory;

    private File[] imageStageDirectories;

    private ArrayList<File> imageRdtDirectories;

    public Image(String imageSourceDirectory, String imageName) {
        this.imageSourceDirectory = imageSourceDirectory;
        this.imageName = imageName;
        this.workDirectory = System.getProperty("java.io.tmpdir") + File.separator + TimeUtilities.getInstance().getCurrentDateAndTime();
    }

    public static Image getInstance(String imageDirectory, String imageName) {
        if(INSTANCE == null) {
            INSTANCE = new Image(imageDirectory, imageName);
        }

        return INSTANCE;
    }

    public static Image getInstance() {
        if(INSTANCE == null) {
            return null;
        }

        return INSTANCE;
    }

    public void clearImageInstance() {
        INSTANCE = null;
    }

    public void setCatFile(File catFile) {
        this.catFile = catFile;
    }

    public void setImageExtractionDirectory(File imageExtractionDirectory) {
        this.imageExtractionDirectory = imageExtractionDirectory;
    }

    public void setImageStageDirectories(File[] imageStageDirectories) {
        this.imageStageDirectories = imageStageDirectories;
    }

    public void setImageRdtDirectories(ArrayList<File> imageRdtDirectories) {
        this.imageRdtDirectories = imageRdtDirectories;
    }
}
