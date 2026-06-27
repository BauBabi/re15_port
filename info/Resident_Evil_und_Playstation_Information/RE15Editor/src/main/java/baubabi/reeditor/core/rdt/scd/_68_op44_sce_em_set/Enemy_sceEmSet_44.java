package baubabi.reeditor.core.rdt.scd._68_op44_sce_em_set;

import lombok.Getter;

@Getter
public class Enemy_sceEmSet_44 {
    //20 byte

    /*
        Entity (enemy, other character)
    */

    //1 byte
    private final String OPCODE = "44";

    //1 byte - The counter of the enemy
    private final String[] enemyCounter = new String[1];

    //1 byte - The one from the enemy fileName in form of hex value
    private final String[] enemyType = new String[1];

    //1 Byte - Also could be something different...
    private final String[] enemyState = new String[1];

    //1 Byte - Also could be something different...
    private final String[] enemyUnknown = new String[1];

    //1 Byte - Also could be something different...
    private final String[] enemySoundBank = new String[1];

    //1 Byte - Also could be something different...
    private final String[] enemyTexture = new String[1];

    //1 Byte - Unique killing id for each enemy
    private final String[] enemyKilledId = new String[1];

    //2 Byte - Enemy X Position
    private final String[] enemyPositionX = new String[2];

    //2 Byte - Enemy Y Position
    private final String[] enemyPositionY = new String[2];

    //2 Byte - Enemy Z Position
    private final String[] enemyPositionZ = new String[2];

    //2 Byte - Enemy X Direction
    private final String[] enemyDirectionX = new String[2];

    //2 Byte - Enemy Y Direction
    private final String[] enemyDirectionY = new String[2];

    //2 Byte - Enemy Z Direction
    private final String[] enemyDirectionZ = new String[2];

    private final int size = 20;

    private final String displayName = OPCODE + " - set enemy";

    public Enemy_sceEmSet_44() {
        this.enemyCounter[0] = "00";
        this.enemyType[0] = "00";
        this.enemyState[0] = "00";
        this.enemyUnknown[0] = "00";
        this.enemySoundBank[0] = "00";
        this.enemyTexture[0] = "00";
        this.enemyKilledId[0] = "00";
        this.enemyPositionX[0] = "00";
        this.enemyPositionX[1] = "00";
        this.enemyPositionY[0] = "00";
        this.enemyPositionY[1] = "00";
        this.enemyPositionZ[0] = "00";
        this.enemyPositionZ[1] = "00";
        this.enemyDirectionX[0] = "00";
        this.enemyDirectionX[1] = "00";
        this.enemyDirectionY[0] = "00";
        this.enemyDirectionY[1] = "00";
        this.enemyDirectionZ[0] = "00";
        this.enemyDirectionZ[1] = "00";
    }

    public Enemy_sceEmSet_44(String enemyValue) {
        if(enemyValue != null) {
            this.enemyCounter[0] = enemyValue.substring(2,4);
            this.enemyType[0] = enemyValue.substring(4,6);
            this.enemyState[0] = enemyValue.substring(6,8);
            this.enemyUnknown[0] = enemyValue.substring(8,10);
            this.enemySoundBank[0] = enemyValue.substring(10,12);
            this.enemyTexture[0] = enemyValue.substring(12,14);
            this.enemyKilledId[0] = enemyValue.substring(14,16);
            this.enemyPositionX[0] = enemyValue.substring(16,18);
            this.enemyPositionX[1] = enemyValue.substring(18,20);
            this.enemyPositionY[0] = enemyValue.substring(20,22);
            this.enemyPositionY[1] = enemyValue.substring(22,24);
            this.enemyPositionZ[0] = enemyValue.substring(24,26);
            this.enemyPositionZ[1] = enemyValue.substring(26,28);
            this.enemyDirectionX[0] = enemyValue.substring(28,30);
            this.enemyDirectionX[1] = enemyValue.substring(30,32);
            this.enemyDirectionY[0] = enemyValue.substring(32,34);
            this.enemyDirectionY[1] = enemyValue.substring(34,36);
            this.enemyDirectionZ[0] = enemyValue.substring(36,38);
            this.enemyDirectionZ[1] = enemyValue.substring(38,40);
        }
    }

    public int getEnemyCounterDecimal() {
        return twoComplement(getEnemyCounter()[0]);
    }

    public int getEnemyStateDecimal() {
        return twoComplement(getEnemyState()[0]);
    }

    public int getEnemyUnknownDecimal() { return twoComplement(getEnemyUnknown()[0]); }

    public int getEnemySoundBankDecimal() {
        return twoComplement(getEnemySoundBank()[0]);
    }

    public int getEnemyTextureDecimal() { return twoComplement(getEnemyTexture()[0]); }

    public int getEnemyKilledIdDecimal() {
        return twoComplement(getEnemyKilledId()[0]);
    }

    public int getEnemyPositionXDecimal() {
        return twoComplement(getEnemyPositionX()[0] + getEnemyPositionX()[1]);
    }

    public int getEnemyPositionYDecimal() {
        return twoComplement(getEnemyPositionY()[0] + getEnemyPositionY()[1]);
    }

    public int getEnemyPositionZDecimal() {
        return twoComplement(getEnemyPositionZ()[0] + getEnemyPositionZ()[1]);
    }

    public int getEnemyDirectionXDecimal() {
        return twoComplement(getEnemyDirectionX()[0] + getEnemyDirectionX()[1]);
    }

    public int getEnemyDirectionYDecimal() {
        return twoComplement(getEnemyDirectionY()[0] + getEnemyDirectionY()[1]);
    }

