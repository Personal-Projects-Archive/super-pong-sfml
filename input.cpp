#include "Input.h"
using namespace sf;

Input::Input()
{
    button.up = button.down = false;
}

Input::Button Input::GetButton() const { return button; }

void Input::HandleEvent(const Event& event, RenderWindow& window)
{
    event.visit([&](auto&& e)
        {
            using T = std::decay_t<decltype(e)>;

            if constexpr (std::is_same_v<T, Event::Closed>)
                window.close();

            else if constexpr (std::is_same_v<T, Event::KeyPressed>)
            {
                if (e.code == Keyboard::Key::Z) button.up = true;
                if (e.code == Keyboard::Key::S) button.down = true;
                if (e.code == Keyboard::Key::Escape) window.close();
            }
            else if constexpr (std::is_same_v<T, Event::KeyReleased>)
            {
                if (e.code == Keyboard::Key::Z) button.up = false;
                if (e.code == Keyboard::Key::S) button.down = false;
            }
        });
}

void Input::HandleRealtimeInput(Text&, RectangleShape& paddle)
{
    Vector2f pos = paddle.getPosition();

    if (button.up) pos.y -= 10.f;
    if (button.down) pos.y += 10.f;

    pos.y = std::clamp(pos.y, 0.f, 600.f - paddle.getSize().y);
    paddle.setPosition(pos);
}
