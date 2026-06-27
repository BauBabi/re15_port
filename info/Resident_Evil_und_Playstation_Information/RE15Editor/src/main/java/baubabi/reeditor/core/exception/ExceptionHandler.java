package baubabi.reeditor.core.exception;

import lombok.extern.log4j.Log4j2;

@Log4j2
public class ExceptionHandler {
    private static ExceptionHandler INSTANCE;

    public static ExceptionHandler getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new ExceptionHandler();
        }

        return INSTANCE;
    }

    public void logException(String logMessage, Exception exception) {
        log.error(logMessage);
        log.error("HASH: " + exception.hashCode());
        log.error("MESSAGE: " + exception.getMessage());
    }
}
