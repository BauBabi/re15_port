package de.re15.extractors;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;

/**
 * Parser for Resident Evil 1.5 / RE2 message files (.msg).
 *
 * The MSG format uses a custom character encoding where hex values map to
 * specific characters. This parser converts raw MSG binary data into
 * human-readable text.
 *
 * Character table based on RE2 PSX format:
 * - 0x00 = Space
 * - 0x01 = .
 * - 0x0C-0x15 = 0-9
 * - 0x1A = !
 * - 0x1B = ?
 * - 0x1D-0x36 = A-Z
 * - 0x3D-0x56 = a-z
 * - 0x57-0xED = Japanese hiragana/katakana
 * - Control codes: 0x04, 0x0A (newline), 0xFC (line break), 0xFE (end)
 */
public class MSGParser {

    private static final Map<Integer, String> CHAR_TABLE = new HashMap<>();

    // Japanese hiragana characters (あ-ん)
    private static final String[] HIRAGANA = {
        "あ", "い", "う", "え", "お",  // 57-5B
        "か", "き", "く", "け", "こ",  // 5C-60
        "さ", "し", "す", "せ", "そ",  // 61-65
        "た", "ち", "つ", "て", "と",  // 66-6A
        "な", "に", "ぬ", "ね", "の",  // 6B-6F
        "は", "ひ", "ふ", "へ", "ほ",  // 70-74
        "ま", "み", "む", "め", "も",  // 75-79
        "や", "ゆ", "よ",              // 7A-7C
        "ら", "り", "る", "れ", "ろ",  // 7D-81
        "わ", "を", "ん",              // 82-84
        "ぁ", "ぃ", "ぅ", "ぇ", "ぉ",  // 85-89 (small)
        "ゃ", "ゅ", "ょ", "っ",        // 8A-8D (small)
        "が", "ぎ", "ぐ", "げ", "ご",  // 8E-92 (dakuten)
        "ざ", "じ", "ず", "ぜ", "ぞ",  // 93-97
        "だ", "ぢ", "づ", "で", "ど",  // 98-9C
        "ば", "び", "ぶ", "べ", "ぼ",  // 9D-A1
        "ぱ", "ぴ", "ぷ", "ぺ", "ぽ"   // A2-A6 (handakuten)
    };

    // Japanese katakana characters (ア-ン)
    private static final String[] KATAKANA = {
        "ア", "イ", "ウ", "エ", "オ",  // A7-AB
        "カ", "キ", "ク", "ケ", "コ",  // AC-B0
        "サ", "シ", "ス", "セ", "ソ",  // B1-B5
        "タ", "チ", "ツ", "テ", "ト",  // B6-BA
        "ナ", "ニ", "ヌ", "ネ", "ノ",  // BB-BF
        "ハ", "ヒ", "フ", "ヘ", "ホ",  // C0-C4
        "マ", "ミ", "ム", "メ", "モ",  // C5-C9
        "ヤ", "ユ", "ヨ",              // CA-CC
        "ラ", "リ", "ル", "レ", "ロ",  // CD-D1
        "ワ", "ヲ", "ン",              // D2-D4
        "ァ", "ィ", "ゥ", "ェ", "ォ",  // D5-D9 (small)
        "ャ", "ュ", "ョ", "ッ",        // DA-DD (small)
        "ガ", "ギ", "グ", "ゲ", "ゴ",  // DE-E2 (dakuten)
        "ザ", "ジ", "ズ", "ゼ", "ゾ",  // E3-E7
        "ダ", "ヂ", "ヅ", "デ", "ド",  // E8-EC
        "ー"                           // ED (chōon mark)
    };

    static {
        // Basic symbols
        CHAR_TABLE.put(0x00, " ");
        CHAR_TABLE.put(0x01, ".");
        CHAR_TABLE.put(0x02, "►");
        CHAR_TABLE.put(0x03, "┌");
        CHAR_TABLE.put(0x04, "┐");
        CHAR_TABLE.put(0x05, "(");
        CHAR_TABLE.put(0x06, ")");
        CHAR_TABLE.put(0x07, "╬");
        CHAR_TABLE.put(0x08, "╕");
        CHAR_TABLE.put(0x09, "\u201C");  // Left double quotation mark "
        CHAR_TABLE.put(0x0A, "\u201D");  // Right double quotation mark "
        CHAR_TABLE.put(0x0B, "▼");

        // Numbers 0-9
        for (int i = 0; i <= 9; i++) {
            CHAR_TABLE.put(0x0C + i, String.valueOf(i));
        }

        // Punctuation
        CHAR_TABLE.put(0x16, ":");
        CHAR_TABLE.put(0x17, "、");
        CHAR_TABLE.put(0x18, ",");
        CHAR_TABLE.put(0x19, "\"");
        CHAR_TABLE.put(0x1A, "!");
        CHAR_TABLE.put(0x1B, "?");
        CHAR_TABLE.put(0x1C, "!?");

        // Uppercase A-Z
        for (int i = 0; i < 26; i++) {
            CHAR_TABLE.put(0x1D + i, String.valueOf((char) ('A' + i)));
        }

        // More symbols
        CHAR_TABLE.put(0x37, "[");
        CHAR_TABLE.put(0x38, "/");
        CHAR_TABLE.put(0x39, "]");
        CHAR_TABLE.put(0x3A, "'");
        CHAR_TABLE.put(0x3B, "-");
        CHAR_TABLE.put(0x3C, "·");

        // Lowercase a-z
        for (int i = 0; i < 26; i++) {
            CHAR_TABLE.put(0x3D + i, String.valueOf((char) ('a' + i)));
        }

        // Japanese hiragana (0x57-0xA6)
        for (int i = 0; i < HIRAGANA.length; i++) {
            CHAR_TABLE.put(0x57 + i, HIRAGANA[i]);
        }

        // Japanese katakana (0xA7-0xED)
        for (int i = 0; i < KATAKANA.length; i++) {
            CHAR_TABLE.put(0xA7 + i, KATAKANA[i]);
        }

        // Special characters
        CHAR_TABLE.put(0xEE, "[L2]");
        CHAR_TABLE.put(0xEF, "[R2]");
        CHAR_TABLE.put(0xF0, "&");
        CHAR_TABLE.put(0xF1, "ヴ");  // vu
        CHAR_TABLE.put(0xF2, "…");   // ellipsis
        CHAR_TABLE.put(0xF3, "。");  // Japanese period
        CHAR_TABLE.put(0xF4, "「");  // opening bracket
        CHAR_TABLE.put(0xF5, "」");  // closing bracket
        CHAR_TABLE.put(0xF6, "『");  // double opening bracket
        CHAR_TABLE.put(0xF7, "』");  // double closing bracket
        CHAR_TABLE.put(0xF8, "〜");  // wave dash
        CHAR_TABLE.put(0xF9, "×");   // multiplication sign
        CHAR_TABLE.put(0xFA, "÷");   // division sign
        CHAR_TABLE.put(0xFB, "¥");   // yen symbol
    }

