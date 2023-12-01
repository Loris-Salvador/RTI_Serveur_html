package Java.core.util;

import Java.core.model.Article;
import com.sun.net.httpserver.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class Fonction_Util {
    public static void sendResponse(HttpExchange exchange, int statusCode, String response) throws IOException {
        System.out.println("Envoi de la réponse (" + statusCode + ") : --" + response + "--");
        byte[] responseData = response.getBytes();
        exchange.sendResponseHeaders(statusCode, responseData.length);

        try (OutputStream os = exchange.getResponseBody()) {
            int chunkSize = 1024; // Taille du chunk, vous pouvez ajuster selon vos besoins
            int offset = 0;

            while (offset < responseData.length) {
                int length = Math.min(responseData.length - offset, chunkSize);
                os.write(responseData, offset, length);
                offset += length;
            }
        } catch (IOException e) {
            // Gérer l'exception liée à l'écriture dans le flux de sortie
            e.printStackTrace();
        } finally {
            exchange.close(); // Assurez-vous de fermer correctement la réponse
        }
    }


    public static void sendHtmlFileResponse(HttpExchange exchange, String filePath) throws IOException {
        exchange.getResponseHeaders().add("Content-Type", "text/html; charset=UTF-8");


        try (InputStream is = Fonction_Util.class.getResourceAsStream(filePath)) {
            if (is != null) {
                byte[] bytes = is.readAllBytes();
                exchange.sendResponseHeaders(200, bytes.length);
                try (OutputStream os = exchange.getResponseBody()) {
                    os.write(bytes);
                }
            } else {
                sendResponse(exchange, 404, "Not Found");
            }
        }
    }
    public static String readRequestBody(HttpExchange exchange) throws IOException
    {
        BufferedReader reader = new BufferedReader(new InputStreamReader(exchange.getRequestBody()));
        StringBuilder requestBody = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null)
        {
            requestBody.append(line);
        }
        reader.close();
        return requestBody.toString();
    }
    public static Map<String, String> parseQueryParams(String query)
    {
        Map<String, String> queryParams = new HashMap<>();
        if (query != null)
        {
            String[] params = query.split("&");
            for (String param : params)
            {
                String[] keyValue = param.split("=");
                if (keyValue.length == 2)
                {
                    queryParams.put(keyValue[0], keyValue[1]);
                }
            }
        }
        return queryParams;
    }


    public static Article convertJsonToArticle(String json) {
        String[] parts = json.replaceAll("[{}\"]", "").split(",");
        int id = Integer.parseInt(parts[0].split(":")[1]);
        String intitule = parts[1].split(":")[1];
        int quantite = Integer.parseInt(parts[2].split(":")[1]);
        float prixUnitaire = Float.parseFloat(parts[3].split(":")[1]);
        String image = parts[4].split(":")[1];

        return new Article(id, intitule, quantite, prixUnitaire, image);
    }

    public static String convertArticlesToJson(ArrayList<Article> articles) {
        // Convertir la liste des articles en format JSON
        StringBuilder json = new StringBuilder("[");
        for (int i = 0; i < articles.size(); i++) {
            Article article = articles.get(i);
            json.append("{\"id\": ").append(article.id())
                    .append(", \"intitule\":\"").append(article.intitule())
                    .append("\", \"quantite\":").append(article.quantite())
                    .append(", \"prixUnitaire\":").append(article.prixUnitaire())
                    .append(", \"image\":\"").append(article.image()).append("\"}");
            if (i < articles.size() - 1) json.append(",");
        }
        json.append("]");
        return json.toString();
    }
}
