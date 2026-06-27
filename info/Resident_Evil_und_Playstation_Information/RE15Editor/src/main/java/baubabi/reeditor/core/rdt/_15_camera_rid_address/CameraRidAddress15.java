package baubabi.reeditor.core.rdt._15_camera_rid_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class CameraRidAddress15 {
    int address;

    int rdtOffsetPositionStart = 36;

    int rdtOffsetPositionEnd = 39;

    int elementSize = 4;

    public CameraRidAddress15(int address) {
        this.address = address;
    }
}
