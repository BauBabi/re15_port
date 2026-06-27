package de.baubabi;

import de.baubabi.parser.HexParser;

import java.util.ArrayList;

public class Main {

    static ArrayList<String> xaList = new ArrayList<>();
    static int xaCounter = 0;

    public static void main(String[] args) {
        String hexData = HexParser.readFileAsHex("C:\\workspace\\Re2Data\\PL0\\VOICE\\VOICE00.XAS");

        readNextFile(hexData, 0);
    }

    private static void readNextFile(String hexData, int counter) {
        int index = readFileUntilPotentialEnd(hexData, counter);

        int fileEndOrNextFileIndex = findFileEndOrNextFileStart(hexData, index);

        if(fileEndOrNextFileIndex == hexData.length()) {
            HexParser.writeHexAsFile("C:\\workspace\\Re2Data-extract\\PL0\\VOICE\\VOICE00-" + xaCounter + ".XA", xaList);
            System.exit(0);
        } else {
            HexParser.writeHexAsFile("C:\\workspace\\Re2Data-extract\\PL0\\VOICE\\VOICE00-" + xaCounter + ".XA", xaList);
            xaList = new ArrayList<>();
            xaCounter = xaCounter + 1;
            if(xaCounter == 136) {
                System.out.println("NEXT");
            }
            readNextFile(hexData, fileEndOrNextFileIndex);
        }
    }

    private static int readFileUntilPotentialEnd(String hexData, int counter) {
        int foundZeroBytes = 0;

        do {
            String readByte = HexParser.readByte(hexData, counter);
            if (readByte.equals("00")) {
                foundZeroBytes = foundZeroBytes + 1;
                xaList.add(readByte);
                counter = counter + 2;
            } else {
                foundZeroBytes = 0;
                xaList.add(readByte);
                counter = counter + 2;
            }

        } while (foundZeroBytes < 150);

        return counter;
    }

    private static int findFileEndOrNextFileStart(String hexData, int counter) {
        boolean nextItem = false;

        do {
            if(!(counter + 2 > hexData.length())) {
                String readByte = HexParser.readByte(hexData, counter);
                if (readByte.equals("00")) {
                    xaList.add(readByte);
                    counter = counter + 2;
                } else {
                    nextItem = true;
                }
            }
            else {
                nextItem = true;
            }

        } while (!nextItem);

        return counter;
    }
}