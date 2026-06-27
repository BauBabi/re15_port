package baubabi.reeditor.core.general.utils;

import lombok.Getter;

import java.text.SimpleDateFormat;
import java.util.Date;

@Getter
public class TimeUtilities {

    private static TimeUtilities INSTANCE;

    private final String currentDateAndTime;

    private TimeUtilities() {
        SimpleDateFormat formatter = new SimpleDateFormat("dd-MM-yyyy_HH-mm-ss");
        Date date = new Date();
        this.currentDateAndTime = formatter.format(date);
    }

    public static TimeUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new TimeUtilities();
        }

        return INSTANCE;
    }
}
