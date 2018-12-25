#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <sstream>
#include "mission.h"
#include <iostream>
#include "level.h"
#include "tinyxml.h"

using namespace sf;

class Entity {
public:
	std::vector <Object> obj; // вектор объектов моей карты, поскольку все потомки этого класса будут взаимодействовать с объектами
	float x, y, dx, dy, speed, moveTimer; // moveTimer для будущих целей
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name; // Чтобы не создавать для каждого вида врага отдельный класс, я буду различать их по имени. Они будут иметь разные скорость, жизни и т.д.
	Entity(Image &image, String Name,float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image); // передаю изображение по ссылке, так как врагов будет несколько, и чтобы для каждого не передавать одно и то же изображение, 
		sprite.setTexture(texture); // оно будет передано один раз и не будет забивать память своими копиями. 
		sprite.setOrigin(w / 2, h / 2); // центр персонажа - его геометрическая середина
	}

	FloatRect getRect() { // функция для получения габаритов прямоугольника
		return FloatRect(x, y, w, h); // проверяет столкновения
	}
};

class Player : public Entity { /* вещи, которые в качестве параметров передаются в класс игрока, передаются в конструтор Entity, 
							   и уже в нём происходит вся работа(инициализация и пр.), тем самым избегая в наследниках лишней инициализации и строк кода. */
public:
	int playerScore; // очки - уникальная переменная, которая может быть только у игрока.
	enum StateObject { left, right, up, down, jump, stay}; // перечисление состояний объекта
	StateObject state;
	Player(Image &image, String Name, Level &level, float X, float Y, int W, int H) : Entity (image, Name, X, Y, W, H)  { // Конструктор с параметрами. При создании объекта ему будут задаваться переданные данные.
		playerScore = 0; state = stay; // по умолчанию персонаж жив и находится на земле в неподвижном состоянии.
		obj = level.GetAllObjects(); // получаем все объекты для взаимодействия персонажа с картой
		sprite.setTextureRect(IntRect(30, 30, w, h)); // задаём спрайту один прямоугольник для вывода одного рыцаря
	}
	void update(float time) { // принимает в себя время sfml, поэтому работает бесконечно
		control(); // функция управления персонажем
		switch (state) {
		case right: dx = speed; break; // по иксу задаём положительную скорость, игреку зануляем. Следовательно персонаж идёт только вправо.
		case left: dx = -speed; break;
		case up: break; // вверх
		case down: break; 
		case jump: break; // прыжок
		case stay: break; // неподвижен
		}

		x += dx * time; // ускорение на время = смещение координат.
		checkCollisionWithMap(dx, 0);
		y += dy * time;
		checkCollisionWithMap(0, dy);

		speed = 0; // после того, как я отпущу клавишу, мой персонаж остановится. Иначе он будет идти дальше.
		sprite.setPosition(x + w/2, y + h/2);  // выводим спрайт в позицию x и y. Бесконечно выводим, иначе бы спрайт стоял на месте.
		if (health <= 0) life = false;
		if (!onGround) {
			dy = dy + 0.015*time; // если персонаж не на земле, то происходит его притяжение к земле.
			checkCollisionWithMap(0, dy);
		}
		if (life)
			getPlayerCoordinateforView(x, y);
	}

