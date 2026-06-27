package baubabi.reeditor.ui.main.panels._02_thirdlayer._03_image_panel;

import baubabi.reeditor.core.exception.ExceptionHandler;
import baubabi.reeditor.core.rdt.RdtProcessing;
import lombok.Getter;
import lombok.extern.log4j.Log4j2;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;

@Getter
@Log4j2
public class ImagePanel {
    private final JPanel imagePanel;

    BufferedImage backgroundImage;

    int imageIndex = 0;

    Image arrowLeft = null;

    Image arrowRight = null;

    public ImagePanel() {
        //Set the upper Image for the enemy
        imagePanel = new JPanel(false);
        imagePanel.setLayout(new GridLayout(2, 0));

        JLabel imageLabel = new JLabel();
        imageLabel.setHorizontalAlignment(JLabel.CENTER);
        imageLabel.setBorder(BorderFactory.createMatteBorder(1, 1, 0, 1, Color.gray));
        imageLabel.setText("Enemy Holder - has not yet been implemented");

        imagePanel.add(imageLabel);

        //Prepare to set the room image.
        File[] imageFiles = RdtProcessing.getInstance().getCurrentProcessedBackgroundImage();
        try {
            //Configure the Room Picture on the lower side of the third panel.
            File imageFile = new File(imageFiles[imageIndex].getAbsolutePath());
            if(!imageFile.isDirectory()) {
                backgroundImage = ImageIO.read(new File(imageFiles[imageIndex].getAbsolutePath()));
                Image tmpImageScaled = backgroundImage.getScaledInstance(480, 360, Image.SCALE_SMOOTH);
                backgroundImage = new BufferedImage(480, 360, BufferedImage.TYPE_INT_ARGB);
                Graphics2D g2d = backgroundImage.createGraphics();
                g2d.drawImage(tmpImageScaled, 0, 0, null);

                ClassLoader classLoader = ClassLoader.getSystemClassLoader();
                URL url = classLoader.getResource("images/arrow-left-3099.png");
                BufferedImage BufferedArrowLeftImage = ImageIO.read(url);
                arrowLeft = BufferedArrowLeftImage.getScaledInstance(90, 60, Image.SCALE_SMOOTH);
                g2d.drawImage(arrowLeft, 0, 140, null);

                url = classLoader.getResource("images/arrow-right-3099.png");
                BufferedImage BufferedArrowRightImage = ImageIO.read(url);
                arrowRight = BufferedArrowRightImage.getScaledInstance(90, 60, Image.SCALE_SMOOTH);
                g2d.drawImage(arrowRight, 390, 140, null);
                g2d.dispose();

                JLabel backgroundLabel = new JLabel(new ImageIcon(backgroundImage));
                backgroundLabel.setBorder(BorderFactory.createMatteBorder(1, 1, 1, 1, Color.gray));

                imagePanel.add(backgroundLabel);
                imagePanel.setName("ImagePanel");

                MouseAdapter listener = new MouseAdapter() {
                    @Override
                    public void mouseClicked(MouseEvent me) {
                        //Left arrow is clicked
                        if (me.getPoint().getX() >= 28 && me.getPoint().getX() <= 84 && me.getPoint().getY() >= 187 && me.getPoint().getY() <= 271) {
                            if (getImageIndex() == 0) {
                                setImageIndex(imageFiles.length - 1);
                            } else {
                                setImageIndex(getImageIndex() - 1);
                            }

                            try {
                                backgroundImage = ImageIO.read(new File(imageFiles[getImageIndex()].getAbsolutePath()));
                                Image tmpImageScaled = backgroundImage.getScaledInstance(480, 360, Image.SCALE_SMOOTH);
                                backgroundImage = new BufferedImage(480, 360, BufferedImage.TYPE_INT_ARGB);
                                Graphics2D g2d = backgroundImage.createGraphics();
                                g2d.drawImage(tmpImageScaled, 0, 0, null);

                                g2d.drawImage(arrowLeft, 0, 140, null);
                                g2d.drawImage(arrowRight, 390, 140, null);

                                backgroundLabel.setIcon(new ImageIcon(backgroundImage));
                            } catch (IOException e) {
                                ExceptionHandler.getInstance().logException("IOException reset Background Image", e);
                            }
                        }

                        //Right arrow is clicked
                        else if (me.getPoint().getX() >= 389 && me.getPoint().getX() <= 443 && me.getPoint().getY() >= 187 && me.getPoint().getY() <= 271) {
                            if (getImageIndex() == imageFiles.length - 1) {
                                setImageIndex(0);
                            } else {
                                setImageIndex(getImageIndex() + 1);
                            }

                            try {
                                backgroundImage = ImageIO.read(new File(imageFiles[getImageIndex()].getAbsolutePath()));
                                Image tmpImageScaled = backgroundImage.getScaledInstance(480, 360, Image.SCALE_SMOOTH);
                                backgroundImage = new BufferedImage(480, 360, BufferedImage.TYPE_INT_ARGB);
                                Graphics2D g2d = backgroundImage.createGraphics();
                                g2d.drawImage(tmpImageScaled, 0, 0, null);

                                g2d.drawImage(arrowLeft, 0, 140, null);
                                g2d.drawImage(arrowRight, 390, 140, null);

                                backgroundLabel.setIcon(new ImageIcon(backgroundImage));
                            } catch (IOException e) {
                                ExceptionHandler.getInstance().logException("IOException reset Background Image", e);
                            }
                        }
                    }
                };

                backgroundLabel.addMouseListener(listener);
            }
        } catch (IOException e) {
            ExceptionHandler.getInstance().logException("IOException loading Level Image", e);
            e.printStackTrace();
        }
    }

    private void setImageIndex(int index) {
        this.imageIndex = index;
    }

    public void prepareEnemyImage(Object enemy) {

    }
}