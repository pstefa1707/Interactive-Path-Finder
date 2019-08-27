#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "math.h"
#include "Cell.hpp"

int WIDTH = 1300;
int HEIGHT = 700;
sf::Vector2f GRID_SIZE(60, 40);
sf::Vector2f CELL_SIZE(WIDTH/GRID_SIZE.x, HEIGHT/GRID_SIZE.y);
sf::RenderWindow win(sf::VideoMode(WIDTH, HEIGHT), "Pstefa Testing", sf::Style::Close);

std::vector<Cell*> walls;
std::vector<Cell*> open;
std::vector<Cell*> closed;
std::vector<Cell*> solved;
Cell start(0, 0, 0);
Cell end(GRID_SIZE.x - 1, GRID_SIZE.y - 1, 0);
bool w_down = false;
bool r_down = false;
bool draw_grid = true;
bool executing = false;

void draw_lines(sf::Color colour);
void draw_cells(std::vector<Cell*> cells, sf::Color colour);
void draw_cell(Cell cell, sf::Color colour);
void set_cell(sf::Vector2i mouse_pos, Cell* cell);
bool add_walls(sf::Vector2i mouse_pos);
bool remove_walls(sf::Vector2i mouse_pos);
bool step();

int main()
{
    sf::Event ev;
    start.calculate_f(end.get_pos());

    while (win.isOpen())
    {
        while (win.pollEvent(ev))
        {
            switch (ev.type)
            {
                case sf::Event::Closed:
                    win.close();
                    break;

                case sf::Event::KeyPressed:
                    if (solved.size() > 0)
                    {
                        solved.erase(solved.begin(), solved.end());
                    }
                    if (ev.key.code == sf::Keyboard::S && !executing) set_cell(sf::Mouse::getPosition(win), &start);
                    if (ev.key.code == sf::Keyboard::E && !executing) set_cell(sf::Mouse::getPosition(win), &end);
                    if (!w_down && ev.key.code == sf::Keyboard::W) w_down = true;
                    if (!r_down && !w_down && ev.key.code == sf::Keyboard::R) r_down = true;
                    break;

                case sf::Event::KeyReleased:
                    if (ev.key.code == sf::Keyboard::W) w_down = false;
                    if (ev.key.code == sf::Keyboard::R) r_down = false;
                    if (ev.key.code == sf::Keyboard::G) draw_grid = !draw_grid;
                    if (ev.key.code == sf::Keyboard::C && !executing) walls.erase(walls.begin(), walls.end());
                    if (ev.key.code == sf::Keyboard::Space)
                    {
                        if (!executing)
                        {
                            executing = true;
                            solved.erase(solved.begin(), solved.end());
                            open.push_back(&start);
                        }
                        else
                        {
                            executing = false;
                            open.erase(open.begin(), open.end());
                            closed.erase(closed.begin(), closed.end());
                        }

                    }
                    break;

                default:
                    break;
            }
        }

        if (executing)
        {
            if (step())
            {
                executing = false;
                open.erase(open.begin(), open.end());
                closed.erase(closed.begin(), closed.end());
            }
        } else
        {
            if (w_down) add_walls(sf::Mouse::getPosition(win));
            if (r_down) remove_walls(sf::Mouse::getPosition(win));
        }

        win.clear(sf::Color::White);
        draw_cell(start, sf::Color::Blue);
        draw_cell(end, sf::Color::Red);
        draw_cells(walls, sf::Color::Black);
        draw_cells(open, sf::Color::Green);
        draw_cells(closed, sf::Color::Red);
        draw_cells(solved, sf::Color::Cyan);
        if (draw_grid) draw_lines(sf::Color::Black);
        win.display();

    }
    return 0;
}

void draw_cell(Cell cell, sf::Color colour)
{
    sf::Vector2f cellPos = cell.get_pos();
    sf::RectangleShape rect(sf::Vector2f(CELL_SIZE.x, CELL_SIZE.y));
    rect.setPosition(cellPos.x*CELL_SIZE.x, cellPos.y*CELL_SIZE.y);
    rect.setFillColor(colour);
    win.draw(rect);
}

void draw_cells(std::vector<Cell*> cells, sf::Color colour)
{
    for (Cell: cells)
    {
        sf::Vector2f cellPos = Cell->get_pos();
        sf::RectangleShape rect(sf::Vector2f(CELL_SIZE.x, CELL_SIZE.y));
        rect.setPosition(cellPos.x*CELL_SIZE.x, cellPos.y*CELL_SIZE.y);
        rect.setFillColor(colour);
        win.draw(rect);
    }
}

