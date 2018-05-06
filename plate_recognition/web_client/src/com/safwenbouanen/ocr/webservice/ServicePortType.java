/**
 * ServicePortType.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis 1.4 Apr 22, 2006 (06:55:48 PDT) WSDL2Java emitter.
 */

package com.safwenbouanen.ocr.webservice;

public interface ServicePortType extends java.rmi.Remote {

    /**
     * Service definition of function ns__ocr
     */
    public java.lang.Integer ocr(int[] img, int rows, int cols) throws java.rmi.RemoteException;
}
