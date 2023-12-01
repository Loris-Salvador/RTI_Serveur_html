package Java.handler;

import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpExchange;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class StaticFileHandler implements HttpHandler {
    @Override
    public void handle(HttpExchange exchange) throws IOException {
        String requestPath = exchange.getRequestURI().getPath();
        String filePathStr = "src/Html" + requestPath;

        String requestPathimg = requestPath.substring(1);

        requestPathimg = requestPathimg.substring(0, requestPathimg.length() - 3) + "png";


        Path filePath = Paths.get(filePathStr);
        Path filePathimg = Paths.get(requestPathimg);


        if (Files.exists(filePath) || Files.exists(filePathimg)) {
            if(!Files.exists(filePath))
                filePath = filePathimg;

            String contentType = getContentType(filePath);
            exchange.getResponseHeaders().add("Content-Type", contentType);
            exchange.sendResponseHeaders(200, Files.size(filePath));

            try (InputStream is = Files.newInputStream(filePath);
                 OutputStream os = exchange.getResponseBody()) {
                byte[] buffer = new byte[4096];
                int bytesRead;
                while ((bytesRead = is.read(buffer)) != -1) {
                    os.write(buffer, 0, bytesRead);
                }
            }
        } else {
            System.out.println(filePathStr);
            System.out.println(filePathimg);
            // Gérer le cas où le fichier n'existe pas
            exchange.sendResponseHeaders(404, -1);
        }
    }

    private String getContentType(Path filePath) throws IOException {
        String contentType = Files.probeContentType(filePath);
        if (contentType == null) {
            // Si le type de contenu n'est pas détecté, utilisez un type par défaut
            return "application/octet-stream";
        } else {
            return contentType;
        }
    }

}
