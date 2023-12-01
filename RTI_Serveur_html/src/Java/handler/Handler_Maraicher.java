package Java.handler;

import Java.core.model.Article;
import com.sun.net.httpserver.*;
import db.DatabaseUseCase;

import java.io.*;
import java.util.ArrayList;
import java.util.Map;

import static Java.core.util.Fonction_Util.*;

public class Handler_Maraicher implements HttpHandler{

    private DatabaseUseCase databaseUseCase;

    public Handler_Maraicher(DatabaseUseCase databaseUseCase)
    {
        this.databaseUseCase = databaseUseCase;
    }

    @Override
    public void handle(HttpExchange exchange) throws IOException
    {
        exchange.getResponseHeaders().add("Access-Control-Allow-Origin", "*");
        exchange.getResponseHeaders().add("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        exchange.getResponseHeaders().add("Access-Control-Allow-Headers", "*");

        String requestMethod = exchange.getRequestMethod();
        String requestPath = exchange.getRequestURI().getPath();

        if ("OPTIONS".equalsIgnoreCase(exchange.getRequestMethod())) {
            exchange.sendResponseHeaders(204, -1); // No content for OPTIONS
        }
        else if (requestMethod.equalsIgnoreCase("GET")) {
            if ("/".equals(requestPath)) {
                // Renvoyer la page HTML pour la racine
                sendHtmlFileResponse(exchange, "../../../Html/index.html");
            } else if ("/api".equals(requestPath)) {
                // Récupérer la liste des tâches au format JSON
                ArrayList<Article> articles = databaseUseCase.getArticles();
                String response = convertArticlesToJson(articles);
                sendResponse(exchange, 200, response);
            } else if ("/api/1st_id".equals(requestPath)) {
                // Récupérer le 1ère id
                String response = databaseUseCase.getFirstAvailableId();
                sendResponse(exchange, 200, response);
            }else {
                System.out.println("GET_error: " + requestPath);
                sendResponse(exchange, 404, "Not Found");
            }
        }
        else if (requestMethod.equalsIgnoreCase("POST"))
        {
            System.out.println("--- Requête POST reçue (ajout) ---");
            // Ajouter une nouvelle tâche
            String requestBody = readRequestBody(exchange);
            System.out.println("requestBody = " + requestBody);
            Article newArticle = convertJsonToArticle(requestBody);
            databaseUseCase.addArticle(newArticle);
            sendResponse(exchange, 201, "Article ajoute avec succes");
        }
        else if (requestMethod.equalsIgnoreCase("PUT"))
        {
            System.out.println("--- Requête PUT reçue (mise a jour) ---");
            // Mettre à jour un Article existant
            Map<String, String> queryParams = parseQueryParams(exchange.getRequestURI().getQuery());

            if (queryParams.containsKey("id"))
            {
                int ArticleId = Integer.parseInt(queryParams.get("id"));

                String requestBody = readRequestBody(exchange);

                String[] parts = requestBody.replaceAll("[{}\"]", "").split(",");
                float prixUnitaire = Float.parseFloat(parts[0].split(":")[1]);
                int quantite = Integer.parseInt(parts[1].split(":")[1]);

                databaseUseCase.updateArticle(ArticleId, quantite, prixUnitaire);

                sendResponse(exchange, 200, "Tache mise a jour avec succes");
            }
            else sendResponse(exchange, 400, "ID de tache manquant dans les parametres");
        }
        else if (requestMethod.equalsIgnoreCase("DELETE"))
        {
            System.out.println("--- Requête DELETE reçue (suppression) ---");
            Map<String, String> queryParams = parseQueryParams(exchange.getRequestURI().getQuery());
            if (queryParams.containsKey("id"))
            {
                int ArticleId = Integer.parseInt(queryParams.get("id"));
                databaseUseCase.deleteArticle(ArticleId);
                sendResponse(exchange, 200, "Tache supprimee avec succes");
            }
            else sendResponse(exchange, 400, "ID de tache manquant dans les parametres");
        }
        else sendResponse(exchange, 405, "Methode non autorisee");
    }
}
