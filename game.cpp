#include "game.hpp"
#include <cmath>
#include <iostream>

Game::Game():
    mWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), WINDOW_TITLE)
{
    mWindow.setFramerateLimit(FPS);
    loadTextures();
    loadFonts();
    startNewGame();
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    float delta = mTimePerFrame.asSeconds();
    while(mWindow.isOpen())
    {
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate > mTimePerFrame) {
            timeSinceLastUpdate -= mTimePerFrame;
            processEvents(delta);
            update(delta);
        }
        render();
    }
}

void Game::createSegments()
{
    mSegments.reserve(NUM_LINES);

    for(auto i = 0; i < NUM_LINES; ++i)
    {
        Segment segment;
        segment.z = i * SEGMENT_LENGTH;
        if(i >= 300 && i < 700)
        {
            segment.curvature = 0.5f;
        } else if(i > 1100){
            segment.curvature = -0.7;
        }
        if (i>750)
        {
            segment.y = mCameraHeight * sin(0.04f * i);
        }
        mSegments.push_back(segment);
    }
}

void Game::loadFonts()
{
    static const std::string pathToMainFont{ "resources/fonts/BRLNSR.TTF" };
    mFontManager.load("small", pathToMainFont);
}

void Game::processEvents(float frameTime)
{
    sf::Event event;
    while(mWindow.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            mWindow.close();
        }
        else if(event.type == sf::Event::MouseButtonReleased)
        {
            //For debugging only
            if(event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << " mx = " << event.mouseButton.x << " my = "
                          << event.mouseButton.y << std::endl;
            }
        }
    }

    mPlayer.speed = 0.0f;
    mPlayer.posX = 0.0f;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        mPlayer.posX -= 0.1f;
        std::cout << "Moving left" << std::endl;
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        mPlayer.posX += 0.1f;
        std::cout << "Moving right" << std::endl;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        mPlayer.speed = 200.0f;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        mPlayer.speed = -200.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) mCameraHeight += 100;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) mCameraHeight -= 100;
}

void Game::update(float frameTime)
{
    mCamera.reset();
    const int N = mSegments.size();
    mCamera.z += mPlayer.speed;
    while(mCamera.z >= N * SEGMENT_LENGTH) mCamera.z -= N * SEGMENT_LENGTH;
    while(mCamera.z < 0) mCamera.z += N * SEGMENT_LENGTH;
    mStartPos = mCamera.z / SEGMENT_LENGTH;
    mCamera.y = mSegments[mStartPos].y + mCameraHeight;

    if (mPlayer.speed > 0)
    {
        mSpriteBackground.move(-mSegments[mStartPos].curvature*2,0);
    }
    else if (mPlayer.speed < 0)
    {
        mSpriteBackground.move( mSegments[mStartPos].curvature*2,0);
    }
}

void Game::render()
{
    mWindow.clear(sf::Color::Black);
    mWindow.draw(mSpriteBackground);

    //Track rendering

    const int N = mSegments.size();
    int maxY = SCREEN_HEIGHT;

    for(int n = mStartPos; n < mStartPos + 300; ++n)
    {
        //std::cout << "n = " << n << " n%N = " << (n%N) << std::endl;
        Segment &currLine = mSegments[n%N]; // Mod operation keeps array index in bounds
        currLine.project(mPlayer.posX * TRACK_WIDTH - mCamera.x, mCamera.y,
        mStartPos * SEGMENT_LENGTH - (n >= N ? N * SEGMENT_LENGTH : 0));
        mCamera.x += mCamera.dx;
        mCamera.dx += currLine.curvature;

        if(currLine.Y >= maxY) continue;
        maxY = currLine.Y;

       // std::cout << "l.curvature = " << currLine.curvature << std::endl;

        //(n/5) - constant affects the width of alternating stripes
        sf::Color grass  = (n / 5) % 2 ? sf::Color(16,200,16) : sf::Color(0,154,0);
        sf::Color clip = (n / 5) % 2 ? sf::Color(255,255,255) : sf::Color(0,0,0);
        sf::Color track   = (n / 5) % 2 ? sf::Color(107,107,107) : sf::Color(105,105,105);

        Segment prevLine = mSegments[(n-1) % N]; //previous line
        //1.2 - clip width
        renderQuad(mWindow, 0, prevLine.Y, SCREEN_WIDTH, 0, currLine.Y,
                 SCREEN_WIDTH, grass);
        renderQuad(mWindow, prevLine.X, prevLine.Y, 1.2 * prevLine.W,
                 currLine.X, currLine.Y, 1.2 * currLine.W, clip);
        renderQuad(mWindow, prevLine.X, prevLine.Y, prevLine.W, currLine.X,
                 currLine.Y, currLine.W, track);
    }
    mWindow.display();
}

void Game::startNewGame()
{
    createSegments();
}

void Game::renderQuad(sf::RenderWindow &window, int nearMidPointX, int nearMidPointY,
                    int nearWidth, int farMidPointX, int farMidPointY, int farWidth,
                    sf::Color color)
{
    sf::ConvexShape shape(4);
    shape.setFillColor(color);
    shape.setPoint(0, sf::Vector2f(nearMidPointX - nearWidth, nearMidPointY));
    shape.setPoint(1, sf::Vector2f(farMidPointX - farWidth, farMidPointY));
    shape.setPoint(2, sf::Vector2f(farMidPointX + farWidth, farMidPointY));
    shape.setPoint(3, sf::Vector2f(nearMidPointX + nearWidth, nearMidPointY));
    window.draw(shape);
}

void Game::loadTextures()
{
    static const std::string pathToBackgroundImage{ "resources/images/bg.png" };
    static const std::string pathToCarImage{ "resources/images/cars.png" };
    mTextureManager.load(TextureID::Background, pathToBackgroundImage);
    mTextureManager.load(TextureID::Cars, pathToCarImage);

    mTextureManager.get(TextureID::Background).setRepeated(true);
    mTextureManager.get(TextureID::Background).setSmooth(true);
    mSpriteBackground.setTexture(mTextureManager.get(TextureID::Background));
    mSpriteBackground.setTextureRect(sf::IntRect(0,0,5000,411));
    mSpriteBackground.setPosition(-2000,0);

    mTextureManager.get(TextureID::Cars).setRepeated(true);
    mTextureManager.get(TextureID::Cars).setSmooth(true);
    mSpriteCar.setTexture(mTextureManager.get(TextureID::Cars));
    mSpriteCar.setTextureRect(sf::IntRect(264,144,31,23));

    float w = mSpriteCar.getGlobalBounds().width;
    float h = mSpriteCar.getGlobalBounds().height;
    mSpriteCar.setOrigin( w / 2, h / 2);
    mSpriteCar.scale(4.0f, 4.0f);
}

/*void Game::configureTextInfo()
{
    mGameInfo.setColor(sf::Color::Yellow);
    mGameInfo.setFont(mFont);
    mGameInfo.setCharacterSize(32);
    mGameInfo.setPosition(5, 12);
}*/

void Game::centralizeWindow()
{
    const int screenWidth = sf::VideoMode::getDesktopMode().width;
    const int screenHeight = sf::VideoMode::getDesktopMode().height;
    mWindow.setPosition(sf::Vector2i((screenWidth - SCREEN_WIDTH) / 2,
                                    (screenHeight - SCREEN_HEIGHT) / 2));
}
