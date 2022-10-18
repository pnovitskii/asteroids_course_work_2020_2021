#define _USE_MATH_DEFINES
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include "Collision.h"
#include <fstream>
using namespace sf;
using namespace std;
unsigned int width = 1920; unsigned int height = 1080;
const int NUM = 30;
const int LIVES = 3;
int score = 0;
bool dead = false;
int destroyed = 0;
int aliveMiniStones = 0;
bool start = false;
bool endGame = false;
bool menu = false;
bool scoreMenu = false;
vector<Sprite> glyby;
vector<double> ugol;
vector<Sprite> hearts;
vector<Sprite> miniStones;
vector<Texture> textures;
struct Bullet
{
    Sprite sprite;
    Vector2f position;
    float angle;
    Bullet(Vector2f pos, float f_angle, Sprite f_sprite)
    {
        sprite = f_sprite;
        position = pos;
        angle = f_angle;
        sprite.setPosition(pos);
        sprite.setRotation(angle);
    }
};
struct Stone
{
    Sprite sprite;
    double angle = 0;
    bool isBig;
    int velocity;
    Stone(Sprite x)
    {
        sprite = x;
        sprite.setPosition(-2000, -2000);
    }
    int getVelocity()
    {
        return velocity;
    }
};
vector<Bullet> bullets;
vector<Stone> kamni;
void updateAsteroid(int i)
{
    kamni[i].sprite.setOrigin(60.f, 60.f);
    switch (rand() % 4 + 1)
    {
    case 1://NORTH
        kamni[i].sprite.setPosition(rand() % 1921, rand() % 151 - 200);
        kamni[i].sprite.setRotation(rand() % 181 + 90);
        break;
    case 2://EAST
        kamni[i].sprite.setPosition(rand() % 51 + 1970, rand() % 1081);
        kamni[i].sprite.setRotation(rand() % 181 + 180);
        break;
    case 3://SOUTH
        kamni[i].sprite.setPosition(rand() % 1921, rand() % 51 + 1130);
        kamni[i].sprite.setRotation(rand() % 181 - 90);
        break;
    case 4://WEST
        kamni[i].sprite.setPosition(rand() % 101 - 150, rand() % 1081);
        kamni[i].sprite.setRotation(rand() % 181);
        break;
    }
    kamni[i].angle = kamni[i].sprite.getRotation() * (M_PI / 180);
    kamni[i].sprite.setTexture(textures[rand() % 2]);
    kamni[i].velocity = rand() % 131 + 250;
}
void checkPos(Sprite& x)
{
    if (x.getPosition().x > 1970)
        x.setPosition(-50, x.getPosition().y);
    else if (x.getPosition().x < -50)
        x.setPosition(1970, x.getPosition().y);
    else if (x.getPosition().y < -50)
        x.setPosition(x.getPosition().x, 1130);
    else if (x.getPosition().y > 1130)
        x.setPosition(x.getPosition().x, -50);
}

