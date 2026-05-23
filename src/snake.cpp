#include <SFML/Graphics.hpp>
#include<cstdlib>
#include<ctime>
#include<deque>
void runSnake(sf::RenderWindow& window) {

	const int CELL_SIZE = 20;
	auto size = window.getSize();
	const int COLS = size.x / CELL_SIZE;
	const int ROWS = size.y / CELL_SIZE;

	sf::RectangleShape rect({ 
		static_cast<float> (CELL_SIZE),
		static_cast<float>(CELL_SIZE)
		});

	rect.setFillColor(sf::Color::White);
	std::deque < sf::Vector2i> body;
	body.push_front({ 5,5 });
	
	sf::RectangleShape food(
		{ static_cast<float> (CELL_SIZE), static_cast<float>(CELL_SIZE) }
	);
	food.setFillColor(sf::Color::Red);
	sf::Font font;
	font.openFromFile("fonts/regular.ttf");
	sf::Text scoreText(font);
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition({ 100.f,100.f });

	sf::Clock clock;
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	sf::Vector2i foodPos(std::rand() % COLS, std::rand() % ROWS);

	float moveInterval = 0.2f;

	sf::Vector2i direction(1, 0);
	int score = 0;
	while (window.isOpen()) {
		while (auto e = window.pollEvent()) {

			if (e->is<sf::Event::Closed>())
				window.close();
			if (e->is<sf::Event::KeyPressed>())
			{
				auto key = e->getIf<sf::Event::KeyPressed>()->code;
				if (key == sf::Keyboard::Key::Escape)
					window.close();
			}
		}

		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			rect.move({ 0.f,-5.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			rect.move({ -5.f,0.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			rect.move({ 0.f,5.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			rect.move({ 5.f,0.f });*/

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			direction = { 0,-1 };
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			direction = { -1,0 };
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			direction = { 0,1 };
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			direction = { 1,0 };


		if (clock.getElapsedTime().asSeconds() >= moveInterval) {
			sf::Vector2i newHead = body.front() + direction;
			for (auto& segment : body) 
			{
				if (newHead == segment){
				window.close();
				}
			}
			body.push_front(newHead);
			body.pop_back();
			clock.restart();

			if (newHead.x < 0 || newHead.y < 0|| newHead.x>=COLS|| newHead.y>=ROWS)
				window.close();
			
			if (foodPos == newHead) {
				foodPos = { std::rand() % COLS, std::rand() % ROWS };
				body.push_back(body.back());
				score++;
			}
		}
		food.setPosition(
			{ static_cast<float>(foodPos.x * CELL_SIZE),static_cast<float>(foodPos.y * CELL_SIZE) }
		);
		
		window.clear(sf::Color(10, 10, 10));
		scoreText.setString("Score: " + std::to_string(score));
		window.draw(scoreText);
		window.draw(food);
		for (auto& segment : body) {
			rect.setPosition(
				{
				static_cast<float>(segment.x * CELL_SIZE),
				static_cast<float>(segment.y * CELL_SIZE)
				}
			);
			window.draw(rect);
		}

		

		window.display();
	}
}
int main() {
	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Snake", sf::State::Fullscreen);
		runSnake(window);
	return 0;
}