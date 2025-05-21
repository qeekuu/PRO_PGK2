#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

int main() {
    sf::UdpSocket socket;
    const sf::IpAddress serverIp = "127.0.0.1"; // adres serwera
    const unsigned short serverPort = 54000;

    // Nieblokujące, żeby móc zarówno wysyłać, jak i odbierać bez wstrzymywania
    socket.setBlocking(false);

    // Wątek do odbierania odpowiedzi od serwera
    std::thread receiveThread([&]() {
        sf::Packet packet;
        sf::IpAddress senderIp;
        unsigned short senderPort;
        while (true) {
            sf::Socket::Status status = socket.receive(packet, senderIp, senderPort);
            if (status == sf::Socket::Done) {
                std::string msg;
                packet >> msg;
                std::cout << "[Serwer->Klient] " << msg << "\n";
                packet.clear();
            }
            // w NotReady można pominąć
            sf::sleep(sf::milliseconds(10));
        }
    });

    // Pętla wysyłania wiadomości do serwera
    std::string line;
    while (std::getline(std::cin, line)) {
        sf::Packet packet;
        packet << line;
        if (socket.send(packet, serverIp, serverPort) != sf::Socket::Done) {
            std::cerr << "Błąd wysyłania do serwera\n";
        }
    }

    receiveThread.join();
    return 0;
}

