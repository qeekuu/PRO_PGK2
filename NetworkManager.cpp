
#include "NetworkManager.h"
#include "Enemy.h"
NetworkManager::NetworkManager()
    : m_mode(Mode::None)
{
}

NetworkManager::~NetworkManager() {
    
    for (auto* c : m_clients) {
        c->disconnect();
        delete c;
    }
    m_socket.disconnect();
}

bool NetworkManager::startServer(unsigned short port) {
    if (m_listener.listen(port) != sf::Socket::Status::Done)
        return false;
    m_listener.setBlocking(false);
    m_mode = Mode::Server;
    return true;
}

bool NetworkManager::startClient(const sf::IpAddress& ip, unsigned short port) {
    if (m_socket.connect(ip, port, sf::seconds(2.f)) != sf::Socket::Status::Done)
        return false;
    m_socket.setBlocking(false);
    m_mode = Mode::Client;
    return true;
}

void NetworkManager::sendPacket(sf::Packet packet) {
    if (m_mode == Mode::Server) {
        for (auto* client : m_clients)
            client->send(packet);
    }
    else if (m_mode == Mode::Client) {
        m_socket.send(packet);
    }
}

void NetworkManager::tickReceive() {
  
    if (m_mode == Mode::Server) {
        auto* newClient = new sf::TcpSocket;
        if (m_listener.accept(*newClient) == sf::Socket::Status::Done) {
            newClient->setBlocking(false);
            m_clients.push_back(newClient);
        }
        else {
            delete newClient;
        }
       
        for (auto* c : m_clients) {
            sf::Packet p;
            if (c->receive(p) == sf::Socket::Status::Done)
                m_received.push_back(p);
        }
    }
   
    else if (m_mode == Mode::Client) {
        sf::Packet p;
        if (m_socket.receive(p) == sf::Socket::Status::Done)
            m_received.push_back(p);
    }
}

void NetworkManager::handleIncoming(std::function<void(float, float)> spawnCallback) {
    for (auto& p : m_received) {
        PacketType type;
        p >> type;
        if (type == PacketType::SpawnEnemy) {
            float x, y;
            p >> x >> y;
            spawnCallback(x, y);
        }
    }
    m_received.clear();
}

void NetworkManager::broadcastState(const std::vector<std::shared_ptr<Enemy>>& enemies) {
    sf::Packet p;
    p << PacketType::StateUpdate
        << uint32_t(enemies.size());
    for (auto& e : enemies) {
        p << e->getId()
            << e->getType()
            << e->getPosition().x
            << e->getPosition().y;
    }
    sendPacket(p);
}