//gsoap ns2 service name: ocr
//gsoap ns2 service type: ocrPortType
//gsoap ns2 service port: http://localhost:18083
//gsoap ns2 service namespace: ocr

//gsoap ns2 service method-protocol: ocr SOAP
//gsoap ns2 service method-style: ocr rpc
//gsoap ns2 service method-encoding: ocr http://schemas.xmlsoap.org/soap/encoding/
//gsoap ns2 service method-action: ocr ""
int ns__ocr(int img[5000], int rows, int cols, int* status);
