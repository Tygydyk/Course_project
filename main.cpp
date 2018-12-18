#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <sstream>
#include <iostream>

using namespace sf;

class Player {
private:
	float x, y = 0;
public:
	float w, h, dx, dy, speed = 0; // координаты x и y игрока, ширина и высота, ускорения по осям, скорость
	int dir, playerScore; // направление перемещения
	String File; // файл с расширением
	Image image; // sfml-изображение
	Texture texture;
	Sprite sprite;
	Player(String F, int X, int Y, float W, float H) { // Конструктор с параметрами. При создании объекта ему будут задаваться переданные данные.
		dir = 0; playerScore = 0;
		File = F;
		w = W; h = H;
		image.loadFromFile("image/" + File); // закидываем в image наше изображение. 
		for (int i = 210; i < 256; i++) {
			image.createMaskFromColor(Color(i, i, i));
		}
		texture.loadFromImage(image); // закидываем изображение в текстуру.
		sprite.setTexture(texture); // заливаем спрайт текстурой.
		x = X; y = Y;
		sprite.setTextureRect(IntRect(30, 30, w, h)); // задаём спрайту один прямоугольник для вывода одного рыцаря
	}
	void update(float time) { // принимает в себя время sfml, поэтому работает бесконечно
		switch (dir) {
		case 0: dx = speed; dy = 0; break; // по иксу задаём положительную скорость, игреку зануляем. Следовательно персонаж идёт только вправо.
		case 1: dx = -speed; dy = 0; break;
		case 2: dx = 0; dy = speed; break; // вниз
		case 3: dx = 0; dy = -speed; break; 
		}

		x += dx * time; // ускорение на время = смещение координат.
		y += dy * time;

		speed = 0; // после того, как я отпущу клавишу, мой персонаж остановится. Иначе он будет идти дальше.
		sprite.setPosition(x, y);  // выводим спрайт в позицию x и y. Бесконечно выводим, иначе бы спрайт стоял на месте.
		interactionWithMap();
	}

	float getPlayerCoordinateX() {	//этим методом будем забирать координату Х	
		return x;
	}
	float getPlayerCoordinateY() {	//этим методом будем забирать координату Y 	
		return y;
	}

	void interactionWithMap() { // функция взаимодействия с картой
		for (int i = y/40; i < (y + h) / 40; i++)
			for (int j = x / 40; j < (x + w) / 40; j++) { // икс делим на 40, т.е. получаем левый квадратик, с которым персонаж соприкасается
				if (TileMap[i][j] == '0') { // если квадратик соответствует символу '0', то проверяем направление скорости
					if (dy > 0) {// если шли вниз
						y = i * 40 - h;
					}
					if (dy < 0) {
						y = i * 40 + 40;
					}
					if (dx > 0) {
						x = j * 40 - w;
					}
					if (dx < 0) {
						x = j * 40 + 40;
					}
				}
				if (TileMap[i][j] == 's') {
					playerScore++;
					TileMap[i][j] = ' ';
				}
			}
	}
};

int main()

{

	RenderWindow window(VideoMode(1400, 800), "SFMLworks");
	view.reset(sf::FloatRect(0, 0, 1400, 800)); // Что-то вроде инициализации камеры.

	Font font; // шрифт
	font.loadFromFile("CyrilicOld.TTF");
	Text text("", font, 20); // создаём объект текст, закидываем в него строку, размер, шрифт.
	text.setFillColor(Color::Red); // присваиваем ему красный цвет
	text.setStyle(Text::Bold); // делаем текст жирным

	Image map_image;//объект изображения для карты
	map_image.loadFromFile("image/map.jpg");//загружаем файл для карты
	Texture map;//текстура карты
	map.loadFromImage(map_image);//заряжаем текстуру картинкой
	Sprite s_map;//создаём спрайт для карты
	s_map.setTexture(map);//заливаем текстуру спрайтом

	Player P( "walkingsprite.jpg", 50, 600, 144, 160);

	float CurrentFrame = 0;
	Clock clock; // создаём переменную времени, т.е. привязка ко времени, а не к мощности процессора

	while (window.isOpen())

	{
		float time = clock.getElapsedTime().asMicroseconds(); // дать прошедшее время в микросекундах
		clock.restart(); // перезагружает время
		time = time / 800;

		Event event;

		while (window.pollEvent(event))

		{

			if (event.type == Event::Closed)

				window.close();

		}

		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			P.dir = 1; P.speed = 0.1;//dir =1 - направление вверх, speed =0.1 - скорость движения. Заметьте - время мы уже здесь ни на что не умножаем и нигде не используем каждый раз
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 9) CurrentFrame -= 9;
			P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 205, 144, 165)); //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
		}

		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			P.dir = 0; P.speed = 0.1;//направление вправо, см выше
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 9) CurrentFrame -= 9;
			P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 25, 144, 165)); //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
		}

		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			P.dir = 3; P.speed = 0.1;//направление вниз, см выше
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 9) CurrentFrame -= 9;
			P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 555, 144, 165)); //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
		}

		if (Keyboard::isKeyPressed(Keyboard::Down)) { //если нажата клавиша стрелка влево или англ буква А
			P.dir = 2; P.speed = 0.1;//направление вверх, см выше
			CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
			if (CurrentFrame > 9) CurrentFrame -= 9; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
			P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 380, 144, 165)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
			// передаём координаты персонажа в функцию управления камерой.
		}

		getPlayerCoordinateforView(P.getPlayerCoordinateX(), P.getPlayerCoordinateY());
		P.update(time);//оживляем объект P класса Player с помощью времени sfml, передавая время в качестве параметра функции update. благодаря этому персонаж может двигаться.
		viewmap(time); // функция скролинга карты
		changeview();
		window.setView(view); // оживляет камеру в окне sfml
		window.clear();
		/////////////////////////////Рисуем карту/////////////////////
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDHT_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 40, 40)); //если встретили символ пробел, то рисуем 1й квадратик
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(568, 47, 40, 40));//если встретили символ s, то рисуем 2й квадратик
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(102, 173, 40, 40));//если встретили символ 0, то рисуем 3й квадратик


				s_map.setPosition(j * 40, i * 40);//по сути раскидывает квадратики, превращая в карту. то есть задает каждому из них позицию. если убрать, то вся карта нарисуется в одном квадрате 32*32 и мы увидим один квадрат

				window.draw(s_map);//рисуем квадратики на экран
			}
		std::ostringstream playerScoreString; // объявление переменной
		playerScoreString << P.playerScore; // формируем строку
		text.setString("Грибы: " + playerScoreString.str()); // задаёт строку тексту и вызываем уже сформированную строку методом .str().
		text.setPosition(view.getCenter().x + 510, view.getCenter().y - 350);
		window.draw(text); // рисуем текст
		window.draw(P.sprite);//рисуем спрайт объекта P класса player
		window.display();

	}

	return 0;

}
