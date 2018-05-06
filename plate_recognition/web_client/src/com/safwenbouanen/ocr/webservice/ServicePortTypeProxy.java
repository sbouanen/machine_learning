package com.safwenbouanen.ocr.webservice;

public class ServicePortTypeProxy implements com.safwenbouanen.ocr.webservice.ServicePortType {
  private String _endpoint = null;
  private com.safwenbouanen.ocr.webservice.ServicePortType servicePortType = null;
  
  public ServicePortTypeProxy() {
    _initServicePortTypeProxy();
  }
  
  public ServicePortTypeProxy(String endpoint) {
    _endpoint = endpoint;
    _initServicePortTypeProxy();
  }
  
  private void _initServicePortTypeProxy() {
    try {
      servicePortType = (new com.safwenbouanen.ocr.webservice.ServiceLocator()).getService();
      if (servicePortType != null) {
        if (_endpoint != null)
          ((javax.xml.rpc.Stub)servicePortType)._setProperty("javax.xml.rpc.service.endpoint.address", _endpoint);
        else
          _endpoint = (String)((javax.xml.rpc.Stub)servicePortType)._getProperty("javax.xml.rpc.service.endpoint.address");
      }
      
    }
    catch (javax.xml.rpc.ServiceException serviceException) {}
  }
  
  public String getEndpoint() {
    return _endpoint;
  }
  
  public void setEndpoint(String endpoint) {
    _endpoint = endpoint;
    if (servicePortType != null)
      ((javax.xml.rpc.Stub)servicePortType)._setProperty("javax.xml.rpc.service.endpoint.address", _endpoint);
    
  }
  
  public com.safwenbouanen.ocr.webservice.ServicePortType getServicePortType() {
    if (servicePortType == null)
      _initServicePortTypeProxy();
    return servicePortType;
  }
  
  public java.lang.Integer ocr(int[] img, int rows, int cols) throws java.rmi.RemoteException{
    if (servicePortType == null)
      _initServicePortTypeProxy();
    return servicePortType.ocr(img, rows, cols);
  }
  
  
}