package baubabi.reeditor.core.rdt._30_camera_rid;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class CameraRid30 {
    String cameraRidData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public CameraRid30(String cameraRidData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.cameraRidData = cameraRidData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
