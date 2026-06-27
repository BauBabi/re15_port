package baubabi.reeditor.core.general.utils;

import lombok.extern.log4j.Log4j2;
import org.junit.Assert;
import org.junit.Test;

import java.io.File;

@Log4j2
public class FileUtilitiesTest {

    @Test
    public void testDirectoryCreationWithExistingPath() {
        FileUtilities fileUtil = FileUtilities.getInstance();
        int success = fileUtil.createDirectory(System.getProperty("java.io.tmpdir") + File.separator + TimeUtilities.getInstance().getCurrentDateAndTime() + "TEST");

        Assert.assertEquals(success, 0);
    }

    @Test
    public void testCopyDirectoryCreationWithExistingDirectory() {
        FileUtilities fileUtil = FileUtilities.getInstance();

        int success = fileUtil.copyDirectory("src/main/resources/PsxImager", System.getProperty("java.io.tmpdir"));
        Assert.assertEquals(success, 0);
    }

    @Test
    public void testCopyDirectoryCreationWithNonExistingSourceDirectory() {
        FileUtilities fileUtil = FileUtilities.getInstance();

        int success = fileUtil.copyDirectory("I/DO/NOT/EXIST", System.getProperty("java.io.tmpdir"));

        Assert.assertTrue(success > 0);
    }

    @Test
    public void testCopyFile() {
        FileUtilities fileUtil = FileUtilities.getInstance();

        int success = fileUtil.copyFile("src/test/resources/exampleFileToCopy.txt", System.getProperty("java.io.tmpdir") + File.separator + "exampleFileToCopy.txt");

        Assert.assertEquals(success, 0);
    }
}