void draw_lines(sf::Color colour)
{
    for (int i = 0; i < GRID_SIZE.x; i++)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(i * (WIDTH/GRID_SIZE.x), 0)),
            sf::Vertex(sf::Vector2f(i * (WIDTH/GRID_SIZE.x), HEIGHT))
        };
        line[0].color = colour;
        line[1].color = colour;
        win.draw(line, 2, sf::Lines);
    }
    for (int j = 0; j < GRID_SIZE.y; j++)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(0, j * (HEIGHT/GRID_SIZE.y))),
            sf::Vertex(sf::Vector2f(WIDTH, j * (HEIGHT/GRID_SIZE.y)))
        };
        line[0].color = colour;
        line[1].color = colour;
        win.draw(line, 2, sf::Lines);
    }
}

void set_cell(sf::Vector2i mouse_pos, Cell* cell)
{
    if (mouse_pos.x > 0 && mouse_pos.x < WIDTH && mouse_pos.y > 0 && mouse_pos.y < HEIGHT)
    {
        int new_x = floor(mouse_pos.x/CELL_SIZE.x);
        int new_y = floor(mouse_pos.y/CELL_SIZE.y);
        *cell = Cell(new_x, new_y, 0);
        start.calculate_f(end.get_pos());
    }
}

bool add_walls(sf::Vector2i mouse_pos)
{
    if (mouse_pos.x > 0 && mouse_pos.x < WIDTH && mouse_pos.y > 0 && mouse_pos.y < HEIGHT)
    {
        int new_x = floor(mouse_pos.x/CELL_SIZE.x);
        int new_y = floor(mouse_pos.y/CELL_SIZE.y);
        for (wall : walls)
        {
            sf::Vector2f wall_pos = wall->get_pos();
            if (wall_pos.x == new_x && wall_pos.y == new_y) return false;
        }
        walls.push_back(new Cell(new_x, new_y, 0));
        std::cout << "added wall at position " << walls[0]->get_pos().x << ", " << walls[0]->get_pos().y << std::endl;
        return true;
    }
    return false;
}

bool remove_walls(sf::Vector2i mouse_pos)
{
    if (mouse_pos.x > 0 && mouse_pos.x < WIDTH && mouse_pos.y > 0 && mouse_pos.y < HEIGHT)
    {
        int del_x = floor(mouse_pos.x/CELL_SIZE.x);
        int del_y = floor(mouse_pos.y/CELL_SIZE.y);
        int index = 0;
        for (wall: walls)
        {
            sf::Vector2f wall_pos = wall->get_pos();
            if (wall_pos.x == del_x && wall_pos.y == del_y)
            {
                walls.erase(walls.begin() + index);
                std::cout << "removed wall at position " << del_x << ", " << del_y << std::endl;
            }
            index++;
        }
    }
    return false;
}

bool is_wall(sf::Vector2f cell_position)
{
    for (wall: walls)
    {
        if (wall->get_pos().x == cell_position.x && wall->get_pos().y == cell_position.y)
        {
            return true;
        }
    }
    return false;
}
bool in_vect(std::vector<Cell*> *vect, sf::Vector2f checkCell)
{
    for (cell: *vect)
    {
        if (checkCell.x == cell->get_pos().x && checkCell.y == cell->get_pos().y) return true;
    }
    return false;
}

void find_children(std::vector<Cell*>* children, Cell* Q_Node)
{
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            int new_x = Q_Node->get_pos().x + x;
            int new_y = Q_Node->get_pos().y + y;
            if (!(x == 0 && y ==0))
            {
                if (new_x < GRID_SIZE.x && new_x >= 0 && new_y < GRID_SIZE.y && new_y >= 0 && !in_vect(&walls, sf::Vector2f(new_x, new_y)))
                {
                    Cell* new_cell = new Cell(new_x, new_y, Q_Node);
                    new_cell->calculate_g();
                    new_cell->calculate_f(end.get_pos());
                    children->push_back(new_cell);
                }
            }
        }
    }
}

bool recurse(Cell* node)
{
    if (node->get_parent() == 0) return true;
    solved.push_back(node);
    return recurse(node->get_parent());
}

bool step()
{
    if (open.size() == 0)
    {
        std::cout << "No solution found!" << std::endl;
        return true;
    }
    sort(open.begin(), open.end(), []( Cell*& lhs, Cell*& rhs )
    {
        return lhs->get_f() < rhs->get_f();
    });
    Cell* Q_Node = open[0];
    open.erase(open.begin());
    if (Q_Node->get_pos().x == end.get_pos().x && Q_Node->get_pos().y == end.get_pos().y)
    {
        std::cout << "Path found!" << std::endl;
        recurse(Q_Node);
        return true;
    }
    std::vector<Cell*> children;
    find_children(&children, Q_Node);
    for(child: children)
    {
        if (in_vect(&closed, child->get_pos())) continue;
        child->calculate_g();
        child->calculate_f(end.get_pos());
        if (!in_vect(&open, child->get_pos())) open.push_back(child);
    }
    closed.push_back(Q_Node);
    return false;
}
