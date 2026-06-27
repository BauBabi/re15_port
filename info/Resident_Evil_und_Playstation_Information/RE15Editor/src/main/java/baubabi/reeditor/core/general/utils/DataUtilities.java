package baubabi.reeditor.core.general.utils;

import lombok.Getter;

@Getter
public class DataUtilities {
    private static DataUtilities INSTANCE;

    public static DataUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new DataUtilities();
        }

        return INSTANCE;
    }

    public int convertHexToInt(String hexData) {
        return Integer.parseInt(hexData, 16);
    }
}
