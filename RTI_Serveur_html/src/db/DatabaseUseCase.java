package db;



import Java.core.model.Article;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;

public class DatabaseUseCase {
    public DatabaseUseCase()
    {}
    public synchronized ArrayList<Article> getArticles()
    {
        String query = "SELECT * FROM ARTICLE";

        ArrayList<Article> articles = new ArrayList<>();

        try {
            ResultSet resultSet = DatabaseConnection.executeQuery(query);

            while(resultSet.next())
            {
                int idArticle = resultSet.getInt("ID");
                String intitule = resultSet.getString("INTITULE");
                int quantite = resultSet.getInt("STOCK");
                float prix = resultSet.getFloat("PRIX");
                String image = resultSet.getString("IMAGE");

                Article article = new Article(idArticle, intitule, quantite, prix,image);

                articles.add(article);

            }

            return articles;
        }
        catch (SQLException e)
        {
            e.printStackTrace();
            return new ArrayList<>();
        }
    }

    public synchronized String getFirstAvailableId() {

        int firstAvailableId = 1;

        for (Article article : getArticles()) {
            if (article.id() == firstAvailableId)
                firstAvailableId++;
            else
                break;
        }

        return String.valueOf(firstAvailableId);
    }

    public synchronized boolean addArticle(Article article)
    {
        String query = "INSERT INTO ARTICLE (ID, INTITULE, STOCK, PRIX, IMAGE) VALUES ('"
                + article.id() + "', '" + article.intitule() + "', '" + article.quantite() + "', '" + article.prixUnitaire() + "', '" + article.image() + "')";
        try
        {
            DatabaseConnection.executeUpdate(query);
        }
        catch (SQLException e)
        {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public synchronized boolean updateArticle(int articleId, int quantite, float prix)
    {
        String query = "UPDATE ARTICLE SET STOCK = " + quantite + ", PRIX = " + prix + " WHERE ID = " + articleId;


        try
        {
            DatabaseConnection.executeUpdate(query);
        }
        catch (SQLException e)
        {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public synchronized boolean deleteArticle(int articleId)
    {
        String query = "DELETE FROM ARTICLE WHERE ID = " + articleId;
        try
        {
            DatabaseConnection.executeUpdate(query);
        }
        catch (SQLException e)
        {
            e.printStackTrace();
            return false;
        }
        return true;
    }

}