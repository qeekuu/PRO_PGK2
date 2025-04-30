#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <random>
#include <cassert>
#include <string>

// ============================================================================
// Typy i stałe
// ============================================================================

using Tile = std::string;
using Coordinates = std::pair<int,int>;

// Proste struktury do reprezentowania kierunku
struct Direction {
    int dy;
    int dx;
};

// Kierunki zgodne z oznaczeniami w Pythonie
static const Direction UP    = {1,  0};
static const Direction DOWN  = {-1, 0};
static const Direction LEFT  = {0, -1};
static const Direction RIGHT = {0,  1};

// Przydatne do iterowania po kierunkach
static const std::vector<Direction> DIRS = {UP, DOWN, LEFT, RIGHT};

// Struktura do przechowywania informacji o kompatybilności:
// (tile1, tile2, direction)
struct Compatibility {
    Tile tile1;
    Tile tile2;
    Direction dir;
};

// Dla std::set / std::map potrzebny jest operator < (albo ==/hash dla unordered_*)
inline bool operator<(const Compatibility& c1, const Compatibility& c2) {
    // Tutaj prosta leksykograficzna definicja porządku
    if(c1.tile1 < c2.tile1) return true;
    if(c1.tile1 > c2.tile1) return false;

    if(c1.tile2 < c2.tile2) return true;
    if(c1.tile2 > c2.tile2) return false;

    // Porównanie kierunku
    if(c1.dir.dy < c2.dir.dy) return true;
    if(c1.dir.dy > c2.dir.dy) return false;

    return (c1.dir.dx < c2.dir.dx);
}

// Przy okazji można zdefiniować operator==, jeśli chcemy używać np. unordered_set
inline bool operator==(const Compatibility& c1, const Compatibility& c2) {
    return (c1.tile1 == c2.tile1 &&
            c1.tile2 == c2.tile2 &&
            c1.dir.dy == c2.dir.dy &&
            c1.dir.dx == c2.dir.dx);
}

// Zbiór (set) lub ewentualnie unordered_set, przechowujemy "dozwolone" kombinacje
using CompatibilitySet = std::set<Compatibility>;

// Słownik kafelek -> waga
using Weights = std::map<Tile, int>;

// Każda komórka w fali przechowuje zbiór możliwych kafelek
using Coefficients = std::set<Tile>;

// 2D macierz zbiorów
using CoefficientMatrix = std::vector<std::vector<Coefficients>>;


// ============================================================================
// Klasa CompatibilityOracle
// ============================================================================

class CompatibilityOracle {
public:
    explicit CompatibilityOracle(const CompatibilitySet& data)
        : data_(data) {}

    // Metoda check: czy tile1 i tile2 są kompatybilne w danym kierunku
    bool check(const Tile& tile1, const Tile& tile2, const Direction& direction) const {
        // Sprawdzamy, czy (tile1, tile2, direction) jest w zbiorze dozwolonych
        Compatibility c{tile1, tile2, direction};
        return (data_.find(c) != data_.end());
    }

private:
    CompatibilitySet data_;
};

// ============================================================================
// Klasa Wavefunction
// ============================================================================

class Wavefunction {
public:
    static Wavefunction createWavefunction(const sf::Vector2i& size, const Weights& weights) {
        // Inicjalizacja macierzy współczynników
        std::vector<Tile> allTiles;
        allTiles.reserve(weights.size());
        for (auto& [tile, _] : weights) {
            allTiles.push_back(tile);
        }
        auto matrix = initCoefficientMatrix(size, allTiles);
        return Wavefunction(matrix, weights);
    }

    // Zwraca referencję do zbioru możliwych kafelek w danym miejscu
    const Coefficients& get(const Coordinates& coords) const {
        return coefficient_matrix_[coords.first][coords.second];
    }
    // Zwraca mutowalną referencję (jeśli chcemy wprowadzać zmiany)
    Coefficients& getMutable(const Coordinates& coords) {
        return coefficient_matrix_[coords.first][coords.second];
    }

