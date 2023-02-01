/// <summary>
/// @author Peter Lowe
/// @date May 2019
///
/// you need to change the above lines or lose marks
/// </summary>

#include "Game.h"
#include <iostream>



/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the text 
/// load and setup thne image
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ 800U, 600U, 32U }, "SFML Game" },
	m_exitGame{false} //when true game will exit
{
	setupFontAndText(); // load font 
	setupSprite(); // load texture
}

/// <summary>
/// default destructor we didn't dynamically allocate anything
/// so we don't need to free it, but mthod needs to be here
/// </summary>
Game::~Game()
{
}


/// <summary>
/// main game loop
/// update 60 times per second,
/// process update as often as possible and at least 60 times per second
/// draw as often as possible but only updates are on time
/// if updates run slow then don't render frames
/// </summary>
void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame); //60 fps
		}
		render(); // as many as possible
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves etc. from OS
/// and user :: Don't do game update here
/// </summary>
void Game::processEvents()
{
	sf::Event newEvent;
	while (m_window.pollEvent(newEvent))
	{
		if ( sf::Event::Closed == newEvent.type) // window message
		{
			m_exitGame = true;
		}
		if (sf::Event::KeyPressed == newEvent.type) //user pressed a key
		{
			processKeys(newEvent);
		}
		if (sf::Event::MouseButtonReleased == newEvent.type)
		{
			if (canFly)
			{
				processMouseButtonUp(newEvent);
			}
		}
	}
}


/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(sf::Event t_event)
{
	if (sf::Keyboard::Escape == t_event.key.code)
	{
		m_exitGame = true;
	}
	if (sf::Keyboard::Space == t_event.key.code && fly % 2 == 0)
	{
		canFly = true;
		fly++;
	}
	else
	{
			canFly = false;
			fly++;
	}
	

}

void Game::processMouseButtonUp(sf::Event t_event)
{
	sf::Vector2f heading(0.0f, 0.0f);
	m_desiredPosition.x = t_event.mouseButton.x;
	m_desiredPosition.y = t_event.mouseButton.y;
	float magnitude = 0;
	if(m_heliPosition.x < m_desiredPosition.x)
	{
		m_direction = Direction::Right;
		m_helicopterSprite.setScale(1.0f, 1.0f);
		m_frameIncrement = 0.5f;
	}
	if(m_heliPosition.x > m_desiredPosition.x)
	{
		m_direction = Direction::Left;
		m_helicopterSprite.setScale(-1.0f, 1.0f);
		m_frameIncrement = 0.5f;
	}
	heading = m_desiredPosition - m_heliPosition;
	magnitude = std::sqrtf(heading.x * heading.x + heading.y * heading.y);
	heading = heading / magnitude;
	m_velocity = heading * 6.0f;
 }


/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}
	animateHeli();
	move();
}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::White);
	m_window.draw(m_welcomeMessage);
	m_window.draw(m_helicopterSprite);
	m_window.display();
}

/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupFontAndText()
{
	if (!m_ArialBlackfont.loadFromFile("ASSETS\\FONTS\\ariblk.ttf"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
	m_welcomeMessage.setFont(m_ArialBlackfont);
	m_welcomeMessage.setString("SFML Game");
	m_welcomeMessage.setStyle(sf::Text::Underlined | sf::Text::Italic | sf::Text::Bold);
	m_welcomeMessage.setPosition(40.0f, 40.0f);
	m_welcomeMessage.setCharacterSize(80U);
	m_welcomeMessage.setOutlineColor(sf::Color::Red);
	m_welcomeMessage.setFillColor(sf::Color::Black);
	m_welcomeMessage.setOutlineThickness(3.0f);

}

/// <summary>
/// load the texture and setup the sprite for the logo
/// </summary>
void Game::setupSprite()
{
	if (!m_helicopterTexture.loadFromFile("ASSETS\\IMAGES\\helicopter.png"))
	{
		// simple error message if previous call fails
		std::cout << "problem loading logo" << std::endl;
	}
	m_helicopterSprite.setTexture(m_helicopterTexture);
	m_helicopterSprite.setPosition(200.0f, 200.0f);
	m_helicopterSprite.setOrigin(90.0f, 32.0f);
	m_helicopterSprite.setTextureRect(sf::IntRect(0, 0, 180, 64));
}

void Game::animateHeli()
{
	if (canFly) 
	{
		int oldFrame = m_frameNo;
		m_frameValue += m_frameIncrement;
		m_frameNo = (int)m_frameValue;
		m_frameNo = m_frameNo % 4;
		if (oldFrame != m_frameNo)
		{
			m_helicopterSprite.setTextureRect(sf::IntRect(0, m_frameNo * 64, 180, 64));
		}
	}

}

void Game::move()
{
	m_heliPosition += m_velocity;
	m_helicopterSprite.setPosition(m_heliPosition);
	if (m_direction != Direction::None)
	{
		if (m_direction == Direction::Left)
		{
			if (m_heliPosition.x < m_desiredPosition.x)
			{
				m_direction = Direction::None;
				m_velocity = sf::Vector2f{ 0.0f,0.0f };
				m_frameIncrement = 0.25f;
			}
		}
		if (m_direction == Direction::Right)
		{
			if (m_heliPosition.x > m_desiredPosition.x)
			{
				m_direction = Direction::None;
				m_velocity = sf::Vector2f{ 0.0f,0.0f };
				m_frameIncrement = 0.25f;
			}
		}
		if (m_direction == Direction::Up)
		{
			if (m_heliPosition.y < m_desiredPosition.y)
			{
				m_direction = Direction::None;
				m_velocity = sf::Vector2f{ 0.0f,0.0f };
				m_frameIncrement = 0.25f;
			}
		}
		if (m_direction == Direction::Down)
		{
			if (m_heliPosition.y > m_desiredPosition.y)
			{
				m_direction = Direction::None;
				m_velocity = sf::Vector2f{ 0.0f,0.0f };
				m_frameIncrement = 0.25f;
			}
		}
	}
}

