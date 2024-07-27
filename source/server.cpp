#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>

#define PORT 8080

int main() {
    int server_socket_file_desc;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    // Create socket file descriptor
    // AF_INET: Address Family Internet - IPv4
    // SOCK_STREAM: Type of socket used with AF_INET - TCP (Stream oriented)
    if ((server_socket_file_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port 8080
    if (setsockopt(server_socket_file_desc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_socket_file_desc, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        close(server_socket_file_desc);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket_file_desc, 3) == -1) {
        perror("Listen");
        close(server_socket_file_desc);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while(true) {
        // Accept a connection from a client
        int new_socket = accept(server_socket_file_desc, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        if (new_socket == -1) {
            perror("Accept Failed: ");
            continue;  // If accept fails, keep checking
        }

        std::cout << "Client connected" << std::endl;

        // Handle client communication
        while (true) {
            std::memset(buffer, 0, sizeof(buffer));
            int num_bytes_read = read(new_socket, buffer, 1024);
            
            if (num_bytes_read > 0) {
                std::cout << "Client: " << buffer << std::endl;
                std::string message = "Received: " + std::string(buffer);
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else if (num_bytes_read > 0) {
                std::cout << "Client disconnected" << std::endl;
                break;
            }
            else {
                perror("Read Failed: ");
                break;
            }

        // Close the connection with the client
        close(new_socket);

        }

    }
    // Close the listening socket (never reached in this example)
    close(server_socket_file_desc);
}


/*
Server:

get address info -> configure socket -> bind -> listen -> accept  -> receive -> send -> receive -> close
                                                           ||________________________________________|
                                                           | 
Client:                                                    |
                                                           _
get address info -> configure socket -------------------> Connect -> send -> receive -> close

*/