    // Gdy w danym polu jest tylko 1 kafelek, pobierz go
    Tile get_collapsed(const Coordinates& coords) const {
        const auto& opts = get(coords);
        assert(opts.size() == 1 && "Komórka nie jest jednoznacznie ustalona!");
        return *opts.begin();
    }

    // Sprawdź, czy wszystkie pola zredukowały się do 1 kafelka
    bool is_fully_collapsed() const {
        for (auto& row : coefficient_matrix_) {
            for (auto& cell : row) {
                if (cell.size() > 1) {
                    return false;
                }
            }
        }
        return true;
    }

    // Oblicz entropię Shannona w danym punkcie
    double shannon_entropy(const Coordinates& coords) const {
        const auto& opts = get(coords);
        double sum_of_weights = 0.0;
        double sum_of_weight_log_weights = 0.0;

        for (const auto& tile : opts) {
            auto w = weights_.at(tile);
            sum_of_weights += w;
            sum_of_weight_log_weights += (w * std::log((double)w));
        }

        if (sum_of_weights == 0.0) {
            return 0.0; // zabezpieczenie
        }

        return std::log(sum_of_weights) - (sum_of_weight_log_weights / sum_of_weights);
    }

    // Kolaps komórki do jednego kafelka (losowo, zgodnie z wagami)
    void collapse(const Coordinates& coords, std::mt19937& rng) {
        auto& opts = getMutable(coords);

        // Zbierz (kafelek, waga) tylko dla aktualnie dozwolonych kafelek
        std::vector<std::pair<Tile,int>> filtered;
        filtered.reserve(opts.size());

        int totalWeight = 0;
        for (auto& [tile, w] : weights_) {
            if (opts.find(tile) != opts.end()) {
                filtered.emplace_back(tile, w);
                totalWeight += w;
            }
        }

        // Losowanie
        std::uniform_real_distribution<double> dist(0.0, (double)totalWeight);
        double rnd = dist(rng);

        Tile chosen = filtered.front().first;
        for (auto& [tile, w] : filtered) {
            rnd -= w;
            if (rnd < 0.0) {
                chosen = tile;
                break;
            }
        }

        // Ustaw w tym miejscu tylko wybraną kafelkę
        opts.clear();
        opts.insert(chosen);
    }

    // Usuwa `forbiddenTile` z listy możliwych kafelek w `coords`
    void constrain(const Coordinates& coords, const Tile& forbiddenTile) {
        auto& opts = getMutable(coords);
        opts.erase(forbiddenTile);
    }

    // Metoda pomocnicza do tworzenia finalnej macierzy (2D) kafelek
    std::vector<std::vector<Tile>> get_all_collapsed() const {
        std::vector<std::vector<Tile>> result;
        result.resize(coefficient_matrix_.size());
        for (size_t y = 0; y < coefficient_matrix_.size(); ++y) {
            result[y].resize(coefficient_matrix_[y].size());
            for (size_t x = 0; x < coefficient_matrix_[y].size(); ++x) {
                // Każda komórka powinna mieć 1 kafelek
                result[y][x] = get_collapsed({(int)y, (int)x});
            }
        }
        return result;
    }

private:
    Wavefunction(const CoefficientMatrix& matrix, const Weights& weights)
        : coefficient_matrix_(matrix), weights_(weights) {}

    // Inicjalizacja macierzy wavefunction (w każdym polu zestaw wszystkich możliwych kafelek)
    static CoefficientMatrix initCoefficientMatrix(const sf::Vector2i& size, const std::vector<Tile>& allTiles) {
        CoefficientMatrix matrix;
        matrix.resize(size.y, std::vector<Coefficients>(size.x));

        for (int y = 0; y < size.y; ++y) {
            for (int x = 0; x < size.x; ++x) {
                // Na starcie wszystkie możliwe
                matrix[y][x] = Coefficients(allTiles.begin(), allTiles.end());
            }
        }
        return matrix;
    }

private:
    CoefficientMatrix coefficient_matrix_;
    Weights weights_;
};

