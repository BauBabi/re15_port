package de.baubabi;

import de.baubabi.file.FileContent;
import de.baubabi.parser.HexParser;
import de.baubabi.types.TypeDefault;

public class Main {

    static TypeFactory typeFactory = new TypeFactory();


    public static void main(String[] args) {
        FileContent timContent = new FileContent(HexParser.readFileAsHex("example2.tim"));
        String byteData = HexParser.readByte(timContent.getFileContent(), 0);
        TypeDefault type = typeFactory.createType(byteData);
        boolean element = type.isElement(timContent.getFileContent(), 0);
    }
}