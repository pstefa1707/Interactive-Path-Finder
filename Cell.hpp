#include <SFML/Graphics.hpp>

class Cell
{
private:
    sf::Vector2f pos;
    Cell* parent;
    int g = 0;
    int q;
    int f;
public:
    sf::Vector2f get_pos();
    Cell* get_parent();
    void calculate_f(sf::Vector2f endPos);
    void calculate_g();
    int get_f();
    Cell(int x, int y, Cell* parent)
    {
        this->parent = parent;
        this->pos = sf::Vector2f(x, y);
    }
};