// ============================================================================
// Funkcje pomocnicze
// ============================================================================

// Zwraca listę kierunków, które są dozwolone z danego punktu w macierzy o wymiarach (width, height).
std::vector<Direction> valid_dirs(const Coordinates& coords, const sf::Vector2i& matrixSize) {
    int y = coords.first;
    int x = coords.second;
    std::vector<Direction> result;

    if (y < matrixSize.y - 1) result.push_back(UP);
    if (y > 0)              result.push_back(DOWN);
    if (x > 0)              result.push_back(LEFT);
    if (x < matrixSize.x - 1) result.push_back(RIGHT);

    return result;
}

// Parsuje przykładową macierz (tak jak w Pythonie) i tworzy:
//
// 1. Zestaw kompatybilności (tile1, tile2, direction)
// 2. Wagi występowania kafelek
std::pair<CompatibilitySet, Weights> parse_example_matrix(const std::vector<std::vector<Tile>>& matrix) {
    CompatibilitySet compatibilities;
    Weights weights;

    int height = (int)matrix.size();
    if (height == 0) return {compatibilities, weights};
    int width = (int)matrix[0].size();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile curTile = matrix[y][x];
            // Zliczanie częstości występowania kafelka
            weights[curTile] += 1;

            // Sprawdź kierunki dookoła
            auto dirs = valid_dirs({y, x}, {width, height});
            for (auto& d : dirs) {
                int ny = y + d.dy;
                int nx = x + d.dx;
                Tile otherTile = matrix[ny][nx];
                // Dodajemy do zbioru (curTile, otherTile, d)
                compatibilities.insert(Compatibility{curTile, otherTile, d});
            }
        }
    }
    return {compatibilities, weights};
}

// ============================================================================
// Klasa Model (rdzeń algorytmu WaveFunctionCollapse)
// ============================================================================

class Model {
public:
    Model(const sf::Vector2i& outputSize,
          const Weights& weights,
          const CompatibilityOracle& oracle)
    : output_size_(outputSize)
    , oracle_(oracle)
	, wavefunction_(Wavefunction::createWavefunction(outputSize, weights))
    {
        // Stworzenie początkowej fali
        wavefunction_ = Wavefunction::createWavefunction(outputSize, weights);
        // Przygotowanie generatora
        std::random_device rd;
        rng_.seed(rd());
    }

    std::vector<std::vector<Tile>> run() {
        // Dopóki nie jest w pełni skompresowane (collapsed)
        while (!wavefunction_.is_fully_collapsed()) {
            iterate();
        }
        return wavefunction_.get_all_collapsed();
    }

private:
    void iterate() {
        // 1. Znajdź współrzędne o minimalnej entropii
        auto coords = min_entropy_coords();

        // 2. Kolaps w tym miejscu
        wavefunction_.collapse(coords, rng_);

        // 3. Propagacja
        propagate(coords);
    }

    void propagate(const Coordinates& coords) {
        std::vector<Coordinates> stack;
        stack.push_back(coords);

        while (!stack.empty()) {
            auto cur = stack.back();
            stack.pop_back();

            // Zbierz kafelki możliwe w cur
            const auto& curPossibleTiles = wavefunction_.get(cur);

            // Dla każdego sąsiada
            auto dirs = valid_dirs(cur, output_size_);
            for (auto& d : dirs) {
                Coordinates other = {cur.first + d.dy, cur.second + d.dx};
                // Kopiujemy kafelki do innej struktury, żeby
                // móc iterować i ewentualnie usuwać
                auto otherTiles = wavefunction_.get(other);

                for (auto& ot : otherTiles) {
                    // Czy dany kafelek jest kompatybilny z czymkolwiek w cur?
                    bool isPossible = false;
                    for (auto& ct : curPossibleTiles) {
                        if (oracle_.check(ct, ot, d)) {
                            isPossible = true;
                            break;
                        }
                    }
                    // Jeśli nie jest kompatybilny z żadnym kafelkiem z cur,
                    // to musimy usunąć go z possible
                    if (!isPossible) {
                        wavefunction_.constrain(other, ot);
                        stack.push_back(other);
                    }
                }
            }
        }
    }

