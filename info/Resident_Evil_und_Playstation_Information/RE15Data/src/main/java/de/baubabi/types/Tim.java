package de.baubabi.types;

import de.baubabi.parser.HexParser;
import de.baubabi.types.element.Rgb;

import java.util.HashMap;
import java.util.Optional;

public class Tim implements TypeDefault {
    //HEADER
    //Byte 1 -> id
    //Byte 2 -> version
    //Byte 3-4 -> Reserved
    //Byte 5-8 -> BPP & Clut Flag

    //CLUT HEADER
    //Byte 9-12 -> Clut Length/Size in Byte - only exists, if Clut flag = 1 (look at Byte 5-8) This includes the Header as well including this length/size field here. Let's say this value is for example 44, than the whole CLUT is 44 Bytes
    //Byte 13-14 -> X-Position in VRAM
    //Byte 15-16 -> Y-Position in VRAM
    //Byte 17-18 -> Image width
    //Byte 19-20 -> Image Height

    //CLUT BODY
    //Next we need the amount of ClutImageColors. That we get by ImageWidth*ImageHeight.
    //Lets say Width is 1 Height is 16, than we have 1*16=16 ImageColors.
    //If we have Width is 2 Height is 16, than we have 2*16=32 ImagesColors and so on.
    //So, the next bytes depends on the amount of colors. Each color has 2 byte.
    //2 Bytes means we have 16 Bit. The color is stored as follows: (0)=STP;(1-5)=R;(6-10)=G;(11-16)=B
    //The STP again is a transparency flag, which means different things according to the RGB Value:
    // R,G,B,STP = 0 -> transparent
    // R,G,B = 0, STP = 1 -> non-transparent
    // R,G,B = 1, STP = 0 -> not transparent
    // R,G,B,STP = 1 -> semi transparent
    //With 2 Bytes per color, it means, when we have 16 ImageColors, we have 16*2=32 Bytes.
    //In the example above - lets say we have a Clut length/size of 44 bytes - we can now calculate if it fits
    //-> The CLUT Header - from clut length/size until Image Height, we have 12 Byte.
    //-> We have 32 Bytes of ImageColors. Header+ImageColors must have the same byte size like the value of clut length/size -> 12Byte+32Byte = 44 Byte.


    String idValue = "";
    String versionValue = "";
    String reserved1Value = "";
    String bitPerPixel = "";
    String clutFlag = "";
    String bppAndClutFlagValue = "";
    int CLUTHEADERBYTES = 12;
    int sizeOfClutInBytesBNum;
    String sizeOfClutValue = "";
    int clutXLocation;
    String clutXLocationValue = "";
    int clutYLocation;
    String clutYLocationValue = "";
    int clutImageWidth;
    String clutImageWidthValue = "";
    int clutImageHeight;
    String clutImageHeightValue = "";
    int numOfClutColors;
    Rgb[] rgbArray;

    HashMap<Integer, String> bitsPerPixelMap = new HashMap<>();

    public Tim() {
        bitsPerPixelMap.put(0, "4 bit CLUT");
        bitsPerPixelMap.put(1, "8 bit CLUT");
        bitsPerPixelMap.put(2, "15 bit Direct");
        bitsPerPixelMap.put(3, "24 bit Direct");
        bitsPerPixelMap.put(4, "Mixed");

    }

    public boolean isElement(String hexToCheck, int startIndex) {
        int hexCursor = startIndex;
        int finalCurrentCursor = 0;

        if(hasId(HexParser.readByte(hexToCheck, hexCursor))) {
            hexCursor = hexCursor + 2;

            if(hasVersion(HexParser.readByte(hexToCheck, hexCursor))) {
                hexCursor = hexCursor + 2;

                if(hasReserved1(HexParser.readUShort2Byte(hexToCheck, hexCursor))) {
                    hexCursor = hexCursor + 4;

                    if(hasBpp(HexParser.readULong4Byte(hexToCheck, hexCursor))) {
                        if(hasClutFlag(HexParser.readULong4Byte(hexToCheck, hexCursor))) {
                            hexCursor = hexCursor + 8;

                            if(clutFlag.equals("0")) {

                            }
                            else if(clutFlag.equals("1")) {

                                sizeOfClutInBytesBNum = fetchClutSize(HexParser.readULong4Byte(hexToCheck, hexCursor));
                                int endOfClutArea = hexCursor + sizeOfClutInBytesBNum*2;

                                //For Clut inspection we only work with Clut Hex Data
                                String clutHexValue = hexToCheck.substring(hexCursor, endOfClutArea);

                                if(setClutData(clutHexValue)) {
                                    hexCursor = endOfClutArea; //Set the end, for the Work after Cluts

                                }
                            }
                            else {
                                return false;
                            }
                        }
                    }
                }
            }

            return true;
        }

        return false;
    }

    public boolean hasId(String hexToCheck) {
        if(hexToCheck.equals("10")) {
            idValue = HexParser.readByte(hexToCheck, 0);
            return true;
        }
        return false;
    }