	void control() {
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			state = left;
			speed = 0.1;
			//P.dir = 1; P.speed = 0.1;//dir =1 - направление вверх, speed =0.1 - скорость движения. Заметьте - время мы уже здесь ни на что не умножаем и нигде не используем каждый раз
			//CurrentFrame += 0.005*time;
			//if (CurrentFrame > 9) CurrentFrame -= 9;
			//P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 205, 144, 165)); //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
		}

		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			state = right;
			speed = 0.1;
			//P.dir = 0; P.speed = 0.1;//направление вправо, см выше
			//CurrentFrame += 0.005*time;
			//if (CurrentFrame > 9) CurrentFrame -= 9;
			//P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 25, 144, 165)); //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
		}

		if (Keyboard::isKeyPressed(Keyboard::Up) && (onGround)) {
			state = jump;
			dy = -0.4;
			onGround = false;
			//P.dir = 3; P.speed = 0.1;//направление вниз, см выше
			//CurrentFrame += 0.005*time;
			//if (CurrentFrame > 9) CurrentFrame -= 9;
			//P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 555, 144, 165)); //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
		}

		if (Keyboard::isKeyPressed(Keyboard::Down)) { //если нажата клавиша стрелка влево или англ буква А
			state = down;
			speed = 0.1;
			//P.dir = 2; P.speed = 0.1;//направление вверх, см выше
			//CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
			//if (CurrentFrame > 9) CurrentFrame -= 9; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
			//P.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 380, 144, 165)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
																					  // передаём координаты персонажа в функцию управления камерой.
		}
		//getPlayerCoordinateforView(P.getPlayerCoordinateX(), P.getPlayerCoordinateY());
	}

	 float getPlayerCoordinateX() {	//этим методом будем забирать координату Х	
		return x;
	}
	float getPlayerCoordinateY() {	//этим методом будем забирать координату Y 	
		return y;
	}

	void checkCollisionWithMap( float Dx, float Dy) { // функция взаимодействия с картой
		/* for (int i = y/32; i < (y + h) / 32; i++)
			for (int j = x / 32; j < (x + w) / 32; j++) { // икс делим на 40, т.е. получаем левый квадратик, с которым персонаж соприкасается
				if (TileMap[i][j] == '0')//если элемент наш тайлик земли? то
				{
					if (Dy>0) { y = i * 32 - h;  dy = 0; onGround = true; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
					if (Dy<0) { y = i * 32 + 32;  dy = 0; }//столкновение с верхними краями карты(может и не пригодиться)
					if (Dx>0) { x = j * 32 - w; }//с правым краем карты
					if (Dx<0) { x = j * 32 + 32; }// с левым краем карты
				}
				else { onGround = false; }
				if (TileMap[i][j] == 's') {
					playerScore++;
					TileMap[i][j] = ' ';
				}
				if (TileMap[i][j] == 'f') { // если подобрали цветок, отнимается здоровье.
					health -= 40;
					TileMap[i][j] = ' '; // сорвали цветок
				}
				if (TileMap[i][j] == 'h') {
					health += 20;
					TileMap[i][j] = ' ';
				}
			 }*/

		for (int i = 0; i<obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))/*проверяем пересечение игрока с объектом
												  если прямоугольник игрока столкнулся с прямоугольником объекта solid, то 
												  мы не даём игроку пройти сквозь этот объект. */
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy>0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy<0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx>0) { x = obj[i].rect.left - w; }
					if (Dx<0) { x = obj[i].rect.left + obj[i].rect.width; }
				}
			}
	}
};


class Enemy : public Entity {
public:
	Enemy(Image &image, String Name, Level &level, float X, float Y, int W, int H) : Entity(image, Name, X, Y, W, H) {
		obj = level.GetObjects("solid"); // даём врагу лишь некоторые объекты для взаимодействия.
		if (name == "EasyEnemy") {
			sprite.setTextureRect(IntRect(13, 131, w, h));
			dx = 0.1;
	 }
	}
	void checkCollisionWithMap(float Dx, float Dy) {//ф ция проверки столкновений с картой
		/* for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
			for (int j = x / 32; j<(x + w) / 32; j++)
			{
				if (TileMap[i][j] == '0')//если элемент наш тайлик земли, то
				{
					if (Dy>0) { y = i * 32 - h; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
					if (Dy<0) { y = i * 32 + 32; }//столкновение с верхними краями карты(может и не пригодиться)
					if (Dx>0) { x = j * 32 - w; dx = -0.1; sprite.scale(-1, 1); }//с правым краем карты
					if (Dx<0) { x = j * 32 + 32; dx = 0.1; sprite.scale(-1, 1); }// с левым краем карты
				}
			}*/

		for(int i = 0; i < obj.size(); i++) // проходимся по всем объектам
			if (getRect().intersects(obj[i].rect)) // проверяем пересечение врага с объектами
			{
				if (Dy > 0) { // это глупый враг, и для него все объекты = solid.
					y = obj[i].rect.top - h;
					dy = 0;
					onGround = true;
				}
				if (Dy < 0) {
					y = obj[i].rect.top + obj[i].rect.height; 
					dy = 0;
				}
				if (Dx > 0) {
					x = obj[i].rect.top - w;
					dx = -0.1;
					sprite.scale(-1, 1);
				}
				if (Dx < 0) {
					x = obj[i].rect.top + obj[i].rect.width;
					dx = 0.1;
					sprite.scale(-1, 1);
				}
			}
	}

	void update(float time)
	{
		if (name == "EasyEnemy") {//для персонажа с таким именем логика будет такой

								  //moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
			checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
			if (health <= 0) { life = false; }
		}
	}
};


