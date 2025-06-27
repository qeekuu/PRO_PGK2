
#pragma once
#include "NetworkCommon.h"
#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include <functional>


class Enemy;

class NetworkManager {
public:
    enum class Mode { None, Server, Client };

    NetworkManager();
    ~NetworkManager();

   
    bool startServer(unsigned short port);
   
    bool startClient(const sf::IpAddress& serverIp, unsigned short port);
    Mode mode() const { return m_mode; }
   
    void sendPacket(sf::Packet packet); 
    void tickReceive();
    void handleIncoming(std::function<void(float, float)> spawnCallback);
    void broadcastState(const std::vector<std::shared_ptr<Enemy>>& enemies); 
    std::vector<sf::Packet>& getReceived() { return m_received; }
    const std::vector<sf::Packet>& getReceived() const { return m_received; }

    void clearReceived() { m_received.clear(); }

private:
    Mode m_mode;
    sf::TcpListener            m_listener;  
    sf::TcpSocket              m_socket;    
    std::vector<sf::TcpSocket*> m_clients;  
    std::vector<sf::Packet>     m_received; 
};