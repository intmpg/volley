#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#define _WIN32_WINNT 0x0500 
#include <windows.h> 


using namespace sf;
////////////////////////////////////Общий класс родитель//////////////////////////////////////////////////////////////////////////////////////
class Entity {
public:
	float m_x;
	float m_y;
	float m_speed;
	int m_score;
	bool m_life;
	bool m_isMove;
	bool m_onGround;
	Vector2f m_acceleration;//dx и dy
	Image m_image;
	Texture m_texture;
	Sprite m_sprite;
	String m_name;//у нас есть 2 игрока, мы не будем делать другой класс для различающегося игрока.всего лишь различим игроков по имени и дадим каждому свое действие  в контроле и update в зависимости от имени

	Entity(Image image, float X, float Y, int W, int H, String Name) : 
		m_image(image),
		m_x(X), 
		m_y(Y), 
		m_name(Name),
		m_speed(0.1),
		m_score(0),
		m_life(true),
		m_onGround(false),
		m_isMove(false)
		{
			m_acceleration.x = 0;
			m_acceleration.y = 0;
			m_texture.loadFromImage(m_image);
			m_sprite.setTexture(m_texture);
			m_sprite.setOrigin(m_sprite.getTextureRect().width / 2, m_sprite.getTextureRect().height / 2);
		};
	

	FloatRect getRect(){
		return FloatRect(m_x, m_y, m_sprite.getTextureRect().width, m_sprite.getTextureRect().height);
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//добавляем тип перечисления - состояние объекта
	

	Player(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name){
		state = stay;
		if (m_name == "Player"){
			m_sprite.setTextureRect(IntRect(1, 1, m_sprite.getTextureRect().width, m_sprite.getTextureRect().height));
		}
		if (m_name == "Player2"){
			m_sprite.setTextureRect(IntRect(0, 0, m_sprite.getTextureRect().width, m_sprite.getTextureRect().height));
		}

	}

	void control(){

		if (m_name == "Player1"){
			if (Keyboard::isKeyPressed){//если нажата клавиша
				if (Keyboard::isKeyPressed(Keyboard::Left)) {//а именно левая
					state = left; m_speed = 0.4;
				}
				if (Keyboard::isKeyPressed(Keyboard::Right)) {
					state = right; m_speed = 0.4;
				}

				if ((Keyboard::isKeyPressed(Keyboard::Up)) && (m_onGround)) {//если нажата клавиша вверх и мы на земле, то можем прыгать
					state = jump; m_acceleration.y = -0.9; m_onGround = false;//увеличил высоту прыжка
				}

				if (Keyboard::isKeyPressed(Keyboard::Down)) {
					state = down;
				}
			}
		}

		if (m_name == "Player2"){
			if (Keyboard::isKeyPressed)
			{//если нажата клавиша
				if (Keyboard::isKeyPressed(Keyboard::A)) {//а именно левая
					state = left; m_speed = 0.4;
				}
				if (Keyboard::isKeyPressed(Keyboard::D)) {
					state = right; m_speed = 0.4;
				}

				if ((Keyboard::isKeyPressed(Keyboard::W) && (m_onGround)))
				{//если нажата клавиша вверх и мы на земле, то можем прыгать
					state = jump; m_acceleration.y = -0.9; m_onGround = false;//увеличил высоту прыжка
				}

				if (Keyboard::isKeyPressed(Keyboard::S)) {
					state = down;
				}
			}
		}
	}
	

	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int yi = m_y / 32; yi < (m_y + m_sprite.getTextureRect().height) / 32; yi++){//проходимся по элементам карты
			for (int xj = m_x / 32; xj<(m_x + m_sprite.getTextureRect().width) / 32; xj++)
			{
				if (TileMap[yi][xj] == '0')//если элемент наш тайлик земли? то
				{
					if (Dy>0){ m_y = yi * 32 - m_sprite.getTextureRect().height;  m_acceleration.y = 0; m_onGround = true; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
					if (Dy<0){ m_y = yi * 32 + 32;  m_acceleration.y = 0; }//столкновение с верхними краями карты(может и не пригодиться)
					if (Dx>0){ m_x = xj * 32 - m_sprite.getTextureRect().width; }//с правым краем карты
					if (Dx<0){ m_x = xj * 32 + 32; }// с левым краем карты
				}
			}
		}
	}

