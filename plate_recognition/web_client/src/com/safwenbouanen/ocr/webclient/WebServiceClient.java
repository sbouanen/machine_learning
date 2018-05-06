package com.safwenbouanen.ocr.webclient;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;

import com.safwenbouanen.ocr.webservice.ServicePortTypeProxy;

import java.io.ByteArrayOutputStream;

public class WebServiceClient {

	public static void main(String[] args) throws IOException
	{
	    BufferedImage image = null;
	    int rows, columns;

        try 
        {
            image = ImageIO.read(new File(args[0]));
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
        }
        columns = image.getWidth(); // columns;
        rows = image.getHeight(); // rows;
        
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ImageIO.write(image, "png", baos);
        byte[] bytes = baos.toByteArray();
        int[] intArray = new int[bytes.length];

        for (int i = 0; i < bytes.length; intArray[i] = 0x000000FF & bytes[i++]);

        ServicePortTypeProxy proxy = new ServicePortTypeProxy("http://localhost:18083");
        proxy.ocr(intArray, rows, columns);
	}
}
