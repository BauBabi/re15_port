package baubabi.reeditor.core.image.utils;

import baubabi.reeditor.core.image.data.Image;
import org.junit.Assert;
import org.junit.Test;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class ImageUtilitiesTest {

    Image image = new Image(
            System.getProperty("user.dir") + File.separator +
                    "src" + File.separator + "test"  + File.separator + "resources" + File.separator +
                    "ImageDirectory",

            "myPackedImage.bin");

    @Test
    public void testClearOfInstance() {

        ImageUtilities.getInstance(image);
        assertNotNull(ImageUtilities.getInstance());

        ImageUtilities.getInstance().clearImageUtilityInstance();

        assertNull(ImageUtilities.getInstance());

    }

    @Test
    public void extractImageTest() {

        ImageUtilities imageUtilities = ImageUtilities.getInstance(image);

        Assert.assertNotNull(imageUtilities);

        if(new File(image.getImageSourceDirectory() + File.separator + image.getImageName()).exists()) {
            assertEquals(0, imageUtilities.extractImage());
        }
        else {
            Assert.assertTrue(imageUtilities.extractImage() > 0);
        }
    }

    @Test
    public void getPsxRipExecutorForLinux() {
        ImageUtilities imageUtilities = mock(ImageUtilities.class);

        List<String> arguments = new ArrayList<>();

        arguments.add("bash");
        arguments.add("-c");
        arguments.add("./psxrip");

        //when
        when(imageUtilities.getPsxRipExecutor()).thenReturn(arguments);

        Assert.assertEquals(imageUtilities.getPsxRipExecutor(), arguments);
    }
    @Test
    public void getPsxRipExecutorForWindows() {
        ImageUtilities imageUtilities = mock(ImageUtilities.class);

        List<String> arguments = new ArrayList<>();

        arguments.add("cmd");
        arguments.add("/c");
        arguments.add("psxrip.exe");

        //when
        when(imageUtilities.getPsxRipExecutor()).thenReturn(arguments);

        Assert.assertEquals(imageUtilities.getPsxRipExecutor(), arguments);
    }

    @Test
    public void packImageTest() {

        ImageUtilities imageUtilities = ImageUtilities.getInstance(image);

        if(new File(image.getImageSourceDirectory() + File.separator + image.getImageName()).exists()) {
            assertEquals(imageUtilities.extractImage(), 0);
            Assert.assertEquals(imageUtilities.packImage(), 0);
        }
        else {
            Assert.assertTrue(imageUtilities.extractImage() > 0);
        }
    }

}