	void checkGameOver(){
		if (m_score <= 0){ m_life = false; }
	}

	void checkMovePlayer(){
		if (!m_isMove){ m_speed = 0; }
	}

	void update(float time)
	{
		control();//функция управления персонажем
		switch (state)//тут делаются различные действия в зависимости от состояния
		{
			case right:m_acceleration.x = m_speed; break;//состояние идти вправо
			case left:m_acceleration.x = -m_speed; break;//состояние идти влево
			case up: break;//будет состояние поднятия наверх (например по лестнице)
			case down: m_acceleration.x = 0; break;//будет состояние во время спуска персонажа (например по лестнице)
			case stay: break;//и здесь тоже		
		}
		m_x += m_acceleration.x*time;
		checkCollisionWithMap(m_acceleration.x, 0);//обрабатываем столкновение по Х
		m_y += m_acceleration.y*time;
		checkCollisionWithMap(0, m_acceleration.y);//обрабатываем столкновение по Y
		m_sprite.setPosition(m_x + m_sprite.getTextureRect().width / 2, m_y + m_sprite.getTextureRect().height / 2); //задаем позицию спрайта в место его центра
		checkGameOver();
		checkMovePlayer();
		m_acceleration.y = m_acceleration.y + 0.0015*time;//постоянно притягиваемся к земле
	}
};

class Ball :public Entity {
	
public:
	bool m_isFeed;//подача мяча
	
	
	
	int const topHit = 20;
	int const centerHit = 15;
	Ball(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		if (m_name == "Ball") {
			m_isFeed = true;//инициализируем переменную подачи. при старте игры она, конечно, будет истиной
			m_sprite.setTextureRect(IntRect(0, 0, m_sprite.getTextureRect().width, m_sprite.getTextureRect().height));
		
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = m_y / 32; i < (m_y + m_sprite.getTextureRect().height) / 32; i++)//проходимся по элементам карты
		for (int j = m_x / 32; j<(m_x + m_sprite.getTextureRect().width) / 32; j++)
		{
			if (TileMap[i][j] == '0')//если элемент наш тайлик земли? то
			{
				if (Dy>0){ m_y = i * 32 - m_sprite.getTextureRect().height; m_acceleration.y = 0; m_onGround = true; m_acceleration.y+= -0.3; }//ударяемся о стены уменьшая скорость мяча и давая ему противоположное стене направление
				if (Dy<0){ m_y = i * 32 + 32;  m_acceleration.y += 0.3; }//см выше
				if (Dx>0){ m_x = j * 32 - m_sprite.getTextureRect().width;  m_acceleration.x += -0.4; }//см выше
				if (Dx<0){ m_x = j * 32 + 32; m_acceleration.x += 0.4; }//см выше
			}
		}
	}