    // Znajduje współrzędne o minimalnej entropii
    Coordinates min_entropy_coords() {
        double minEntropy = 1e30; // coś dużego
        Coordinates bestCoords = {0, 0};

        for (int y = 0; y < output_size_.y; ++y) {
            for (int x = 0; x < output_size_.x; ++x) {
                auto& cell = wavefunction_.get({y,x});
                if (cell.size() == 1) {
                    // Już collapsed; pomiń
                    continue;
                }
                double ent = wavefunction_.shannon_entropy({y,x});
                // dodanie drobnego szumu, by uniknąć ciągłego wybierania
                // tych samych przy remisach
                double noise = std::uniform_real_distribution<double>(0.0,0.001)(rng_);
                double val = ent - noise;

                if (val < minEntropy) {
                    minEntropy = val;
                    bestCoords = {y, x};
                }
            }
        }
        return bestCoords;
    }

private:
    sf::Vector2i output_size_;
    CompatibilityOracle oracle_;
    Wavefunction wavefunction_;
    std::mt19937 rng_;
};

// ============================================================================
// Funkcja renderująca wynik do okna SFML
// ============================================================================

// Prosta mapa: z kafelka do koloru SFML
std::map<Tile, sf::Color> tileColors = {
    {"L", sf::Color::Green},
    {"S", sf::Color::Blue},
    {"C", sf::Color::Yellow},
    {"A", sf::Color::Cyan},
    {"B", sf::Color::Magenta}
};

void renderSFML(const std::vector<std::vector<Tile>>& matrix) {
    // Rozmiar pojedynczego pola (piksele)
    const int cellSize = 20;

    int height = (int)matrix.size();
    if(height == 0) return;
    int width = (int)matrix[0].size();

    // Tworzymy okno
    sf::RenderWindow window(sf::VideoMode(width * cellSize, height * cellSize), "WaveFunctionCollapse - SFML");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        // Rysowanie kafelek
        sf::RectangleShape rect(sf::Vector2f((float)cellSize, (float)cellSize));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const auto& tile = matrix[y][x];
                sf::Color color = sf::Color::White; // domyślnie
                if (tileColors.find(tile) != tileColors.end()) {
                    color = tileColors[tile];
                }
                rect.setPosition((float)(x * cellSize), (float)(y * cellSize));
                rect.setFillColor(color);
                window.draw(rect);
            }
        }

        window.display();
    }
}

// ============================================================================
// Przykładowa funkcja main
// ============================================================================

int main() {
    // Przykładowa macierz wejściowa (taka sama jak w Pythonie)
    std::vector<std::vector<Tile>> input_matrix = {
        {"L","L","L","L"},
        {"L","L","L","L"},
        {"L","L","L","L"},
        {"L","C","C","L"},
        {"C","S","S","C"},
        {"S","S","S","S"},
        {"S","S","S","S"},
    };

    // Wydobycie informacji o kompatybilnościach i wagach
    auto [compatibilities, weights] = parse_example_matrix(input_matrix);

    // Tworzymy obiekt-oraklę zebranych kompatybilności
    CompatibilityOracle oracle(compatibilities);

    // Tworzymy Model, podajemy rozmiar wyjścia (szerokość=50, wysokość=10)
    sf::Vector2i outputSize(50, 10);
    Model model(outputSize, weights, oracle);

    // Uruchamiamy algorytm
    auto output = model.run();

    // Wyświetlamy wynik w oknie SFML
    renderSFML(output);

    return 0;
}

