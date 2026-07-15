#include "main.h"
#include <cmath>
#include <vector>

bool LoadFont(Font& font);
Text makeText(const Font& font, String str, unsigned size, Vector2f pos);

RenderWindow window;
Input input;

enum GameState { MENU, PLAYING, GAME_OVER };
enum Mode { NONE, VS_BOT, VS_PLAYER };

struct Particle
{
    CircleShape shape;
    float life;
};

std::vector<Particle> particles;

bool checkCollision(const FloatRect& a, const FloatRect& b)
{
    return (a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x &&
        a.position.y < b.position.y + b.size.y &&
        a.position.y + a.size.y > b.position.y);
}

void spawnParticles(Vector2f pos)
{
    for (int i = 0; i < 8; i++)
    {
        Particle p;
        p.shape = CircleShape(3.f);
        p.shape.setFillColor(Color::Yellow);
        p.shape.setPosition(pos);
        p.life = 0.4f;
        particles.push_back(p);
    }
}

int main()
{
    window.create(VideoMode({ WIN_WIDTH, WIN_HEIGHT }), "SUPER PONG");
    window.setVerticalSyncEnabled(true);

    GameState state = MENU;
    Mode mode = NONE;

    RectangleShape midLine({ 4.f, (float)WIN_HEIGHT });
    midLine.setPosition({ WIN_WIDTH / 2.f - 2.f, 0 });
    midLine.setFillColor(Color(80, 80, 80));

    RectangleShape leftPaddle({ 20.f, 120.f });
    leftPaddle.setPosition({ 40.f, 240.f });

    RectangleShape rightPaddle({ 20.f, 120.f });
    rightPaddle.setPosition({ WIN_WIDTH - 60.f, 240.f });

    CircleShape ball(10.f);
    ball.setFillColor(Color::White);
    ball.setPosition({ WIN_WIDTH / 2.f, WIN_HEIGHT / 2.f });

    Vector2f ballVel(6.f, 4.f);
    float speedMult = 1.f;

    Font font;
    if (!LoadFont(font)) return -1;

    Text title = makeText(font, "SUPER PONG", 60, { 200,120 });
    Text menu = makeText(font, "1 : VS BOT\n2 : VS PLAYER\nSPACE : START", 26, { 260,320 });
    Text scoreTxt = makeText(font, "", 28, { 260,20 });
    Text gameOverTxt = makeText(font, "", 40, { 200,280 });

    int scoreL = 0, scoreR = 0;
    const int MAX_SCORE = 11;

    while (window.isOpen())
    {
        while (const optional<Event> event = window.pollEvent())
        {
            input.HandleEvent(*event, window);

            event->visit([&](auto&& e)
                {
                    using T = decay_t<decltype(e)>;
                    if constexpr (is_same_v<T, Event::KeyPressed>)
                    {
                        if (state == MENU)
                        {
                            if (e.code == Keyboard::Key::Num1) mode = VS_BOT;
                            if (e.code == Keyboard::Key::Num2) mode = VS_PLAYER;
                            if (e.code == Keyboard::Key::Space && mode != NONE)
                                state = PLAYING;
                        }
                    }
                });
        }

        if (state == PLAYING)
        {
            input.HandleRealtimeInput(scoreTxt, leftPaddle);

            // Joueur 2
            Vector2f rPos = rightPaddle.getPosition();
            if (mode == VS_PLAYER)
            {
                if (Keyboard::isKeyPressed(Keyboard::Key::Up)) rPos.y -= 10.f;
                if (Keyboard::isKeyPressed(Keyboard::Key::Down)) rPos.y += 10.f;
            }
            else
            {
                float target = ball.getPosition().y;
                if (fabs(target - rPos.y) > 20)
                    rPos.y += (target > rPos.y ? 5.f : -5.f);
            }

            rPos.y = std::clamp(rPos.y, 0.f, WIN_HEIGHT - rightPaddle.getSize().y);
            rightPaddle.setPosition(rPos);

            ball.move(ballVel * speedMult);

            if (ball.getPosition().y <= 0 || ball.getPosition().y + 20 >= WIN_HEIGHT)
                ballVel.y = -ballVel.y;

            auto bounce = [&](RectangleShape& paddle)
                {
                    if (checkCollision(ball.getGlobalBounds(), paddle.getGlobalBounds()))
                    {
                        float center = paddle.getPosition().y + paddle.getSize().y / 2;
                        float diff = (ball.getPosition().y + 10) - center;
                        ballVel.x = -ballVel.x;
                        ballVel.y = diff * 0.15f;
                        spawnParticles(ball.getPosition());
                    }
                };

            bounce(leftPaddle);
            bounce(rightPaddle);

            if (ball.getPosition().x < 0) { scoreR++; ball.setPosition({ 400,300 }); }
            if (ball.getPosition().x > WIN_WIDTH) { scoreL++; ball.setPosition({ 400,300 }); }

            scoreTxt.setString(
                (mode == VS_PLAYER ? "PLAYER 1 " : "PLAYER ") + std::to_string(scoreL) +
                "  |  " +
                (mode == VS_PLAYER ? "PLAYER 2 " : "CPU ") + std::to_string(scoreR)
            );

            if (scoreL >= MAX_SCORE || scoreR >= MAX_SCORE)
            {
                state = GAME_OVER;
                gameOverTxt.setString("GAME OVER\nPRESS ESC");
            }
        }

        for (auto& p : particles)
        {
            p.life -= 0.02f;
            p.shape.move({ rand() % 5 - 2.f, rand() % 5 - 2.f });    
        }
        particles.erase(remove_if(particles.begin(), particles.end(),
            [](Particle& p) { return p.life <= 0; }), particles.end());

        window.clear(Color(15, 15, 25));

        if (state == MENU)
        {
            window.draw(title);
            window.draw(menu);
        }
        else
        {
            window.draw(midLine);
            window.draw(leftPaddle);
            window.draw(rightPaddle);
            window.draw(ball);
            window.draw(scoreTxt);
        }

        if (state == GAME_OVER) window.draw(gameOverTxt);
        for (auto& p : particles) window.draw(p.shape);

        window.display();
    }
}



bool LoadFont(sf::Font& font)
{
    try
    {
        font = sf::Font("res/Poppins.ttf"); // SFML 3
    }
    catch (...)
    {
        std::cout << "Erreur chargement police\n";
        return false;
    }
    return true;
}
Text makeText(const sf::Font& font, sf::String str, unsigned size, sf::Vector2f pos)
{
    sf::Text t(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(sf::Color::White);
    t.setPosition(pos);
    return t;
}
