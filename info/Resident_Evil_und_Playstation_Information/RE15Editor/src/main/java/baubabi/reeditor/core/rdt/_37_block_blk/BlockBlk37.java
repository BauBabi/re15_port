package baubabi.reeditor.core.rdt._37_block_blk;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class BlockBlk37 {
    String blockBlkData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public BlockBlk37(String blockBlkData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.blockBlkData = blockBlkData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
