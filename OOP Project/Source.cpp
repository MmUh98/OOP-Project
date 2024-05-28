#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream> // For std::cerr

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PLAYER_SPEED = 400.0f;
const float FALL_SPEED = 200.0f;
const float SPAWN_INTERVAL = 1.0f; // seconds

class Player {
public:
    Player(const std::string& textureFile, float width, float height) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Error: Could not load player texture from file" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        sprite.setTexture(texture);
        sprite.setScale(width / sprite.getLocalBounds().width, height / sprite.getLocalBounds().height);
        sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - height - 10);
    }

    void move(float dx, float dt) {
        sprite.move(dx * dt, 0);
        if (sprite.getPosition().x < 0) {
            sprite.setPosition(0, sprite.getPosition().y);
        }
        if (sprite.getPosition().x + sprite.getGlobalBounds().width > WINDOW_WIDTH) {
            sprite.setPosition(WINDOW_WIDTH - sprite.getGlobalBounds().width, sprite.getPosition().y);
        }
    }

    sf::Sprite getSprite() const {
        return sprite;
    }

private:
    sf::Texture texture;
    sf::Sprite sprite;
};

class FallingObject {
public:
    FallingObject(const sf::Texture& texture, float speed)
        : speed(speed) {
        sprite.setTexture(texture);
        sprite.setPosition(static_cast<float>(std::rand() % (WINDOW_WIDTH - static_cast<int>(sprite.getGlobalBounds().width))), -sprite.getGlobalBounds().height);
    }

    void update(float dt) {
        sprite.move(0, speed * dt);
    }

    sf::Sprite getSprite() const {
        return sprite;
    }

    bool isOutOfWindow() const {
        return sprite.getPosition().y > WINDOW_HEIGHT;
    }

private:
    sf::Sprite sprite;
    float speed;
};

class Game {
public:
    Game()
        : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Avoid Falling Objects"),
        player("F:/OOP Project/images/player-jet.png", 50.0f, 50.0f), // Change this path to your player's image file
        score(0) {
        window.setFramerateLimit(60);
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        // Load texture for falling objects
        if (!fallingObjectTexture.loadFromFile("F:/OOP Project/images/donald-duck.png")) { // Change this path to your falling object's image file
            std::cerr << "Error: Could not load falling object texture from file" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Load texture for background
        if (!backgroundTexture.loadFromFile("F:/OOP Project/images/background.png")) { // Change this path to your background image file
            std::cerr << "Error: Could not load background texture from file" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Load font
        if (!font.loadFromFile("F:/OOP Project/fonts/Roboto-Bold.ttf")) { // Change this path to your font file
            std::cerr << "Error: Could not load font from file" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Setup score text
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        updateScoreText();
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    sf::RenderWindow window;
    Player player;
    std::vector<FallingObject> fallingObjects;
    sf::Clock spawnClock;
    sf::Clock gameClock;
    sf::Texture fallingObjectTexture;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Font font;
    sf::Text scoreText;
    int score;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    void update() {
        sf::Time deltaTime = gameClock.restart();
        float dt = deltaTime.asSeconds();

        // Handle player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.move(-PLAYER_SPEED, dt);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.move(PLAYER_SPEED, dt);
        }

        // Spawn new falling objects
        if (spawnClock.getElapsedTime().asSeconds() >= SPAWN_INTERVAL) {
            fallingObjects.emplace_back(fallingObjectTexture, FALL_SPEED);
            spawnClock.restart();
        }

        // Update falling objects
        for (auto& obj : fallingObjects) {
            obj.update(dt);
        }

        // Check for collisions and remove out-of-bound objects
        auto playerSprite = player.getSprite();
        fallingObjects.erase(
            std::remove_if(fallingObjects.begin(), fallingObjects.end(), [&playerSprite, this](FallingObject& obj) {
                if (playerSprite.getGlobalBounds().intersects(obj.getSprite().getGlobalBounds())) {
                    std::exit(0); // Game over
                }
                if (obj.isOutOfWindow()) {
                    score++; // Increment score when object goes out of the window
                    updateScoreText();
                    return true;
                }
                return false;
                }),
            fallingObjects.end()
        );
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite); // Draw background first
        window.draw(player.getSprite());
        for (const auto& obj : fallingObjects) {
            window.draw(obj.getSprite());
        }
        window.draw(scoreText);
        window.display();
    }

    void updateScoreText() {
        scoreText.setString("Score: " + std::to_string(score));
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
