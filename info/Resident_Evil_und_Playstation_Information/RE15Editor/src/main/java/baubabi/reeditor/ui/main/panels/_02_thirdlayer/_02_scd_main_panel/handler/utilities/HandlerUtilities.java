package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.utilities;

import baubabi.reeditor.core.exception.ExceptionHandler;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;

public class HandlerUtilities {
    private static HandlerUtilities INSTANCE;

    public static HandlerUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new HandlerUtilities();
        }

        return INSTANCE;
    }

    public String filler(String originalHex, int elementSize) {
        if(originalHex.length() %2 == 1) {
            originalHex = "0" + originalHex;
        }

        if(originalHex.length() < elementSize*2) {
            for(int i = originalHex.length(); i < elementSize*2; i = i+2) {
                originalHex = "00" + originalHex;
            }
        }

        return originalHex;
    }

    public static String reverseHex(String originalHex) {
        // TODO: Validation that the length is even
        int lengthInBytes = originalHex.length() / 2;
        char[] chars = new char[lengthInBytes * 2];
        for (int index = 0; index < lengthInBytes; index++) {
            int reversedIndex = lengthInBytes - 1 - index;
            chars[reversedIndex * 2] = originalHex.charAt(index * 2);
            chars[reversedIndex * 2 + 1] = originalHex.charAt(index * 2 + 1);
        }
        return new String(chars);
    }

    public String fetchAndReplaceScdElementData(List<Object> singleScdDataElements) {
        String newSubScdData = "";
        for(int i = 0; i < singleScdDataElements.size(); i++) {
            Method method = null;
            try {
                method = singleScdDataElements.get(i).getClass().getDeclaredMethod("prepareNewScdDataByteForSaving", String.class);
            } catch (NoSuchMethodException e) {
                ExceptionHandler.getInstance().logException("NoSuchMethodException at updating the full scd data at saving", e);
            }
            try {
                newSubScdData = (String) method.invoke(singleScdDataElements.get(i), newSubScdData);
            } catch (IllegalAccessException e) {
                ExceptionHandler.getInstance().logException("IllegalAccessException at updating the full scd data at saving", e);
            } catch (InvocationTargetException e) {
                ExceptionHandler.getInstance().logException("InvocationTargetException at updating the full scd data at saving", e);
            }
        }
        return newSubScdData;
    }
}