    public int getEnemyDirectionZDecimal() {
        return twoComplement(getEnemyDirectionZ()[0] + getEnemyDirectionZ()[1]);
    }

    public void setEnemyType(String enemyType) { this.enemyType[0] = enemyType; }

    public void setEnemyCounter(String enemyCounterDecimal) { this.enemyCounter[0] = getHexStringForDecimalForOneByteVariables(enemyCounterDecimal); }

    public void setEnemyState(String enemyStateDecimal) { this.enemyState[0] = getHexStringForDecimalForOneByteVariables(enemyStateDecimal); }

    public void setEnemyUnknown(String enemyUnknownDecimal) { this.enemyUnknown[0] = getHexStringForDecimalForOneByteVariables(enemyUnknownDecimal); }

    public void setEnemySoundBank(String enemySoundBankDecimal) { this.enemySoundBank[0] = getHexStringForDecimalForOneByteVariables(enemySoundBankDecimal); }

    public void setEnemyTexture(String enemyTextureDecimal) { this.enemyTexture[0] = getHexStringForDecimalForOneByteVariables(enemyTextureDecimal); }

    public void setEnemyKilledId(String enemyKilledIdDecimal) { this.enemyKilledId[0] = getHexStringForDecimalForOneByteVariables(enemyKilledIdDecimal); }

    public void setEnemyPositionX(String enemyPositionXDecimal) { setValueToEachByte(getHexStringForDecimalForTwoByteVariables(enemyPositionXDecimal), this.enemyPositionX); }

    public void setEnemyPositionY(String enemyPositionYDecimal) { setValueToEachByte(getHexStringForDecimalForTwoByteVariables(enemyPositionYDecimal), this.enemyPositionY); }

    public void setEnemyPositionZ(String enemyPositionZDecimal) { setValueToEachByte(getHexStringForDecimalForTwoByteVariables(enemyPositionZDecimal), this.enemyPositionZ); }

    public void setEnemyDirectionX(String enemyDirectionXDecimal) { setValueToEachByte(getHexStringForDecimalForTwoByteVariables(enemyDirectionXDecimal), this.enemyDirectionX); }

    public void setEnemyDirectionY(String enemyDirectionYDecimal) { setValueToEachByte(getHexStringForDecimalForTwoByteVariables(enemyDirectionYDecimal), this.enemyDirectionY); }

    public void setEnemyDirectionZ(String enemyDirectionZDecimal) { setValueToEachByte(getHexStringForDecimalForTwoByteVariables(enemyDirectionZDecimal), this.enemyDirectionZ); }

    public static Integer twoComplement(String hex) {
        Integer decimalNumberPositive = Integer.valueOf(hex, 16);
        if(decimalNumberPositive > 32767) {
            return decimalNumberPositive - 65536;
        }
        else {
            return decimalNumberPositive;
        }
    }

    public String getHexStringForDecimalForOneByteVariables(String DecimalValueForParsing) {
        String resultString;
        if(Integer.parseInt(DecimalValueForParsing) < 0) {
            resultString = String.format("%02X", Integer.parseInt(DecimalValueForParsing));
            resultString = resultString.substring(4);
        }
        else {
            resultString = String.format("%02X", Integer.parseInt(DecimalValueForParsing));
        }

        return resultString;
    }

    public String getHexStringForDecimalForTwoByteVariables(String DecimalValueForParsing) {
        String resultString = "";
        if(Integer.parseInt(DecimalValueForParsing) < 0) {
            resultString = String.format("%02X", Integer.parseInt(DecimalValueForParsing));
            resultString = resultString.substring(4);
        }
        else {
            String hex = String.format("%02X", Integer.parseInt(DecimalValueForParsing));
            if(hex.length() < 4) {
                if(hex.length() == 3) {
                    resultString = "0";
                }
                else if(hex.length() == 2) {
                    resultString = "00";
                }
                else if(hex.length() == 1) {
                    resultString = "000";
                }
                else {
                    resultString = "00";
                }
            }
            resultString = resultString + hex;
        }

        return resultString;
    }

    public void setValueToEachByte(String value, String[] StringArrayToSetValue) {
        for(int i = 0; i < value.length(); i = i + 2) {
            StringArrayToSetValue[i/2] = value.substring(i, i+2);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + enemyCounter[0];
        newScdDataByteString = newScdDataByteString + enemyType[0];
        newScdDataByteString = newScdDataByteString + enemyState[0];
        newScdDataByteString = newScdDataByteString + enemyUnknown[0];
        newScdDataByteString = newScdDataByteString + enemySoundBank[0];
        newScdDataByteString = newScdDataByteString + enemyTexture[0];
        newScdDataByteString = newScdDataByteString + enemyKilledId[0];
        newScdDataByteString = newScdDataByteString + enemyPositionX[0];
        newScdDataByteString = newScdDataByteString + enemyPositionX[1];
        newScdDataByteString = newScdDataByteString + enemyPositionY[0];
        newScdDataByteString = newScdDataByteString + enemyPositionY[1];
        newScdDataByteString = newScdDataByteString + enemyPositionZ[0];
        newScdDataByteString = newScdDataByteString + enemyPositionZ[1];
        newScdDataByteString = newScdDataByteString + enemyDirectionX[0];
        newScdDataByteString = newScdDataByteString + enemyDirectionX[1];
        newScdDataByteString = newScdDataByteString + enemyDirectionY[0];
        newScdDataByteString = newScdDataByteString + enemyDirectionY[1];
        newScdDataByteString = newScdDataByteString + enemyDirectionZ[0];
        newScdDataByteString = newScdDataByteString + enemyDirectionZ[1];

        return newScdDataByteString;
    }
}