//struct SoundEffects
//{
//    Music backGroundMusic;
//    SoundBuffer buffer1;
//    SoundBuffer buffer2;
//    Sound sound1;
//    Sound sound2;
//public:
//    SoundEffects()
//    {
//        backGroundMusic.openFromFile("sound/8-Bit_Music___Space_Police_.wav");
//        backGroundMusic.setVolume(10);
//        //backGroundMusic.play();
//        backGroundMusic.setLoop(true);
//
//        buffer1.loadFromFile("sound/Fire Gun.wav");
//        buffer2.loadFromFile("sound/Volcano Explosion.wav");
//        //buffer.loadFromFile("sound/Fire Gun.wav");
//        sound1.setBuffer(buffer1);
//        //buffer.loadFromFile("sound/Volcano Explosion.wav");
//        sound2.setBuffer(buffer2);
//        //sound1.openFromFile("sound/Fire Gun.wav");
//    }
//    void playBackGroundMusic()
//    {
//        backGroundMusic.play();
//    }
//    void playGunFireSound()
//    {
//        sound1.play();
//    }
//    void playExplosionSound()
//    {
//        sound2.play();
//    }
//};
struct StarShip
{
    Texture texture;
    Sprite sprite;
    double angle = 0;
    Vector2f velocity = { 0, 0 };
    StarShip()
    {
        texture.loadFromFile("graphics/starship.png");
        sprite.setTexture(texture);
        sprite.setTextureRect(IntRect(0, 0, 50, 80));
        sprite.setOrigin(25.f, 40.f);
        //sprite.setPosition(width / 2, height / 2);
        sprite.setPosition(4000, 4000);
    }
    void acceleration(Time time)
    {
        sprite.setTextureRect(IntRect(50, 0, 50, 80));
        angle = sprite.getRotation() * (M_PI / 180);
        velocity.x += sin(angle) * 400 * time.asSeconds();
        velocity.y -= cos(angle) * 400 * time.asSeconds();

    }
    void breaking()
    {
        sprite.setTextureRect(IntRect(0, 0, 50, 80));
    }
    void moveForward(Time time)
    {
        sprite.move(velocity * time.asSeconds());
        velocity.x *= 1 - time.asSeconds();
        velocity.y *= 1 - time.asSeconds();
        if (sprite.getPosition().x > 1970)
            sprite.setPosition(-50, sprite.getPosition().y);
        else if (sprite.getPosition().x < -50)
            sprite.setPosition(1970, sprite.getPosition().y);
        else if (sprite.getPosition().y < -50)
            sprite.setPosition(sprite.getPosition().x, 1130);
        else if (sprite.getPosition().y > 1130)
            sprite.setPosition(sprite.getPosition().x, -50);
    }
    void turnLeft(Time time)
    {
        sprite.rotate(-200.f * time.asSeconds());
    }
    void turnRight(Time time)
    {
        sprite.rotate(200.f * time.asSeconds());
    }
    Sprite getSprite()
    {
        return sprite;
    }

};
bool checkMouseCollision(Sprite x, Vector2i mousePos)
{
    if (mousePos.x < x.getPosition().x + 150 && \
        mousePos.x > x.getPosition().x - 150 && \
        mousePos.y > x.getPosition().y - 30 && \
        mousePos.y < x.getPosition().y + 30)
        return true;
    return false;
}
int main()
{
    srand(time(NULL));
    Vector2i mousePosition;
    bool isScore = false;
    bool isFile = false;
    int record = 0;
    fstream file("records.txt");
    vector<string> scores;
    if (!file.is_open())
    {
        cout << "Error while reading a file." << endl;
        return 0;
    }
    if (file.peek() == fstream::traits_type::eof())
    {
        isFile = true;
    }
    if (!isFile)
    {
        string str;
        getline(file, str);
        record = stoi(str);
        scores.push_back(str);
        while (getline(file, str))
            scores.push_back(str);
    }
    file.close();
    RenderWindow window(VideoMode(1920, 1080), "Asteroids - kursovaya rabota", Style::Fullscreen);
    //ContextSettings settings;
    //settings.antialiasingLevel = 16;
    Texture logoTexture;
    logoTexture.loadFromFile("graphics/logo.png");
    Sprite logoSprite(logoTexture);
    logoSprite.setOrigin(1927 / 2, 741 / 2);
    logoSprite.setRotation(12);
    logoSprite.setPosition(width / 2, height / 2);
    logoSprite.setScale(0.5, 0.5);
    Font font;
    font.loadFromFile("fonts/9303.ttf");
    Text startText;
    startText.setFont(font);
    startText.setString("Press ENTER to start");
    startText.setCharacterSize(55);
    FloatRect textRect = startText.getLocalBounds();
    startText.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
    startText.setPosition(width / 2, height / 2 + 180);
    Text message;
    message.setFont(font);
    message.setCharacterSize(55);
    Texture potatoTexture;
    //potatoTexture.loadFromFile("graphics/roflanGlyba.png");

    potatoTexture.loadFromFile("graphics/roflanGlyba.png");
    Texture appleTexture;
    appleTexture.loadFromFile("graphics/apple.png");
    textures.push_back(potatoTexture); textures.push_back(appleTexture);
    for (int i = 0; i < 9; i++)
    {
        Sprite stoneSprite;
        stoneSprite.setTexture(textures[rand() % 2]);
        kamni.push_back(stoneSprite);
    }

    for (int i = 0; i < 9; i++)
        updateAsteroid(i);
    Text scoreText;
    scoreText.setString("Score: 0");
    scoreText.setPosition(10, 100);
    scoreText.setFont(font);
    scoreText.setCharacterSize(50);
    int counterScore = 0;
    //SoundEffects audio;
    //audio.playBackGroundMusic();
    StarShip starship;
    Clock clock;
    Texture heartTexture;
    heartTexture.loadFromFile("graphics/heart.png");
    for (int i = 0; i < LIVES; i++)
    {
        Sprite heartSprite;
        heartSprite.setTexture(heartTexture);
        heartSprite.setScale(0.15, 0.15);
        heartSprite.setPosition(0 + i * 100, 0);
        hearts.push_back(heartSprite);
    }

    //Texture starshipTexture;
    //starshipTexture.loadFromFile("graphics/starship.png");
    //Sprite starshipSprite;
    //starshipSprite.setTexture(starshipTexture);
    //starshipSprite.setOrigin(25.f, 40.f);
    //starshipSprite.setPosition(-2000, -2000);
    //starshipTexture.setSmooth(true);

    Texture space;
    space.loadFromFile("graphics/space.jpg");
    Sprite spaceSprite;
    spaceSprite.setTexture(space);
    spaceSprite.setPosition(0, 0);

    Texture bulletTexture;
    bulletTexture.loadFromFile("graphics/bullet.png");
    Texture spotTexture;
    spotTexture.loadFromFile("graphics/spot.png");
    Sprite spotSprite(spotTexture);
    spotSprite.setOrigin(300, 300);
    spotSprite.setPosition(width / 2, height / 2);
    double shootTimer = 0;
    float textTimer = 0;
    float showTimer = 0;

    //Play Texture
    Texture startButtonTexture;
    startButtonTexture.loadFromFile("graphics/play2.png");
    Texture startButtonSelected;
    startButtonSelected.loadFromFile("graphics/play2selected.png");
    Sprite startButtonSprite;
    startButtonSprite.setTexture(startButtonTexture);
    startButtonSprite.setPosition(width / 2, height / 2);
    startButtonSprite.setOrigin(300 / 2, 60 / 2);

    //Scores Texture
    Texture scoresButtonTexture;
    scoresButtonTexture.loadFromFile("graphics/scoresButton.png");
    Sprite scoresButtonSprite;
    scoresButtonSprite.setTexture(scoresButtonTexture);
    scoresButtonSprite.setOrigin(300 / 2, 60 / 2);
    scoresButtonSprite.setPosition(width / 2, height / 2 + 80);
    Texture scoresButtonSelected;
    scoresButtonSelected.loadFromFile("graphics/scoresButtonSelected.png");

    //Quit Texture
    Texture quitButtonTexture;
    quitButtonTexture.loadFromFile("graphics/quitButton.png");
    Sprite quitButtonSprite;
    quitButtonSprite.setTexture(quitButtonTexture);
    quitButtonSprite.setOrigin(300 / 2, 60 / 2);
    quitButtonSprite.setPosition(width / 2, height / 2 + 160);
    Texture quitButtonSelected;
    quitButtonSelected.loadFromFile("graphics/quitButtonSelected.png");

    while (window.isOpen())//gameloop
    {
        cout << starship.sprite.getPosition().x << " " << starship.sprite.getPosition().y << endl;
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        //CLOSE WINDOW WITH ESC
        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();
        Time time = clock.restart();

        //starshipSprite.setTextureRect(IntRect(0, 0, 50, 80));
        if (!start && !menu && Keyboard::isKeyPressed(Keyboard::Enter))
        {
            menu = true;
            logoSprite.setPosition(width / 2, height / 2 - 150);
        }
        else if (menu)
        {
            mousePosition = Mouse::getPosition();
            if (checkMouseCollision(startButtonSprite, mousePosition))
            {
                startButtonSprite.setTexture(startButtonSelected);
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    //start = true;
                    menu = false;
                    //logoSprite.setPosition(logoSprite.getPosition().x, logoSprite.getPosition().y - 140);
                    logoSprite.setPosition(width / 2, height / 2 - 50);
                    start = true;
                    starship.sprite.setPosition(960, 540);
                    kamni.clear();
                    for (int i = 0; i < 3; i++)
                    {
                        Sprite stoneSprite;
                        stoneSprite.setTexture(textures[rand() % 2]);
                        kamni.push_back(stoneSprite);
                        kamni[kamni.size() - 1].isBig = true;
                    }

                    for (int i = 0; i < 3; i++)
                        updateAsteroid(i);
                }
            }
            else
                startButtonSprite.setTexture(startButtonTexture);
            if (checkMouseCollision(scoresButtonSprite, mousePosition) && !isScore)
            {
                scoresButtonSprite.setTexture(scoresButtonSelected);
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    menu = false;
                    isScore = true;
                    stringstream stream;
                    if (isFile)
                    {
                        stream << "Score table is empty." << endl;
                    }
                    else {
                        stream << "Best score : " << scores[0] << "\n\n";
                        for (int i = 1; i < scores.size(); i++)
                        {
                            stream << "\t\t\t " << i << " \t\t\t " << scores[i] << endl;
                        }
                        stream << "\nPress ENTER to continue" << endl;
                    }
                    message.setString(stream.str());
                    textRect = message.getLocalBounds();
                    message.setOrigin((textRect.left + textRect.width) / 2.f, (textRect.top + textRect.height) / 2);
                    message.setPosition(width / 2, height / 2);
                }
            }
            else if (isScore && Keyboard::isKeyPressed(Keyboard::Enter))
            {
                isScore = false;
                menu = true;
            }
            else
                scoresButtonSprite.setTexture(scoresButtonTexture);
            if (checkMouseCollision(quitButtonSprite, mousePosition))
            {
                quitButtonSprite.setTexture(quitButtonSelected);
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    file.open("records.txt");
                    if (!file.is_open())
                    {
                        cout << "Error! No file" << endl;
                        return 0;
                    }
                    if (!isFile)
                    {
                        for (auto x : scores)
                            file << x << endl;
                    }
                    file.close();
                    window.close();
                }
            }
            else
                quitButtonSprite.setTexture(quitButtonTexture);
        }
        if (endGame && Keyboard::isKeyPressed(Keyboard::Enter))
        {
            logoSprite.setPosition(width / 2, height / 2);
            endGame = false;
        }
        //MOVING FORWARD OF THE SHIP
        if (Keyboard::isKeyPressed(Keyboard::W) && start && !dead)
            starship.acceleration(time);
        else if (!dead && start)
            starship.breaking();
        if (!menu && start)
            starship.moveForward(time);
        //if (!dead && start)
            //checkPos(starship.sprite);
        //ROTATING OF THE SHIP
        if (Keyboard::isKeyPressed(Keyboard::A) && start)
            starship.turnLeft(time);
        //starshipSprite.rotate(-320.f * time.asSeconds());
        if (Keyboard::isKeyPressed(Keyboard::D) && start)
            starship.turnRight(time);
        //starshipSprite.rotate(320.f * time.asSeconds());

        //SHOOT TIMER Mouse::isButtonPressed(Mouse::Left)
        if (shootTimer < 5 && !(Keyboard::isKeyPressed(Keyboard::Space)))//isKeyPressed(Mouse::Left)))
            shootTimer += 300 * time.asSeconds();

        //SHOOTING Keyboard::isKeyPressed(Keyboard::Space) Mouse::isButtonPressed(Mouse::Left)
        if (shootTimer >= 5 && Keyboard::isKeyPressed(Keyboard::Space) && start)
        {
            //fireGunSound.play();
            //audio.playGunFireSound();
            //fireGunSound.play();
            //audio.sounds1.play();
            Sprite bulletSprite(bulletTexture);
            bulletSprite.setOrigin(7.f, 7.f);
            float angle = starship.sprite.getRotation() * (M_PI / 180);
            Vector2f pos(starship.sprite.getPosition().x + (50 * sin(angle)), starship.sprite.getPosition().y + (-50 * cos(angle)));
            bullets.push_back(Bullet(pos, angle, bulletSprite));
            shootTimer = 0;
        }

        //MOVING OF STONES
        for (int i = 0; i < kamni.size(); i++) {
            int v = kamni[i].getVelocity();
            kamni[i].sprite.rotate(30.f * time.asSeconds());
            kamni[i].sprite.move(-v * time.asSeconds() * cos(kamni[i].angle), -v * time.asSeconds() * sin(kamni[i].angle));
            if (kamni[i].sprite.getPosition().y > height + 400 || kamni[i].sprite.getPosition().y < -400 \
                || kamni[i].sprite.getPosition().x > width + 400 || kamni[i].sprite.getPosition().x < -400)
                updateAsteroid(i);
            //if (kamni[i].sprite.getPosition().y > 1355 || kamni[i].sprite.getPosition().y < -355 \
                || kamni[i].sprite.getPosition().x > 2050 || kamni[i].sprite.getPosition().x < -130)
                //updateAsteroid(i);
        }

        //COLLISION SHIP WITH STONES
        for (int i = 0; i < kamni.size(); i++)
        {
            if (Collision::PixelPerfectTest(starship.sprite, kamni[i].sprite, Uint8())) {
                //audio.playExplosionSound();
                updateAsteroid(i);
                hearts.pop_back();
                starship.sprite.setPosition(4000, 4000);
                dead = true;
                break;
            }
        }

        if (dead && start && hearts.size() != 0)
        {
            bool flag = false;
            for (int i = 0; i < kamni.size(); i++)
            {
                if (Collision::PixelPerfectTest(spotSprite, kamni[i].sprite, Uint8()))
                    flag = true;
            }
            for (int i = 0; i < miniStones.size(); i++)
            {
                if (Collision::PixelPerfectTest(spotSprite, miniStones[i], Uint8()))
                    flag = true;
            }
            if (!flag)
            {
                starship.sprite.setPosition(width / 2, height / 2);
                starship.sprite.setRotation(0);
                starship.velocity = { 0, 0 };
                dead = false;
            }

        }
        //END GAME WHEN HEARTS == 0
        if (hearts.size() == 0)
        {
            endGame = true;
            //logoSprite.setPosition(-1000, -1000);
            stringstream stream;
            stream << "\t   GAME OVER\n\nYour score is " << score << ".";
            message.setString(stream.str());
            textRect = message.getLocalBounds();
            message.setOrigin((textRect.left + textRect.width) / 2.f, (textRect.top + textRect.height) / 2);
            message.setPosition(width / 2, height / 2);
            if (isFile)
            {
                scores.push_back(to_string(score));
                scores.push_back(to_string(score));
                record = score;
                isFile = false;
            }
            else if (score >= record)
            {
                if (scores.size() > 10)
                    scores.pop_back();
                scores[0] = to_string(score);
                scores.insert(scores.begin() + 1, to_string(score));
                record = score;
            }
            else if (score < record)
            {
                if (scores.size() > 10)
                    scores.pop_back();
                scores.insert(scores.begin() + 1, to_string(score));
            }
            logoSprite.setPosition(width / 2, height / 2);
            score = 0;
            counterScore = 0;
            miniStones.clear();
            kamni.clear();
            for (int i = 0; i < LIVES; i++)
            {
                Sprite heartSprite;
                heartSprite.setTexture(heartTexture);
                heartSprite.setScale(0.15, 0.15);
                heartSprite.setPosition(0 + i * 100, 0);
                hearts.push_back(heartSprite);
            }
            for (int i = 0; i < 9; i++)
            {
                Sprite stoneSprite;
                stoneSprite.setTexture(textures[rand() % 2]);
                kamni.push_back(stoneSprite);
            }

            for (int i = 0; i < 9; i++)
                updateAsteroid(i);
            start = false;
        }

        //MOVING OF BULLETS
        for (int i = 0; i < bullets.size(); i++)
        {
            bullets[i].sprite.move(650.f * time.asSeconds() * sin(bullets[i].angle), -650.f * time.asSeconds() * cos(bullets[i].angle));
            if (bullets[i].sprite.getPosition().x > 1980 || bullets[i].sprite.getPosition().x < 0 || bullets[i].sprite.getPosition().y > 1080 || bullets[i].sprite.getPosition().y < 0)
            {
                bullets.erase(bullets.begin() + i);
                bullets.shrink_to_fit();
            }
        }

        //COLLISION STONE WITH BULLET
        for (int i = 0; i < kamni.size(); i++) {
            for (int j = 0; j < bullets.size(); j++) {
                if (Collision::PixelPerfectTest(kamni[i].sprite, bullets[j].sprite, Uint8())) {

                    if (kamni[i].isBig == true) {
                        bullets.erase(bullets.begin() + j);
                        //Sprite miniStoneSprite;
                        //miniStoneSprite.setTexture();
                        //miniStoneSprite.setScale(0.5, 0.5);
                        //miniStoneSprite.setRotation(kamni[i].angle);
                        //miniStoneSprite.rotate(45.f);
                        //miniStones.push_back(miniStoneSprite);
                        kamni.push_back(kamni[i].sprite);
                        kamni[kamni.size() - 1].sprite.setScale(0.5, 0.5);
                        kamni[kamni.size() - 1].isBig = false;
                        kamni[kamni.size() - 1].sprite.setPosition(kamni[i].sprite.getPosition());
                        kamni[kamni.size() - 1].sprite.setRotation(kamni[i].angle / (M_PI / 180) + (rand() % 31 + 25));
                        kamni[kamni.size() - 1].angle = kamni[kamni.size() - 1].angle = kamni[kamni.size() - 1].sprite.getRotation() * (M_PI / 180);
                        kamni[kamni.size() - 1].velocity = kamni[i].getVelocity() + 10;

                        //miniStones[miniStones.size() - 1].setPosition(kamni[i].sprite.getPosition());
                        //miniStones[miniStones.size() - 1].setRotation(kamni[i].angle / (M_PI / 180) + 45.f);
                        kamni.push_back(kamni[i].sprite);
                        kamni[kamni.size() - 1].sprite.setScale(0.5, 0.5);
                        kamni[kamni.size() - 1].isBig = false;
                        kamni[kamni.size() - 1].sprite.setPosition(kamni[i].sprite.getPosition());

                        kamni[kamni.size() - 1].sprite.setRotation(kamni[i].angle / (M_PI / 180) - (rand() % 31 + 25));
                        kamni[kamni.size() - 1].angle = kamni[kamni.size() - 1].angle = kamni[kamni.size() - 1].sprite.getRotation() * (M_PI / 180);
                        kamni[kamni.size() - 1].velocity = kamni[i].getVelocity() + 10;
                        //miniStones.push_back(miniStoneSprite);
                        //miniStones[miniStones.size() - 1].setRotation(kamni[i].angle / (M_PI / 180) - 45.f);
                        //miniStones[miniStones.size() - 1].setPosition(kamni[i].sprite.getPosition());

                        //updateAsteroid(i);

                        kamni.erase(kamni.begin() + i);
                        destroyed++;
                        aliveMiniStones += 2;
                        score += 10;
                        counterScore += 10;
                    }
                    else {
                        score += 15;
                        counterScore += 15;
                        bullets.erase(bullets.begin() + j);
                        kamni.erase(kamni.begin() + i);
                        aliveMiniStones--;
                    }
                }
            }
        }
        if (counterScore > 50 && aliveMiniStones == 0 || kamni.size() == 0)
        {
            for (int i = 0; i < rand() % 5 + destroyed; i++)
            {
                Sprite stoneSprite;
                stoneSprite.setTexture(textures[rand() % 2]);
                kamni.push_back(stoneSprite);
                kamni[kamni.size() - 1].isBig = true;
                //updateAsteroid(kamni.size() - 1);
            }
            aliveMiniStones = 0;
            destroyed = 0;
            counterScore = 0;
        }
        for (int i = 0; i < miniStones.size(); i++)
        {
            double angle = miniStones[i].getRotation() * (M_PI / 180);
            miniStones[i].move(-320.f * time.asSeconds() * cos(angle), -320.f * time.asSeconds() * sin(angle));
            checkPos(miniStones[i]);
        }
        for (int i = 0; i < miniStones.size(); i++)
        {
            for (int j = 0; j < bullets.size(); j++)
            {
                if (Collision::PixelPerfectTest(miniStones[i], bullets[j].sprite, Uint8()))
                {
                    score += 15;
                    counterScore += 15;
                    bullets.erase(bullets.begin() + j);
                    miniStones.erase(miniStones.begin() + i);
                }
            }
        }

        stringstream scorestream;
        scorestream << "Score: " << score;
        scoreText.setString(scorestream.str());
        window.clear();
        window.draw(spaceSprite);
        //window.draw(starshipSprite);
        window.draw(starship.getSprite());
        for (int i = 0; i < kamni.size(); i++)
            window.draw(kamni[i].sprite);
        for (auto x : bullets)
            window.draw(x.sprite);
        for (int i = 0; i < miniStones.size(); i++)
            window.draw(miniStones[i]);
        if (start) {
            for (int i = 0; i < hearts.size(); i++)
                window.draw(hearts[i]);
            window.draw(scoreText);
        }
        textTimer += time.asSeconds();
        if ((!start && !endGame && !isScore) || menu)
        {
            window.draw(logoSprite);

        }
        if ((!start && !menu && !isScore) || endGame) {
            if (textTimer > 0.5f) {
                window.draw(startText);
                showTimer += time.asSeconds();
                if (showTimer > 0.5f)
                {
                    textTimer = 0;
                    showTimer = 0;
                }
            }
        }
        if (menu)
        {
            window.draw(startButtonSprite);
            window.draw(scoresButtonSprite);
            window.draw(quitButtonSprite);
        }
        if (endGame || isScore)
            window.draw(message);
        window.display();
    }
    window.display();
    return 0;
}
