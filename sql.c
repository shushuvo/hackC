#include <stdio.h>
#include <mysql/mysql.h>

int main() {
    MYSQL *conn;
    
    // Initialize the MySQL library
    mysql_library_init(0, NULL, NULL);

    // Create a MySQL connection handle
    conn = mysql_init(NULL);

    // Connect to the MySQL server
    if (mysql_real_connect(conn, "localhost", "username", "password", "database_name", 0, NULL, 0) == NULL) {
        fprintf(stderr, "Error: %s\n", mysql_error(conn));
        mysql_close(conn);
        mysql_library_end();
        return 1;
    }

    // Connection successful
    printf("Connected to MySQL server!\n");

    // Perform database operations...

    // Close the MySQL connection
    mysql_close(conn);

    // Shutdown the MySQL library
    mysql_library_end();

    return 0;
}