int main() {

	RenderWindow window(VideoMode(1400, 800), "SFMLworks");
	view.reset(sf::FloatRect(0, 0, 1400, 800)); // Что-то вроде инициализации камеры.

	Level level; // экземпляр класса "уровень"
	level.LoadFromFile("map.tmx"); // я загрузил в него карту, и с помощью своих методов он его обрабатывает.

	Font font; // шрифт
	font.loadFromFile("CyrilicOld.TTF");
	Text text("", font, 20); // создаём объект текст, закидываем в него строку, размер, шрифт.
	text.setFillColor(Color::Red); // присваиваем ему красный цвет
	text.setStyle(Text::Bold); // делаем текст жирным

	/*Image map_image;//объект изображения для карты
	map_image.loadFromFile("image/map.png");//загружаем файл для карты
	Texture map;//текстура карты
	map.loadFromImage(map_image);//заряжаем текстуру картинкой
	Sprite s_map;//создаём спрайт для карты
	s_map.setTexture(map);//заливаем текстуру спрайтом

	*/
	Image hero_image;
	hero_image.loadFromFile("image/walkingsprite.jpg");
	for (int i = 210; i < 256; i++) {
		hero_image.createMaskFromColor(Color(i, i, i));
	}

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("image/easy_enemy.png");
	easyEnemyImage.createMaskFromColor(Color(0, 0, 0));//сделали маску по цвету.но лучше изначально иметь прозрачную картинку

	Object player = level.GetObject("Player");//объект игрока на нашей карте.задаем координаты игроку в начале при помощи него
	Object easyEnemyObject = level.GetObject("easyEnemy");//объект легкого врага на нашей карте.задаем координаты игроку в начале при помощи него


	Image scroll_image;
	scroll_image.loadFromFile("image/scroll.jpg");
	Texture scroll_texture;
	scroll_texture.loadFromImage(scroll_image);
	Sprite scroll_sprite;
	scroll_sprite.setTexture(scroll_texture);
	scroll_sprite.setTextureRect(IntRect(0, 0, 340, 510));
	scroll_sprite.setScale(0.6f, 0.6f); // уменьшаем свиток, потому что он должен занимать лишь небольшую часть окна.


	Player P(hero_image, "Player", level, player.rect.left, player.rect.top, 144, 160); // передаю координаты прямоугольника player 
	Enemy EasyEnemy(easyEnemyImage, "EasyEnemy", level, easyEnemyObject.rect.left, easyEnemyObject.rect.top, 77, 99);

	bool showMissionText = true; // логическая переменная, отвечающая за появление текста миссии на экране

	float CurrentFrame = 0;
	Clock clock;// создаём переменную времени, т.е. привязка ко времени, а не к мощности процессора
	Clock gameTimeClock; // игровое время
	int gameTime = 0;

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds(); // дать прошедшее время в микросекундах

		if (P.life == true) gameTime = gameTimeClock.getElapsedTime().asSeconds(); // идёт вперёд, пока жив игрок.


		clock.restart(); // перезагружает время
		time = time / 800;

		//Vector2i pixelPos = Mouse::getPosition(window);//забираем коорд курсора
		//Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)

		Event event;

		while (window.pollEvent(event))
		{

			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed) // если нажата клавиша
				if ((event.key.code == Keyboard::Tab)) {

					switch (showMissionText) {

					case true: {
						std::ostringstream task;
						task << getTextMission(getCurrentMission(P.getPlayerCoordinateX()));
						text.setString(task.str());
						showMissionText = false;//эта строка позволяет убрать все что мы вывели на экране
						break;//выходим , чтобы не выполнить условие "false" (которое ниже)
					}

					case false: {
						text.setString("");//если не нажата клавиша таб, то весь этот текст пустой
						showMissionText = true;// а эта строка позволяет снова нажать клавишу таб и получить вывод на экран
						break;
					}
					}
				}
		}

		P.update(time);//оживляем объект P класса Player с помощью времени sfml, передавая время в качестве параметра функции update. благодаря этому персонаж может двигаться.
		EasyEnemy.update(time);
		viewmap(time); // функция скролинга карты
		changeview();
		window.setView(view); // оживляет камеру в окне sfml
		window.clear();
		level.Draw(window);

		window.draw(P.sprite);//рисуем спрайт объекта P класса player
		window.draw(EasyEnemy.sprite);
		window.display();

	}

	return 0;

}