    /**
     * Parse a MSG file and return the decoded text.
     */
    public String parse(byte[] data) {
        if (data == null || data.length == 0) {
            return "";
        }

        StringBuilder result = new StringBuilder();
        int pos = 0;

        // Skip header bytes if present (typically starts with 04 00)
        if (data.length >= 2 && (data[0] & 0xFF) == 0x04 && (data[1] & 0xFF) == 0x00) {
            pos = 2;
        }

        while (pos < data.length) {
            int b = data[pos] & 0xFF;

            // Check for control codes
            if (b == 0x04) {
                // Header/control byte - check next byte
                if (pos + 1 < data.length) {
                    int nextByte = data[pos + 1] & 0xFF;
                    if (nextByte == 0x00) {
                        // 04 00 = Start of message
                        pos += 2;
                        continue;
                    } else if (nextByte == 0x01) {
                        // 04 01 = End of message
                        break;
                    } else if (nextByte == 0x08) {
                        // 04 08 = Color code (skip 2 bytes)
                        pos += 2;
                        continue;
                    }
                }
                pos++;
                continue;
            }

            if (b == 0x0A) {
                // Newline
                result.append("\n");
                pos++;
                continue;
            }

            if (b == 0xFC) {
                // Line break
                result.append("\n");
                pos++;
                continue;
            }

            if (b == 0xFD) {
                // Paragraph break (usually followed by 00)
                result.append("\n\n");
                pos++;
                if (pos < data.length && (data[pos] & 0xFF) == 0x00) {
                    pos++;
                }
                continue;
            }

            if (b == 0xFE) {
                // End of text
                break;
            }

            if (b == 0x1B && pos + 1 < data.length) {
                // Check if this is the terminator pattern (1B 04 01 01)
                if ((data[pos + 1] & 0xFF) == 0x04) {
                    break;
                }
            }

            // Look up character in table
            String ch = CHAR_TABLE.get(b);
            if (ch != null) {
                result.append(ch);
            } else {
                // Unknown byte - show as hex
                result.append(String.format("[%02X]", b));
            }

            pos++;
        }

        return result.toString().trim();
    }

    /**
     * Parse a MSG file from disk.
     */
    public String parseFile(Path path) throws IOException {
        byte[] data = Files.readAllBytes(path);
        return parse(data);
    }

    /**
     * Parse a MSG file and write the decoded text to a .txt file.
     */
    public void parseAndWriteText(Path msgPath, Path outputPath) throws IOException {
        String text = parseFile(msgPath);
        Files.writeString(outputPath, text);
    }

    /**
     * Test the parser with a sample MSG file.
     */
    public static void main(String[] args) throws IOException {
        MSGParser parser = new MSGParser();

        // Test with some example files
        Path[] testFiles = {
            Path.of("extracted/PSX/STAGE1/room11B0/msg/main00.msg"),
            Path.of("extracted/PSX/STAGE1/room11B0/msg/main01.msg"),
            Path.of("extracted/PSX/STAGE1/room1021/msg/main00.msg"),
            Path.of("extracted/PSX/STAGE1/room1091/msg/main00.msg")
        };

        for (Path file : testFiles) {
            if (Files.exists(file)) {
                System.out.println("=== " + file + " ===");
                String text = parser.parseFile(file);

                // Write the decoded text to a file for proper viewing
                Path outputPath = file.resolveSibling(file.getFileName().toString().replace(".msg", ".txt"));
                Files.writeString(outputPath, text, java.nio.charset.StandardCharsets.UTF_8);
                System.out.println("Written to: " + outputPath);

                // Show Unicode codepoints to verify decoding
                System.out.print("Codepoints: ");
                for (int i = 0; i < Math.min(text.length(), 30); i++) {
                    char c = text.charAt(i);
                    System.out.print(String.format("U+%04X ", (int) c));
                }
                if (text.length() > 30) {
                    System.out.print("...");
                }
                System.out.println("\n");
            }
        }
    }
}
