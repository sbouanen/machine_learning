/**
 * Service.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis 1.4 Apr 22, 2006 (06:55:48 PDT) WSDL2Java emitter.
 */

package com.safwenbouanen.ocr.webservice;

public interface Service extends javax.xml.rpc.Service {

/**
 * gSOAP 2.8.65 generated service definition
 */
    public java.lang.String getServiceAddress();

    public com.safwenbouanen.ocr.webservice.ServicePortType getService() throws javax.xml.rpc.ServiceException;

    public com.safwenbouanen.ocr.webservice.ServicePortType getService(java.net.URL portAddress) throws javax.xml.rpc.ServiceException;
}
