package baubabi.reeditor.ui.components.backup;

import java.awt.*;

public class ImagePanel extends Panel {
    Image image;

    public ImagePanel()
    {
        loadImage();
        setBackground(Color.black);
    }

    public void paint(Graphics g)
    {
        super.paint(g);
        int w = getWidth();
        int h = getHeight();
        int imageWidth = image.getWidth(this);
        int imageHeight = image.getHeight(this);
        int x = (w - imageWidth)/2;
        int y = (h - imageHeight)/2;
        g.drawImage(image, x, y, this);
    }

    /**
     * Let clients know our size requirements
     */
    public Dimension getPreferredSize()
    {
        return new Dimension(image.getWidth(this), image.getHeight(this));
    }

    private void loadImage()
    {
        String fileName = "src/test/resources/3DObjects/bingo.jpg";
        Toolkit toolkit = Toolkit.getDefaultToolkit();
        MediaTracker tracker = new MediaTracker(this);
        image = toolkit.createImage(fileName);
        tracker.addImage(image, 0);
        try
        {
            tracker.waitForID(0);
        }
        catch(InterruptedException ie)
        {
            System.out.println("interrupt: " + ie.getMessage());
        }
    }
}
