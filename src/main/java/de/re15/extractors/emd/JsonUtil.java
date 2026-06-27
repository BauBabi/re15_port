package de.re15.extractors.emd;

import java.util.Iterator;
import java.util.List;
import java.util.Map;

final class JsonUtil {
    private JsonUtil() {}

    static String toJson(Object value) {
        StringBuilder sb = new StringBuilder();
        appendValue(sb, value);
        return sb.toString();
    }

    @SuppressWarnings("unchecked")
    private static void appendValue(StringBuilder sb, Object value) {
        if (value == null) {
            sb.append("null");
        } else if (value instanceof String s) {
            sb.append('"').append(escape(s)).append('"');
        } else if (value instanceof Number || value instanceof Boolean) {
            sb.append(value.toString());
        } else if (value instanceof Map) {
            Map<?, ?> map = (Map<?, ?>) value;
            sb.append('{');
            Iterator<? extends Map.Entry<?, ?>> it = map.entrySet().iterator();
            boolean first = true;
            while (it.hasNext()) {
                Map.Entry<?, ?> entry = it.next();
                if (!first) sb.append(',');
                first = false;
                sb.append('"').append(escape(String.valueOf(entry.getKey()))).append('"').append(':');
                appendValue(sb, entry.getValue());
            }
            sb.append('}');
        } else if (value instanceof List) {
            List<?> list = (List<?>) value;
            sb.append('[');
            for (int i = 0; i < list.size(); i++) {
                if (i > 0) sb.append(',');
                appendValue(sb, list.get(i));
            }
            sb.append(']');
        } else if (value.getClass().isArray()) {
            sb.append('[');
            int length = java.lang.reflect.Array.getLength(value);
            for (int i = 0; i < length; i++) {
                if (i > 0) sb.append(',');
                appendValue(sb, java.lang.reflect.Array.get(value, i));
            }
            sb.append(']');
        } else {
            sb.append('"').append(escape(value.toString())).append('"');
        }
    }

    private static String escape(String s) {
        return s.replace("\\", "\\\\").replace("\"", "\\\"");
    }
}
