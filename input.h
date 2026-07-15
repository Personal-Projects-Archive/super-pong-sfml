#ifndef INPUT_H
#define INPUT_H

#include <SFML/Graphics.hpp>

// Classe qui stocke l'état des boutons
class Input
{
public:
    // Structure interne qui mémorise l'état des touches
    struct Button
    {
        bool left;
        bool right;
        bool up;
        bool down;
        bool attack;
        bool escape;
    };

    // Constructeur
    Input();
    //Proto GetBtn
    Button GetButton(void) const;
    // Gestion des événements (une seule fois)
    void HandleEvent(const sf::Event& event, sf::RenderWindow& window);

    // Gestion des touches maintenues (fluide)
    void HandleRealtimeInput(sf::Text& txt, sf::RectangleShape& player);

private:
    Button button; // état interne des boutons
};

#endif
