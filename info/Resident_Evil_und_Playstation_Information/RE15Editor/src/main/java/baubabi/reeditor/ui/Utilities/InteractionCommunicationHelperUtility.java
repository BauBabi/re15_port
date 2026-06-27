package baubabi.reeditor.ui.Utilities;

import lombok.Getter;
import lombok.Setter;

import java.beans.PropertyChangeListener;

@Setter
@Getter
public class InteractionCommunicationHelperUtility {
    private static InteractionCommunicationHelperUtility INSTANCE;

    private PropertyChangeListener infoDetailTabPropertyChangeListener;

    public static InteractionCommunicationHelperUtility getInstance() {
        if(INSTANCE == null) {
            INSTANCE = new InteractionCommunicationHelperUtility();
        }

        return INSTANCE;
    }
}