	void checkCollisionWithPlayer(Player p, Player p2) {
		if (getRect().intersects(p.getRect())){//если мяч пересекся с игроком 1
			if (m_isFeed == true){ m_isFeed = false; }//если была подача то сообщаем что она уже была
			if (p.m_x + p.m_sprite.getTextureRect().width / 2 + topHit > m_x + m_sprite.getTextureRect().width / 2){ m_acceleration.x = -0.6; m_acceleration.y = -0.3; }//если мяч попал правее головы игрока, то он летит вправо
			if (p.m_x + p.m_sprite.getTextureRect().width / 2 - topHit < m_x + m_sprite.getTextureRect().width / 2) { m_acceleration.x = 0.6; m_acceleration.y = -0.3; }//аналогично влево
			if ((p.m_x + p.m_sprite.getTextureRect().width / 2 + 15 > m_x) && (p.m_x + p.m_sprite.getTextureRect().width / 2 - 15 < m_x)){ m_acceleration.y = -0.7; }//если ударили мячом примерно в районе центра игрока, то мяч подлетает чуть выше
			if (!p.m_onGround){ m_acceleration.y -= 0.4; }//если игрок прыгнул то ударяем мяч сильнее,прибавляя к текущей его скорости еще 0,4
			if (p.m_y + p.m_sprite.getTextureRect().height - 2 < m_y) { m_acceleration.y = 0.6; }//если игрок вдруг напрыгнул на мяч сверху то мяч летит вниз
		}
		if (getRect().intersects(p2.getRect())){//если мяч пересекся с игроком 2
			if (m_isFeed == true){ m_isFeed = false; }//если была подача то сообщаем что она уже была
			if (p2.m_x + p2.m_sprite.getTextureRect().width / 2 + topHit > m_x + m_sprite.getTextureRect().width / 2){ m_acceleration.x = -0.6; m_acceleration.y = -0.3; }//если мяч попал правее головы игрока то он летит вправо
			if (p2.m_x + p2.m_sprite.getTextureRect().width / 2 - topHit < m_x + m_sprite.getTextureRect().width / 2) { m_acceleration.x = 0.6; m_acceleration.y = -0.3; }//аналогично влево
			if ((p2.m_x + p2.m_sprite.getTextureRect().width / 2 + centerHit > m_x) && (p2.m_x + p2.m_sprite.getTextureRect().width / 2 - 15 <m_x)){ m_acceleration.y = -0.7; }//если ударили мячом примерно в районе центра игрока то мяч подлетает чуть выше
			if (!p2.m_onGround){ m_acceleration.y -= 0.4; }//если игрок прыгнул то ударяем мяч сильнее,прибавляя к текущей его скорости еще 0,4
			if (p2.m_y + p2.m_sprite.getTextureRect().height - 2<m_y) { m_acceleration.y = 0.6; }//если игрок вдруг напрыгнул на мяч сверху то мяч летит вниз
		}
	}

	void update(float time)
	{
		if (m_isFeed) { m_acceleration.y = 0; }//если сейчас подача, то мяч должен стоять на месте и никуда не лететь, т.е скорость 0
			else {
				m_acceleration.y = m_acceleration.y + 0.0010*time;//притяж к земле
			}
		m_x += m_acceleration.x*time;
		checkCollisionWithMap(m_acceleration.x, 0);//обрабатываем столкновение по Х
		m_y += m_acceleration.y*time;
		checkCollisionWithMap(0, m_acceleration.y);//обрабатываем столкновение по Y
		m_sprite.setPosition(m_x + m_sprite.getTextureRect().width / 2, m_y + m_sprite.getTextureRect().height / 2); //задаем позицию спрайта в место его центра
	}
};

void scoreAdd(Ball &ball, Player &p,Player &p2) { 
	//начисление очков
	if ((ball.m_onGround == true) && (ball.m_y > 600)) {//если мяч на земле и при этом ниже сетки (иначе будет срабатывать когда попали на верхушку сетки)
		ball.m_acceleration.y = 0; ball.m_acceleration.x = 0;//тормозим мяч при забивании
		if (ball.m_x > 645) { p.m_score++; ball.m_x = 360; ball.m_y = 360; }//если попали мячом по полю второго игрока то начисляем очки первому и ставим мяч на подачу 1 ому игроку
		else { //иначе попали по второму делаем тоже самое
			if (ball.m_x < 644) { p2.m_score++; ball.m_x = 900; ball.m_y = 360; }//даем очки второму игроку, ставим мяч ему на подачу
		}
		ball.m_onGround = false;//мяч в воздухе ждет подачи
		ball.m_isFeed = true;//подача
	}
}

