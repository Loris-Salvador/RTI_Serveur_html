package Java;

import Java.handler.Handler_Maraicher;
import Java.handler.StaticFileHandler;
import com.sun.net.httpserver.HttpServer;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.sql.SQLException;

import db.*;

public class Main {
    public static void main(String[] args) throws IOException {
        System.out.println("API Rest demarree...");

        //DB////////////////////////////////////////////////////
        try {
            DatabaseConnection databaseConnection = new DatabaseConnection(DatabaseConnection.MYSQL, "localhost", "PSLA_RTI", "RTI", "RTI");
        }
        catch (ClassNotFoundException | SQLException e)
        {
            e.printStackTrace();
            System.exit(1);
        }

        DatabaseUseCase databaseUseCase = new DatabaseUseCase();

        HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
        server.createContext("/", new Handler_Maraicher(databaseUseCase));
        server.createContext("/api", new Handler_Maraicher(databaseUseCase));
        server.createContext("/api/1st_id", new Handler_Maraicher(databaseUseCase));

        // Ajoutez cela dans votre gestionnaire principal
        server.createContext("/css", new StaticFileHandler());
        server.createContext("/javascript", new StaticFileHandler());
        server.createContext("/image", new StaticFileHandler());

        server.start();
    }
}