import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class MD1HexPatternScanner {
    public static void main(String[] args) throws Exception {
        if (args.length < 1) {
            System.out.println("Usage: java MD1HexPatternScanner <md1-file>");
            return;
        }
        String md1Path = args[0];
        byte[] data = new byte[16384];
        try (FileInputStream fis = new FileInputStream(md1Path)) {
            fis.read(data);
        }
        System.out.println("[SHORTS] Die ersten 100 Werte als short (little endian):");
        for (int i = 0; i < 100 && i*2+1 < data.length; i++) {
            short val = ByteBuffer.wrap(data, i*2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            System.out.printf("%d: %d\n", i*2, val);
        }
        System.out.println("\n[FLOATS] Die ersten 50 Werte als float (little endian):");
        for (int i = 0; i < 50 && i*4+3 < data.length; i++) {
            float val = ByteBuffer.wrap(data, i*4, 4).order(ByteOrder.LITTLE_ENDIAN).getFloat();
            System.out.printf("%d: %.4f\n", i*4, val);
        }
        System.out.println("\n[BLOCKS] 20 Blöcke à 8 Bytes als Shorts (3 Koordinaten + Padding):");
        for (int i = 0; i < 20; i++) {
            int off = i*8;
            if (off+7 >= data.length) break;
            short x = ByteBuffer.wrap(data, off, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short y = ByteBuffer.wrap(data, off+2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short z = ByteBuffer.wrap(data, off+4, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short pad = ByteBuffer.wrap(data, off+6, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            System.out.printf("%d: x=%d y=%d z=%d pad=%d\n", off, x, y, z, pad);
        }
    }
}

