#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <string>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <time.h>

class TileMap : public sf::Drawable, public sf::Transformable
{
public:

    bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
    {
        // load the tileset texture
        if (!m_tileset.loadFromFile(tileset))
            return false;

        // resize the vertex array to fit the level size
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(width * height * 4);

        // populate the vertex array, with one quad per tile
        for (unsigned int i = 0; i < width; ++i)
            for (unsigned int j = 0; j < height; ++j)
            {
                // get the current tile number
                int tileNumber = tiles[i + j * width];

                // find its position in the tileset texture
                int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                // get a pointer to the current tile's quad
                sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

                // define its 4 corners
                quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                // define its 4 texture coordinates
                quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
                quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
                quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
                quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
            }

        return true;
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};

class Text : public sf::Text {
public:
    Text() {

        setFillColor(sf::Color::White);
        setCharacterSize(20);
    }
};

class Entity : public sf::CircleShape {
public:
    int health, attack, xp, coins, direction;
    int movement = 0;
};

class Players : public Entity {
public:
    int level, stage;
    Players() {
        health = 10;
        attack = 3;
        xp = 0;
        coins = 0;
        level = 0;
        stage = 1;
        setFillColor(sf::Color::White);
        setRadius(20);
        setPosition(400, 400);
    }
    
};
class PlayersLight : public Entity{
public:
    PlayersLight() {
        setFillColor(sf::Color(0,0,0,125));
        setRadius(200);
        setPointCount(100);
        setOutlineColor(sf::Color(24,24,24,255));
        setOutlineThickness(1600);//1600
    }
};
class PlayersFist : public Entity {
public:
    PlayersFist() {
        setFillColor(sf::Color::White);
        setRadius(5);
    }
};

class Snake : public Entity {
public:
    Snake() {
        health = 5;
        attack = 1;
        xp = 1;
        coins = 1;
        setFillColor(sf::Color((rand() % 105), (rand() % 105+150), (rand() % 105), 255));
        setRadius(rand() % 5 + 10);
    }
};

void moving(Entity& A) {
    switch (A.direction) {
    case 1:
        A.move(-.05, 0);
        break;
    case 2:
        A.move(.05, 0);
        break;
    case 3:
        A.move(0, -.05);
        break;
    case 4:
        A.move(0, .05);
        break;
    case 5:
        A.move(-.025, -.025);
        break;
    case 6:
        A.move(.025, .025);
        break;
    case 7:
        A.move(.025, -.025);
        break;
    case 8:
        A.move(-.025, .025);
        break;
    }
}

bool collision(Entity& A, Entity B){
    float dx = A.getPosition().x + A.getRadius() - B.getPosition().x - B.getRadius();
    float dy = A.getPosition().y + A.getRadius() - B.getPosition().y - B.getRadius();
    float d = std::sqrt((dx * dx) + (dy * dy));
    if (d <= (A.getRadius() + B.getRadius())) {
        A.move(dx/2, dy/2);
        return true;
    }
        
    else
        return false;
}

void wallCollision(Entity& A) {
    if (A.getPosition().x <= 32) {
        A.move(1, 0);
    }
    if (   ( A.getPosition().x + (  2 * A.getRadius() )  ) >= 736) {
        A.move(-1, 0);
    }
    if (A.getPosition().y <= 32) {
        A.move(0, 1);
    }
    if ((A.getPosition().y + (2 * A.getRadius())) >= 736) {
        A.move(0, -1);
    }
}

void setNewPosition(Entity A, Entity& B) {
    int x = ceilf(A.getPosition().x);
    int y = ceilf(A.getPosition().y);
    B.setPosition((x + (rand() % 300) + 200)%700, (y + (rand() % 500) + 200)%700);
}


int main()
{
    srand(time(NULL));

    float rot = 0;

    bool quit = false;

    int collide = 0;

    int snakeKills = 0;

    sf::Font myFont;

    myFont.loadFromFile("arial.ttf");

    /*std::vector<sf::CircleShape> myVector(10, sf::CircleShape());
    for (int i = 0; i < myVector.size(); i++) {
        myVector[i].setFillColor(sf::Color((rand()%255), (rand() % 255), (rand() % 255),255));
        myVector[i].setPosition((rand() % 455), (rand() % 455));
        myVector[i].setRadius((rand() % 50 + 10));
    }*/
    //myVector.erase(myVector.begin() + i );

    std::vector<Entity> entities;
    entities.push_back(PlayersLight()); //Entity 0 is always light
    entities.push_back(PlayersFist());//Entity 1 is always fist
    entities.push_back(Players()); //Entity 2 is always player
 
    for (int i = 1; i <= 10; i++) {
        entities.push_back(Snake());
    }
    
    for (int i = 3; i < entities.size(); i++) {
        setNewPosition(entities[1], entities[i]);
    }
        
    std::vector<Text> texts;

    for (int i = 1; i <= 10; i++) {
        texts.push_back(Text());
    }

    

    sf::RenderWindow window(sf::VideoMode(896, 896), "GAME");

    const int level1[] =
    {
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, //5, 6, 6, 7,
4, 3, 2, 1, 3, 1, 3, 2, 0, 1, 1, 2, 3, 2, 3, 3, 2, 0, 2, 0, 0, 3, 0, 4, //11, 13, 13, 12,
4, 2, 3, 3, 3, 1, 2, 2, 2, 1, 3, 1, 0, 3, 2, 1, 1, 1, 3, 0, 1, 2, 0, 4, //11, 13, 13, 12,
4, 3, 0, 0, 1, 2, 2, 0, 1, 1, 1, 0, 3, 0, 1, 2, 1, 1, 1, 0, 3, 2, 1, 4, //11, 13, 13, 12,
4, 3, 2, 3, 0, 0, 2, 0, 0, 3, 3, 2, 3, 0, 0, 0, 0, 3, 0, 2, 2, 2, 3, 4, //11, 13, 13, 12,
4, 3, 1, 1, 2, 1, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0, 3, 0, 1, 3, 1, 1, 3, 4, //11, 13, 13, 12,
4, 3, 0, 1, 1, 2, 1, 2, 3, 3, 0, 0, 3, 0, 1, 3, 3, 2, 0, 0, 3, 0, 1, 4, //11, 13, 13, 12,
4, 2, 3, 1, 3, 3, 0, 0, 0, 1, 2, 1, 0, 2, 0, 0, 2, 3, 0, 3, 3, 3, 1, 4, //11, 13, 13, 12,
4, 1, 0, 3, 3, 2, 1, 2, 3, 1, 1, 3, 1, 2, 0, 3, 3, 0, 1, 0, 0, 3, 3, 4, //11, 13, 13, 12,
4, 0, 2, 0, 0, 1, 1, 1, 0, 0, 3, 2, 3, 2, 3, 0, 1, 0, 2, 1, 3, 2, 2, 4, //11, 13, 13, 12,
4, 1, 3, 3, 0, 0, 3, 2, 0, 3, 3, 2, 0, 0, 2, 3, 2, 1, 1, 1, 2, 2, 1, 4, //11, 13, 13, 12,
4, 2, 1, 0, 0, 2, 2, 0, 1, 2, 0, 0, 0, 0, 3, 3, 2, 0, 3, 1, 3, 1, 2, 4, //11, 13, 13, 12,
4, 3, 1, 3, 0, 3, 1, 1, 0, 2, 3, 2, 2, 0, 0, 2, 0, 0, 2, 0, 3, 1, 0, 4, //11, 13, 13, 12,
4, 0, 0, 3, 1, 0, 2, 2, 3, 2, 1, 1, 0, 1, 3, 3, 3, 1, 0, 3, 3, 0, 3, 4, //11, 13, 13, 12,
4, 0, 2, 0, 0, 2, 1, 0, 1, 2, 0, 0, 0, 0, 2, 1, 1, 2, 3, 0, 2, 2, 1, 4, //11, 13, 13, 12,
4, 1, 1, 3, 0, 0, 3, 2, 0, 3, 1, 1, 0, 2, 3, 0, 2, 3, 0, 1, 1, 1, 3, 4, //11, 13, 13, 12,
4, 2, 3, 0, 2, 2, 2, 0, 1, 2, 0, 0, 1, 0, 0, 2, 2, 0, 0, 1, 0, 3, 1, 4, //11, 13, 13, 12,
4, 3, 2, 0, 1, 0, 2, 0, 1, 1, 2, 3, 1, 3, 1, 1, 0, 2, 1, 0, 0, 3, 0, 4, //11, 13, 13, 12,
4, 2, 1, 0, 0, 0, 3, 2, 0, 0, 2, 2, 1, 3, 3, 3, 2, 0, 3, 0, 1, 3, 2, 4, //11, 13, 13, 12,
4, 3, 0, 3, 3, 0, 1, 1, 0, 1, 1, 3, 3, 1, 0, 2, 3, 1, 1, 3, 0, 0, 3, 4, //11, 13, 13, 12,
4, 2, 0, 1, 1, 2, 0, 1, 2, 0, 1, 3, 1, 1, 1, 2, 1, 0, 3, 1, 2, 2, 2, 4, //11, 13, 13, 12,
4, 1, 2, 0, 2, 0, 2, 2, 2, 0, 0, 2, 1, 2, 2, 3, 2, 3, 0, 3, 1, 1, 3, 4, //11, 13, 13, 12,
4, 0, 2, 0, 3, 2, 1, 1, 2, 3, 1, 0, 1, 3, 0, 2, 2, 2, 0, 0, 1, 2, 0, 4, //8, 9, 9, 10,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, //13, 13, 13, 13,

    };
    TileMap map1;
    if (!map1.load("tiles2.png", sf::Vector2u(32, 32), level1, 24, 24))
        return -1;

    const int level2[] =
    {
5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 13, 5, 6, 6, 7,
11, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 13, 11, 13, 13, 12,
11, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 13, 11, 13, 13, 12,
8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 13, 8, 9, 9, 10
    };

    TileMap map2;
    if (!map2.load("tiles2.png", sf::Vector2u(32, 32), level2, 28, 4))
        return -1;
    map2.setPosition(0, 768);

    const int level3[] =
    {
        5, 6, 6, 7,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        11, 13, 13, 12,
        8, 9, 9, 10,
        13, 13, 13, 13
    };
    TileMap map3;
    if (!map3.load("tiles2.png", sf::Vector2u(32, 32), level3, 4, 24))
        return -1;
    map3.setPosition(768, 0);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || quit )
                window.close();
        }

        //INPUT///////////////---------------------------
        if (collide == 0) {
            //.1, .0707, .0293
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                entities[2].move(0, .1);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                entities[2].move(0, -.1);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                entities[2].move(-.1, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                entities[2].move(.1, 0);
            }

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::S))) {
                entities[2].move(-.0293,-.0293);
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::W))) {
                entities[2].move(-.0293, .0293);
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::S))) {
                entities[2].move(.0293, -.0293);
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::W))) {
                entities[2].move(.0293, .0293);
            }



            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                rot=rot-.003;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                rot=rot+.003;
            }
        }
        

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        {
            quit = true;
        }

        //////////////////////---------------------------


        //LOGIC///////////////------------------
        for (int i = 3; i < entities.size(); i++) {
            if (collision(entities[2], entities[i])) {
                collide = 250;
                entities.erase(entities.begin() + i );
                entities.push_back(Snake());
                snakeKills++;
                setNewPosition(entities[2],entities[entities.size()-1]);
                    
            }
            if (collision(entities[1], entities[i])) {
                entities.erase(entities.begin() + i);
                entities.push_back(Snake());
                snakeKills++;
                setNewPosition(entities[2], entities[entities.size() - 1]);

            }
                
            if (entities[i].movement == 0) {
                entities[i].movement = (rand() % 1000);
                entities[i].direction = (rand() % 9);
            }
            else {
                moving(entities[i]);
                entities[i].movement = entities[i].movement - 1;
            }
        }
        if (collide>0) {
            entities[2].setFillColor(sf::Color::Red);
            collide--;
        }
        else {
            entities[2].setFillColor(sf::Color::White);
            collide = 0;
        }
        for (int i = 2; i < entities.size(); i++) {
            wallCollision(entities[i]);
        }

        for (int i = 0; i < 3; i++) {
            texts[i].setPosition(10, 778 + 20 * i);
            texts[i].setFont(myFont);
        }

        std::string temp = std::to_string(snakeKills);
        std::string snakeString = "Snakes Killed: ";
        temp = snakeString + temp;
        texts[0].setString(temp);

        temp = std::to_string(sf::Mouse::getPosition().y);
        texts[1].setString(temp);

        entities[0].setPosition(entities[2].getPosition().x - entities[0].getRadius() + entities[2].getRadius(), entities[2].getPosition().y - entities[0].getRadius()+entities[2].getRadius());//light moves with player   
        entities[1].setPosition(entities[2].getPosition().x - entities[1].getRadius() + entities[2].getRadius() + (35* cos(rot)), entities[2].getPosition().y - entities[1].getRadius() + entities[2].getRadius()+(35 * sin(rot)));

        if (entities[2].health <= 0)
            quit = true;
        //////////////////////------------------


        //DRAW////////////////---------
        window.clear();

        window.draw(map1);
        for (int i = 2; i <entities.size(); i++) {
            window.draw(entities[i]);
        }
        window.draw(entities[0]);//light
        window.draw(entities[1]);//player

        window.draw(map2);
        window.draw(map3);

        for (int i = 0; i < texts.size(); i++) {
            window.draw(texts[i]);
        }
        window.display();
        //////////////////////---------
    }

    return 0;
}