package de.re15.tools;

import de.re15.extractors.emd.*;
import java.nio.file.Path;

public final class EmdDebugMain {
    public static void main(String[] args) throws Exception {
        EmdParseResult result = EmdFileParser.parse(Path.of("extracted/PSX/EMD/CDEMD0/em10.emd"));
        System.out.println("skeleton IDs: " + result.getSkeletons().keySet());
    }
}
