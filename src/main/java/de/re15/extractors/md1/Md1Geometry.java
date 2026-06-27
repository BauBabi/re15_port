package de.re15.extractors.md1;

/**
 * Utility methods that convert PSX-space MD1 coordinates into glTF/OBJ friendly values.
 */
public final class Md1Geometry {

    private Md1Geometry() {
    }

    public static double[] mapVertex(MD1File.Vertex vertex) {
        double x = mapVertexComponent(vertex.z);
        double y = mapVertexComponent(vertex.y);
        double z = mapVertexComponent(vertex.x);
        return new double[]{x, y, z};
    }

    private static double mapVertexComponent(short value) {
        /* BO-round 2026-05-29: removed the non-canonical `if (v>0) v-=0.02` bias
         * hack (asymmetric mesh distortion). The /50.0 is the intentional DCC
         * export scale (reference-only OBJ/glTF; the C engine reads raw s16). */
        return -value / 50.0;
    }

    public static double[] mapNormal(MD1File.Normal normal) {
        double nx = mapNormalComponent(-normal.z, NormalAxis.X);
        double ny = mapNormalComponent(-normal.y, NormalAxis.Y);
        double nz = mapNormalComponent(-normal.x, NormalAxis.Z);
        return new double[]{nx, ny, nz};
    }

    private static double mapNormalComponent(int raw, NormalAxis axis) {
        /* BO-round 2026-05-29: MD1 normals are Q12 UNIT vectors (4096 = 1.0).
         * The canonical decode is raw/4096.0 exact. The old empirically-fitted
         * NormalLookup + 0.9575×+offset produced non-unit normals (len≈0.957),
         * dimming lighting in DCC viewers. (lib_md1 + DOCUMENTATION.md; the C
         * engine md1_common.c already uses the raw Q12 unit directly.) */
        return raw / 4096.0;
    }

    public static double[] mapUv(int u, int v, int pageValue, Md1TextureAtlas.TextureAtlasDimensions atlas) {
        int page = pageValue & 0xFFFF;
        int baseX = computePageBaseX(page);
        int baseY = computePageBaseY(page);
        int texX = baseX + (u & 0xFF);
        int texY = baseY + (v & 0xFF);
        int clampedX = atlas.width() > 0 ? Math.max(0, Math.min(texX, atlas.width() - 1)) : texX;
        double mappedU = atlas.width() > 0 ? clampedX / (double) atlas.width() : 0.0;
        int clampedY = atlas.height() > 1 ? Math.max(0, Math.min(texY, atlas.height() - 1)) : texY;
        double mappedV = atlas.height() > 1 ? (atlas.height() - 1 - clampedY) / (double) (atlas.height() - 1) : 0.0;
        return new double[]{clamp(mappedU), clamp(mappedV)};
    }

    public static int computePageBaseX(int page) {
        int colorMode = (page >> 5) & 0x3;
        int pageX = page & 0xF;
        int base = pageX * 64;
        return switch (colorMode) {
            case 0 -> base * 2;
            case 1 -> base;
            case 2 -> base / 2;
            default -> base;
        };
    }

    public static int computePageBaseY(int page) {
        int pageY = (page >> 4) & 0x3;
        return pageY * 256;
    }

    public static double clamp(double value) {
        if (value < 0.0) {
            return 0.0;
        }
        if (value > 1.0) {
            return 1.0;
        }
        return value;
    }

    private enum NormalAxis { X, Y, Z }

    @Override
    public String toString() {
        return "Md1Geometry{}";
    }
}
