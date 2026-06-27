package de.baubabi;

import de.baubabi.types.Tim;
import de.baubabi.types.TypeDefault;

public class TypeFactory {

    public TypeDefault createType(String identifier) {
        switch (identifier) {
            case "10":
                return new Tim();
            case "20":
                return new Tim();
            case "XX":
                return new Tim();
            default:
                throw new IllegalArgumentException("Ungültiger Wert");
        }
    }
}
