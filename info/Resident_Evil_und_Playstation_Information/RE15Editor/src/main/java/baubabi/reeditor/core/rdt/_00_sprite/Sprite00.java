package baubabi.reeditor.core.rdt._00_sprite;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Sprite00 {
    int sprite;

    int rdtOffsetPositionStart = 0;

    int rdtOffsetPositionEnd = 0;

    int elementSize = 1;

    public Sprite00(int sprite) {
        this.sprite = sprite;
    }
}