    private boolean hasVersion(String hexToCheck) {
        if(hexToCheck.equals("00")) {
            versionValue = HexParser.readByte(hexToCheck, 0);
            return true;
        }
        return false;
    }

    private boolean hasReserved1(String hexToCheck) {
        if(hexToCheck.equals("0000")) {
            reserved1Value = HexParser.readUShort2Byte(hexToCheck, 0);
            return true;
        }
        return false;
    }

    private boolean hasBpp(String hexToCheck) {
        String binaryStringOfInt = hexToBinary(hexToCheck);

        String pmodeString = binaryStringOfInt.substring(13,16);
        int bppNumber = Integer.parseInt(pmodeString, 2);
        Optional<String> bpp = Optional.ofNullable(bitsPerPixelMap.get(bppNumber));

        if(bpp.isPresent()) {
            bitPerPixel = bpp.get();
            return true;
        }

        return false;
    }

    private boolean hasClutFlag(String hexToCheck) {
        String binaryStringOfInt = hexToBinary(hexToCheck);

        String clutFlagString = binaryStringOfInt.substring(12, 13);
        return switch (clutFlagString) {
            case "0", "1" -> {
                clutFlag = clutFlagString;
                bppAndClutFlagValue = HexParser.readULong4Byte(hexToCheck, 0);
                yield true;
            }
            default -> false;
        };
    }

    private int fetchClutSize(String hexToCheck) {
        return Integer.parseInt(hexToCheck, 16);
    }

    private boolean setClutData(String clutHexValue) {
        int clutIndex = 0;

        sizeOfClutValue = clutHexValue.substring(0, 8);

        clutIndex = clutIndex + 8;

        clutXLocation = Integer.parseInt(HexParser.readUShort2Byte(clutHexValue, clutIndex), 16);
        clutXLocationValue = clutHexValue.substring(clutIndex, clutIndex + 4);

        clutIndex = clutIndex + 4;

        clutYLocation = Integer.parseInt(HexParser.readUShort2Byte(clutHexValue, clutIndex), 16);
        clutYLocationValue = clutHexValue.substring(clutIndex, clutIndex + 4);

        clutIndex = clutIndex + 4;

        clutImageWidth = Integer.parseInt(HexParser.readUShort2Byte(clutHexValue, clutIndex), 16);
        clutImageWidthValue = clutHexValue.substring(clutIndex, clutIndex + 4);

        clutIndex = clutIndex + 4;

        clutImageHeight = Integer.parseInt(HexParser.readUShort2Byte(clutHexValue, clutIndex), 16);
        clutImageHeightValue = clutHexValue.substring(clutIndex, clutIndex + 4);

        clutIndex = clutIndex + 4;

        return fetchClutColors(clutHexValue, clutIndex);
    }

    private boolean fetchClutColors(String clutHexValue, int clutIndex) {
        numOfClutColors = clutImageWidth * clutImageHeight;
        int numOfClutColorsInByte = numOfClutColors*2;
        int lengthOfClutValue = clutHexValue.length();

        //Two ways to check the data is correct
        //Clut Header bytes are always 12 Bytes -> clutSize (4), x (2), y (2), w (2), h (2) and sizeOfClutInBytesBNum is the size of whole Clut data including the header.
        if(numOfClutColorsInByte + CLUTHEADERBYTES == sizeOfClutInBytesBNum) { //colors + Header Bytes must have the same size as the calculated clut size
            if(lengthOfClutValue-clutIndex == numOfClutColorsInByte*2) { //The Number of the Hex Values without Header must be the same, like the calculated amount of colors in Bytes.
                rgbArray = new Rgb[numOfClutColors];
                int saveIndex = 0;
                for(int colorIndex = clutIndex; colorIndex < lengthOfClutValue; colorIndex = colorIndex + 4) {
                    String colorInByte = HexParser.readUShort2Byte(clutHexValue, colorIndex);
                    String colorBin = hexToBinary(colorInByte);

                    int r = Integer.parseInt(colorBin.substring(11, 16), 2); // R: Bits 0-4
                    int g = Integer.parseInt(colorBin.substring(6, 11), 2);  // G: Bits 5-9
                    int b = Integer.parseInt(colorBin.substring(1, 6), 2);   // B: Bits 10-14
                    int stp = Integer.parseInt(colorBin.substring(0, 1), 2);  // STP: Bit 15

                    Rgb rgbValue = new Rgb(r, g, b, stp, HexParser.readUShort2Byte(colorInByte, 0));
                    rgbValue.setTransperency();

                    rgbArray[saveIndex] = rgbValue;
                    saveIndex = saveIndex + 1;
                }
                return true;
            }
        }
        return false;
    }

    private String hexToBinary(String hexValue) {
        String colorBin = String.format("%16s", Integer.toBinaryString(Integer.parseInt(hexValue, 16))).replace(' ', '0');
        return colorBin;
    }
}