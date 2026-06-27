package baubabi.reeditor.core.rdt._20_block_blk_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class BlockBlkAddress20 {
    int address;

    int rdtOffsetPositionStart = 56;

    int rdtOffsetPositionEnd = 59;

    int elementSize = 4;

    public BlockBlkAddress20(int address) {
        this.address = address;
    }
}
