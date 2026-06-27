package baubabi.reeditor.core.image.data;

import org.junit.Test;

import java.io.File;

import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;

public class ImageTest {

    @Test
    public void testClearOfInstance() {
        Image image = Image.getInstance(System.getProperty("user.dir") + File.separator +
                "src" + File.separator + "test"  + File.separator + "resources" + File.separator +
                "ImageDirectory",

                "myPackedImage.bin");

        image = Image.getInstance();
        assertNotNull(image);

        image.clearImageInstance();

        image = Image.getInstance();
        assertNull(image);

    }
}