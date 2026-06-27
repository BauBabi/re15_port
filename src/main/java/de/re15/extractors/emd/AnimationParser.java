package de.re15.extractors.emd;

import de.re15.extractors.util.LittleEndianDataView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

final class AnimationParser {
    private AnimationParser() {}

    static Optional<AnimationMetadata> parseAnimation(byte[] data, int offset) {
        if (offset <= 0 || offset >= data.length) {
            return Optional.empty();
        }
        LittleEndianDataView view = new LittleEndianDataView(data);
        int count0 = view.readUnsignedShort(offset);
        int offset0 = view.readUnsignedShort(offset + 2);
        if (offset0 == 0) {
            return Optional.empty();
        }
        int clipCount = offset0 / 4;
        List<AnimationClip> clips = new ArrayList<>(clipCount);
        clips.add(new AnimationClip("clip_0", 0, count0));
        int frameCount = count0;
        int nextOffset = count0;
        int cur = offset + 4;
        for (int index = 1; index < clipCount; index++) {
            int countI = view.readUnsignedShort(cur);
            cur += 4; // skip count + unknown
            clips.add(new AnimationClip("clip_" + index, nextOffset, countI));
            frameCount += countI;
            nextOffset += countI;
        }
        int framesOffset = offset + offset0;
        int[] frames = new int[frameCount];
        ByteBuffer buf = ByteBuffer.wrap(data, framesOffset, frameCount * 4).order(ByteOrder.LITTLE_ENDIAN);
        int lastFrameIndex = 0;
        for (int i = 0; i < frameCount; i++) {
            int value = buf.getInt();
            frames[i] = value;
            int keyIndex = value & 0xFFF;
            if (keyIndex > lastFrameIndex) {
                lastFrameIndex = keyIndex;
            }
        }
        lastFrameIndex += 1;
        AnimationMetadata meta = new AnimationMetadata(frames, clips.toArray(AnimationClip[]::new), lastFrameIndex);
        return Optional.of(meta);
    }
}
