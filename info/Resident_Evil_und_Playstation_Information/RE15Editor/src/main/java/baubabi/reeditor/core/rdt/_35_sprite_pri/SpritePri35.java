package baubabi.reeditor.core.rdt._35_sprite_pri;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class SpritePri35 {
    String spritePriData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public SpritePri35(String spritePriData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.spritePriData = spritePriData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
