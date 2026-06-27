package baubabi.reeditor.core.general.utils;

import baubabi.reeditor.core.exception.ExceptionHandler;

import javax.swing.*;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;

public class ReflectionUtilities {
    private static ReflectionUtilities INSTANCE;

    public static ReflectionUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new ReflectionUtilities();
        }

        return INSTANCE;
    }

    public int executeMethodForClassObjectAndReturnInt(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = getDeclaredMethod(elementForGettingCorrespondingOffsetFor, methodToExecute, callerClass);
        int resultOfMethodInvocation = 0;

        try {
            resultOfMethodInvocation = (int) method.invoke(elementForGettingCorrespondingOffsetFor);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling " + methodToExecute + " at " + callerClass, e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return resultOfMethodInvocation;
    }

    public String executeMethodForClassObjectAndReturnString(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = getDeclaredMethod(elementForGettingCorrespondingOffsetFor, methodToExecute, callerClass);
        String resultOfMethodInvocation = "";

        try {
            resultOfMethodInvocation = (String) method.invoke(elementForGettingCorrespondingOffsetFor);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling " + methodToExecute + " at " + callerClass, e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return resultOfMethodInvocation;
    }

    public Object executeMethodForClassObjectAndReturnObject(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = getDeclaredMethod(elementForGettingCorrespondingOffsetFor, methodToExecute, callerClass);
        Object resultOfMethodInvocation = 0;

        try {
            resultOfMethodInvocation = method.invoke(elementForGettingCorrespondingOffsetFor);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling " + methodToExecute + " at " + callerClass, e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return resultOfMethodInvocation;
    }

    public List<Object> executeMethodForClassObjectAndReturnObjectList(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = getDeclaredMethod(elementForGettingCorrespondingOffsetFor, methodToExecute, callerClass);
        List<Object> resultOfMethodInvocation = null;

        try {
            resultOfMethodInvocation = (List<Object>) method.invoke(elementForGettingCorrespondingOffsetFor);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling " + methodToExecute + " at " + callerClass, e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return resultOfMethodInvocation;
    }

    public JPanel executeMethodForClassObjectAndReturnJPanel(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = getDeclaredMethod(elementForGettingCorrespondingOffsetFor, methodToExecute, callerClass);
        JPanel resultOfMethodInvocation = null;

        try {
            resultOfMethodInvocation = (JPanel) method.invoke(elementForGettingCorrespondingOffsetFor);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling " + methodToExecute + " at " + callerClass, e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return resultOfMethodInvocation;
    }

    public void executeMethodForClassObjectWithIntegerParameter(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass, int parameter) {
        Method method = getDeclaredMethodWithParameterForInteger(elementForGettingCorrespondingOffsetFor, methodToExecute, callerClass);

        try {
            method.invoke(elementForGettingCorrespondingOffsetFor, parameter);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling " + methodToExecute + " at " + callerClass, e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling " + methodToExecute + " at " + callerClass, e);
        }
    }

    public void executeMethodForClassObjectWithStringParameter(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass, String parameter) {
        Method method = getDeclaredMethodWithParameterForString(elementForGettingCorrespondingOffsetFor, methodToExecute, callerClass);

        try {
            method.invoke(elementForGettingCorrespondingOffsetFor, parameter);
        } catch (IllegalAccessException e) {
            ExceptionHandler.getInstance().logException("IllegalAccessException at calling " + methodToExecute + " at " + callerClass, e);
        } catch (InvocationTargetException e) {
            ExceptionHandler.getInstance().logException("InvocationTargetException at calling " + methodToExecute + " at " + callerClass, e);
        }
    }

    private Method getDeclaredMethod(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = null;
        try {
            method = elementForGettingCorrespondingOffsetFor.getClass().getDeclaredMethod(methodToExecute);
        } catch (NoSuchMethodException e) {
            ExceptionHandler.getInstance().logException("NoSuchMethodException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return method;
    }

    private Method getDeclaredMethodWithParameterForInteger(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = null;
        try {
            method = elementForGettingCorrespondingOffsetFor.getClass().getDeclaredMethod(methodToExecute, int.class);
        } catch (NoSuchMethodException e) {
            ExceptionHandler.getInstance().logException("NoSuchMethodException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return method;
    }

    private Method getDeclaredMethodWithParameterForString(Object elementForGettingCorrespondingOffsetFor, String methodToExecute, String callerClass) {
        Method method = null;
        try {
            method = elementForGettingCorrespondingOffsetFor.getClass().getDeclaredMethod(methodToExecute, String.class);
        } catch (NoSuchMethodException e) {
            ExceptionHandler.getInstance().logException("NoSuchMethodException at calling " + methodToExecute + " at " + callerClass, e);
        }

        return method;
    }


}
