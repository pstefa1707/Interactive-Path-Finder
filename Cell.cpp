#include "Cell.hpp"

sf::Vector2f Cell::get_pos()
{
    return this->pos;
}

Cell* Cell::get_parent()
{
    return this->parent;
}

int Cell::get_f()
{
    return this->f;
}

void Cell::calculate_f(sf::Vector2f endPos)
{
    this->q = sqrt(pow(abs(this->pos.x - endPos.x), 2) + pow(abs(this->pos.y - endPos.y), 2));
    this->f = this->g + this->q;
}

void Cell::calculate_g()
{
    this->g = this->parent->g + 1;
}
