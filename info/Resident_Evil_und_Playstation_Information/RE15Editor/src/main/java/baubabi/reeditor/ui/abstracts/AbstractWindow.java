package baubabi.reeditor.ui.abstracts;

import baubabi.reeditor.core.exception.ExceptionHandler;
import lombok.Getter;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.awt.*;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import static javax.swing.WindowConstants.DISPOSE_ON_CLOSE;

@Log4j2
@Getter
public abstract class AbstractWindow {
    JFrame frame = new JFrame("default");

    public AbstractWindow(int width, int height, String windowTitle, String textWindowClassName) {
        frame.setSize(width, height);
        frame.setTitle(windowTitle);
        try {
            Class textWindowClass = Class.forName(textWindowClassName);
            Method method = textWindowClass.getDeclaredMethod("getPanel");
            Object textClassInstance = textWindowClass.getDeclaredConstructor().newInstance();
            Object textPanelObject = method.invoke(textClassInstance);
            frame.getContentPane().add((Component) textPanelObject);
        } catch (ClassNotFoundException e) {
            ExceptionHandler.getInstance().logException("ClassNotFoundException at creating abstract Window", e);
        } catch (NoSuchMethodException e) {
            ExceptionHandler.getInstance().logException("NoSuchMethodException at calling the getTextPanel method in the abstract Window", e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling the invoke of getTextPanel method in the abstract Window", e);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling the invoke of getTextPane in the abstract Window", e);
        } catch (InstantiationException e) {
            ExceptionHandler.getInstance().logException("InstantiationException at creating and instance of the class in the abstract Window", e);
        }

        frame.setVisible(true);

        frame.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
    }
}
