package baubabi.reeditor.ui.components;

import baubabi.reeditor.core.exception.ExceptionHandler;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.awt.*;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

@Log4j2
public class ObjectListSelectionNameHandler extends DefaultListCellRenderer {

    //Get a human readable name from the scd elements (the "displayName" element).
    public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus)
    {
        super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);

        Class elementClass = null;
        try {
            if(value instanceof String && value.toString().contains("@")) {
                value = value.toString().split("@")[0];
            }
            elementClass = Class.forName(value.getClass().getCanonicalName());
        } catch (ClassNotFoundException e) {
            ExceptionHandler.getInstance().logException("ClassNotFoundException Exception at getting the class for the ListCellRenderer", e);
        }
        Method method = null;
        try {
            method = elementClass.getDeclaredMethod("getDisplayName");
        } catch (NoSuchMethodException e) {
            ExceptionHandler.getInstance().logException("NoSuchMethod Exception at creating the name to display in the ObjectListSelectionNameHandler class", e);
        }
        Object elementInstance = null;
        try {
            elementInstance = elementClass.getDeclaredConstructor().newInstance();
        } catch (InstantiationException e) {
            ExceptionHandler.getInstance().logException("Instantiation Exception at creating the name to display in the ObjectListSelectionNameHandler class", e);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException Exception at creating the name to display in the ObjectListSelectionNameHandler class", e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException Exception at creating the name to display in the ObjectListSelectionNameHandler class", e);
        } catch (NoSuchMethodException e) {
            ExceptionHandler.getInstance().logException("NoSuchMethodException Exception at creating the name to display in the ObjectListSelectionNameHandler class", e);
        }
        String stringOfObject = null;
        try {
            Object elementObject = method.invoke(elementInstance);
            stringOfObject = (String) elementObject;
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException Exception at creating the name to display in the ObjectListSelectionNameHandler class", e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException Exception at creating the name to display in the ObjectListSelectionNameHandler class", e);
        }
        setText( stringOfObject );

        return this;
    }
}
