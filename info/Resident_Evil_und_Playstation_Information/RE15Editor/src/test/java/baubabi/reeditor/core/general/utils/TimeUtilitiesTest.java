package baubabi.reeditor.core.general.utils;

import org.junit.Assert;
import org.junit.Test;

public class TimeUtilitiesTest {

    TimeUtilities timeUtility = TimeUtilities.getInstance();

    @Test
    public void getCurrentDateAndTime() {
        //Days can be 01, 02 ... until 09, then | 10, 11 ... or 20, 21 ... until 19 or 29 then | 30 or 31
        String days = "((0[1-9])|([12][0-9])|(3[01]))";
        String month = "((0[0-9])|(1[012]))";
        String years = "((20[012]\\d|19\\d\\d)|(1\\d|2[0123]))";
        String dateAndTimeConnectorIn = "-";
        String hours = "((0[0-9])|(1[0-9])|(2[01234]))";
        String minutes = "((0[0-9])|(1[0-9])|(2[0-9])|(3[0-9])|(4[0-9])|(5[0-9])|(6[0]))";
        String seconds = minutes;
        String dateAndTimeConnectorOut = "_";
        String regex = days + dateAndTimeConnectorIn + month + dateAndTimeConnectorIn + years + dateAndTimeConnectorOut + hours + dateAndTimeConnectorIn + minutes + dateAndTimeConnectorIn + seconds;
        Assert.assertTrue(timeUtility.getCurrentDateAndTime().matches("(" + regex + ")"));
    }
}