import java.io.File;
import java.awt.Color;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import java.io.IOException;
import java.util.ArrayList;

public class ImageRGB implements Runnable {
    private Thread thread;
    //EL PARAMETRO DEL HILO ES LA RUTA DEL IMAGEN QUE LE CORRESPONDE ABRIR
    private String directoryImage; 

    private static final String RED_COLOR = "RED";
    private static final String GREEN_COLOR = "GREEN";
    private static final String BLUE_COLOR = "BLUE";  

    ImageRGB(String directoryImage) {
        this.directoryImage = directoryImage;
        System.out.println("Creando hilo para la imagen " + directoryImage);
    }

    private void saturateImage(File file, String optionColor) throws IOException{
        BufferedImage img = ImageIO.read(file); //Abrimos la imagen en un buffer
        for (int y = 0; y < img.getHeight(); y++) {
            for (int x = 0; x < img.getWidth(); x++) {
                // Obtener color del pixel en la posicion xy
                int pixel = img.getRGB(x, y);
                // Lo pasamos a la clase Color
                Color color = new Color(pixel, true);
                // Separamos RGB con Alpha
                int red = color.getRed();
                int green = color.getGreen();
                int blue = color.getBlue();
                int alpha = color.getAlpha();

                //SE SATURA DEPENDIENDO DEL COLOR QUE SE ELIGIO DESDE LOS PARAMETROS
                if (optionColor == RED_COLOR) {
                    color = new Color(255, green, blue, alpha); //SATURAR EN ROJO
                    img.setRGB(x, y, color.getRGB());
                } else if (optionColor == GREEN_COLOR) {
                    color = new Color(red, 255, blue, alpha); //SATURAR EN VERDE
                    img.setRGB(x, y, color.getRGB());
                } else if (optionColor == BLUE_COLOR) {
                    color = new Color(red, green, 255, alpha); //SATURAR EN AZUL
                    img.setRGB(x, y, color.getRGB());
                }

            }
        }
        //SEPARAMOS LA RUTA DEL NOMBRE DEL ARCHIVO
        String path = file.getAbsolutePath().replaceFirst("[.][^.]+$", ""); 
        //SEPARAMOS LA EXTENCION DEL ARCHIVO DEL NOMBRE
        String fileExt = file.getName().replaceAll("^.*\\.(.*)$", "$1");

        //CREAMOS UNA COPIA DEL ARCHIVO AGREGANDO EL COLOR QUE SE SATURO AL FINAL DEL NOMBRE ORIGINAL
        // IMAGE.JPG ---> IMAGENBLUE.JPG
        File fileCopy = new File(path + optionColor+ "." + fileExt);
        //GUARDAMOS LA IMAGEN DEL BUFFER EN LA COPIA DEL ARCHIVO
        ImageIO.write(img, fileExt, fileCopy);
    }

    public void run() {
        System.out.println("Corriendo hilo para saturar la imagen " + directoryImage);
        try {
            File file = new File(directoryImage); //ABRIMOS EL ARCHIVO
            try{
                saturateImage(file, ImageRGB.RED_COLOR); //SATURAMOS LA IMAGEN EN ROJO
                saturateImage(file, ImageRGB.GREEN_COLOR); //SATURAMOS LA IMAGEN EN VERDE
                saturateImage(file, ImageRGB.BLUE_COLOR); //SATURAMOS LA IMAGEN EN AZUL
            }catch(IOException e){
                e.printStackTrace();
            }
            Thread.sleep(50); //ESPERAMOS A QUE EL HILO FINALIZE
        } catch (InterruptedException e) {
            System.out.println("El hilo con la imagen " + directoryImage + " se ha interrumpido.");
        }
        System.out.println("El hilo con la imagen " + directoryImage + " finalizó.");
    }

    public void start() {
        System.out.println("Empezando hilo de " + directoryImage);
        if (thread == null) {
            //CREAMOS EL HILO Y LE ASIGNAMOS LA RUTA DE LA IMAGEN QUE LE CORRESPONDE COMO PARAMETRO
            thread = new Thread(this, directoryImage);
            //INICAMOS EL HILO
            thread.start();
        }
    }

    public static void main(String[] args) {
        //CARPETA DONDE ESTAN GUARDADAS LAS IMAGENES
        String path = "/home/xubuntu/Desktop/redes2/practica01/imagenesRGB/img/";
        File directoryPath = new File(path); //ABRIMOS LA CARPETA
        String contents[] = directoryPath.list(); //LISTAMOS EL CONTENIDO

        for (String imageName : contents) { //OBTENEMOS LOS NOMBRES DE LAS IMAGENES
            ImageRGB thread = new ImageRGB(directoryPath.getAbsolutePath()+ "/" +imageName);
            thread.start();
        }
    }
}