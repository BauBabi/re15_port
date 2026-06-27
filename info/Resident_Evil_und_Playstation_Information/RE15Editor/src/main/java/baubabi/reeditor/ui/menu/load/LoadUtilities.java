package baubabi.reeditor.ui.menu.load;

import baubabi.reeditor.core.image.data.Image;
import baubabi.reeditor.core.image.utils.ImageUtilities;
import baubabi.reeditor.core.rdt.RdtProcessing;

public class LoadUtilities {
    private static LoadUtilities INSTANCE;

    private static Image image;

    private static ImageUtilities imageUtilities;

    public static LoadUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new LoadUtilities();
        }

        return INSTANCE;
    }

    public void initializeImageUtilities() {
        image = Image.getInstance();

        if (ImageUtilities.getInstance() != null) {
            ImageUtilities.getInstance().clearImageUtilityInstance();
        }
        imageUtilities = ImageUtilities.getInstance(image);
    }

    public void extractImage() {
        imageUtilities.extractImage();
    }

    public void loadRdtElements() {
        RdtProcessing rdtProcessing = RdtProcessing.getInstance();

        rdtProcessing.parseRdt(image.getImageRdtDirectories().get(0));

        rdtProcessing.parseScdElements();
    }
}
