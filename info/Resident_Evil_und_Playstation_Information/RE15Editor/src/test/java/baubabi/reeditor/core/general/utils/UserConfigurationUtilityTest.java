package baubabi.reeditor.core.general.utils;

import org.junit.Test;

import java.io.File;

import static org.junit.Assert.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;

public class UserConfigurationUtilityTest {

    UserConfigurationUtility userConfigurationUtility = UserConfigurationUtility.getInstance();

    @Test
    public void testEPSXeConfigLoading() {
        File file = new File("config.ini");
        if(file.exists()) {
            userConfigurationUtility.loadEPSXeConfiguration();
            assertNotNull(userConfigurationUtility.getAbsoluteEpsxePathFile());
        }
        else {
            assertNull(userConfigurationUtility.getAbsoluteEpsxePathFile());
        }
    }

    @Test
    public void testSaveEPSXeConfiguration() {
        File file = new File("myEpsxe.exe");
        userConfigurationUtility.saveEPSXeConfiguration(file);
        userConfigurationUtility.loadEPSXeConfiguration();

        assert(userConfigurationUtility.getAbsoluteEpsxePathFile().getAbsolutePath().contains("myEpsxe.exe"));
    }
}