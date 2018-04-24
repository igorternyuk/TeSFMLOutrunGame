#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "resourcemanager.hpp"
#include <vector>
#include <string>

class Game
{
public:
    explicit Game();
    void run();
private:
    const std::string WINDOW_TITLE { "Outrun" };
    enum
    {
        SCREEN_WIDTH = 1024,
        SCREEN_HEIGHT = 768,
        TRACK_WIDTH = 2000,
        SEGMENT_LENGTH = 200,
        NUM_LINES = 1600,
        FPS = 60
    };

    sf::RenderWindow mWindow;
    sf::Time mTimePerFrame {sf::seconds(1.0f / FPS)};

    enum class TextureID
    {
        Background,
        Cars
    };

    ResourceManager<sf::Texture, TextureID> mTextureManager;
    ResourceManager<sf::Font, std::string> mFontManager;
    ResourceManager<sf::SoundBuffer, std::string> mSoundManager;
    sf::Sprite mSpriteBackground;
    sf::Sprite mSpriteCar;

    struct Player
    {
        float posX;
        float speed;
        float curvature;
        Player()
        {
            posX = 0.0f;
            speed = 0.0f;
            curvature = 0.0f;
        }
    };

    //const float mCameraDepth = 0.84f;
    int mCameraHeight = 1500;

    struct Camera
    {
        float x, y, z, dx;
        Camera(){ x = y = z = dx = 0; }
        void reset() { x = y = dx = 0; }
    };

    struct Segment
    {
        float x, y, z; //line center
        float X, Y, W; //screen coord
        float curvature, scale;

        Segment() { curvature = x = y = z = 0; }

        void project(int camX, int camY, int camZ)
        {
            scale = 0.84f / (z - camZ);
            X = (1 + scale * (x - camX)) * SCREEN_WIDTH / 2;
            Y = (1 - scale * (y - camY)) * SCREEN_HEIGHT / 2;
            W = scale * TRACK_WIDTH * SCREEN_WIDTH / 2;
        }
    };


    Camera mCamera;
    Player mPlayer;
    std::vector<Segment> mSegments;
    int mStartPos;

    void processEvents(float frameTime);
    void update(float frameTime);
    void render();

    void startNewGame();
    void createSegments();
    void loadFonts();
    void loadTextures();
    void configureTextInfo();
    void centralizeWindow();

    void renderQuad(sf::RenderWindow &window, int nearMidPointX, int nearMidPointY,
                  int nearWidth, int farMidPointX, int farMidPointY, int farWidth,
                  sf::Color color = {0,255,0});
};
