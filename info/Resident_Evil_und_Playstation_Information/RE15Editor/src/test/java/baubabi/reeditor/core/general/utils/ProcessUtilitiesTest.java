package baubabi.reeditor.core.general.utils;

import org.junit.Assert;
import org.junit.Ignore;
import org.junit.Test;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class ProcessUtilitiesTest {

    ProcessUtilities processUtility = ProcessUtilities.getInstance();

    @Test
    public void getOperatingSystem() {
        String operatingSystem = processUtility.getOperatingSystem().toLowerCase();
        assertTrue(operatingSystem.contains("windows") || operatingSystem.contains("linux") || operatingSystem.contains("mac"));
    }

    @Test
    public void executeProcessThrowsIOException() {
        ProcessUtilities processUtilities = mock(ProcessUtilities.class);

        when(processUtilities.executeProcess("", new ArrayList<>())).thenAnswer(invocation -> {
            throw new IOException();
        });
    }

    @Test
    public void executeProcessThrowsInterruptException() {
        ProcessUtilities processUtilities = mock(ProcessUtilities.class);

        when(processUtilities.executeProcess("", new ArrayList<>())).thenAnswer(invocation -> {
            throw new InterruptedException();
        });
    }

    @Test
    public void executeProcessTest() {
        ProcessUtilities processUtilities = mock(ProcessUtilities.class);

        //when
        when(processUtilities.executeProcess("", new ArrayList<>())).thenReturn(0);

        Assert.assertEquals(0, processUtilities.executeProcess("", new ArrayList<>()));
    }

    @Test
    public void ObjLoader() {
        ProcessUtilities processUtilities = mock(ProcessUtilities.class);

        //when
        when(processUtilities.ObjLoader("", "")).thenReturn(0);

        Assert.assertEquals(0, processUtilities.ObjLoader("", ""));
    }

    @Ignore("Test only for local execution to test 3d Modeler loading")
    @Test
    public void ObjLoaderTest() {
        ProcessUtilities processUtilities = ProcessUtilities.getInstance();

        String resourceDirectory = System.getProperty("user.dir") + File.separator + "src" + File.separator + "test"  + File.separator + "resources";
        String texture = resourceDirectory + File.separator + "3DObjects/bingo.jpg";
        String obj = resourceDirectory + File.separator + "3DObjects/testCube.obj";
        processUtilities.ObjLoader(obj, texture);

        //when
        when(processUtilities.ObjLoader("", "")).thenReturn(0);

        Assert.assertEquals(0, processUtilities.ObjLoader("", ""));
    }
}