void drawScore(Ball &ball,Player &p,Player &p2, Text &text, RenderWindow &window){
	

	String player1Won = "Первый игрок победил! Сделайте подачу, чтобы начать заново";
	String player2Won = "Второй игрок победил! Сделайте подачу, чтобы начать заново";

	//вывод счета на экран
	std::ostringstream playerScoreString;//тут храним счет
	if (((p.m_score < 16) || (p2.m_score < 16)) && ((p.m_score>0) || (p2.m_score>0)))//если счет открыт и игра еще не окончена (играем до 16ти),то
	{
		playerScoreString << "Счет " << p.m_score << ":" << p2.m_score;//то выводим счет игроков на экран
		text.setString(playerScoreString.str());//закидываем тексту переменную счета
		text.setPosition(view.getCenter().x - 30, view.getCenter().y - 400);//позиция этого текста относительно центра окна
		text.setColor(Color::Yellow);// цвет текста
	}


	if (p.m_score == 16) { ball.m_x = 300; ball.m_y = 400; text.setString(player1Won); text.setPosition(view.getCenter().x - 230, view.getCenter().y - 200); p.m_score = 0; p2.m_score = 0; }//если у 1 ого игрока 16 очков то он победил, выводится надпись об этом . задается место вывода надписи и обнуляется счет игроков. при этом мяч становится на исходную позицию
	if (p2.m_score == 16) { ball.m_x = 300; ball.m_y = 400; text.setString(player2Won); text.setColor(Color::Red); text.setPosition(view.getCenter().x - 230, view.getCenter().y - 200); p.m_score = 0; p2.m_score = 0; }//все тоже самое только для 2 игрока

	window.draw(text);//рисуем текст (счет, или надпись о победе)

}

void game() {

	HWND hWnd = GetConsoleWindow();//берем текущ консоль, (скрывает консоль (работает только под windows))
	ShowWindow(hWnd, SW_HIDE);//скрывает консоль

	RenderWindow window(VideoMode(1300, 800), "Kursovaya");
	view.reset(FloatRect(0, 0, 1300, 800));

	Font font;//шрифт sfml
	font.loadFromFile("CyrilicOld.ttf");//загрузили файл шрифта
	Text text("", font, 20);//объявили текст сфмл и дали ему изначально пустую строку, затем дали шрифт и размер
	text.setColor(Color::Yellow);//цвет текста
	text.setStyle(Text::Bold);//жирный текст

	Image map_image;
	map_image.loadFromFile("images/map.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);

	Image heroImage;
	heroImage.loadFromFile("images/hero.png");
	heroImage.createMaskFromColor(Color(255, 255, 255));//сделали маску по цвету.но лучше изначально иметь прозрачную картинку

	Image p2Image;
	p2Image.loadFromFile("images/hero2.png");
	p2Image.createMaskFromColor(Color(0, 0, 0));//сделали маску по цвету.но лучше изначально иметь прозрачную картинку


	Image bImage;
	bImage.loadFromFile("images/ball.png");
	bImage.createMaskFromColor(Color(0, 0, 0));

	Player p(heroImage, 500, 100, 90, 90, "Player1");//объект класса игрока
	Player p2(p2Image, 700, 100, 90, 90, "Player2");//player2
	Ball  ball(bImage, 300, 360, 40, 40, "Ball");//объект класса мяч

	Clock clock;
	while (window.isOpen())
	{

		float time = clock.getElapsedTime().asMicroseconds();

		clock.restart();
		time = time / 800;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		ball.update(time);
		p.update(time);// Player update function	
		p2.update(time);//p2 update function

		ball.checkCollisionWithPlayer(p,p2);

		scoreAdd(ball,p,p2);//добавление очков


		window.setView(view);
		window.clear();


		drawMap(s_map, window);
		drawScore(ball, p, p2, text, window);

		window.draw(p2.m_sprite);
		window.draw(p.m_sprite);
		window.draw(ball.m_sprite);//рисуем мяч
		window.display();
	}
}


int main()
{
	game();
	
	return 0;
}