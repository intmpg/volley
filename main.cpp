#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#define _WIN32_WINNT 0x0500 
#include <windows.h> 


using namespace sf;
////////////////////////////////////����� ����� ��������//////////////////////////////////////////////////////////////////////////////////////
class Entity {
public:
	float m_x;
	float m_y;
	float m_speed;
	int m_score;
	bool m_life;
	bool m_isMove;
	bool m_onGround;
	Vector2f m_acceleration;//dx � dy
	Image m_image;
	Texture m_texture;
	Sprite m_sprite;
	String m_name;//� ��� ���� 2 ������, �� �� ����� ������ ������ ����� ��� �������������� ������.����� ���� �������� ������� �� ����� � ����� ������� ���� ��������  � �������� � update � ����������� �� �����

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
	enum { left, right, up, down, jump, stay } state;//��������� ��� ������������ - ��������� �������
	

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
			if (Keyboard::isKeyPressed){//���� ������ �������
				if (Keyboard::isKeyPressed(Keyboard::Left)) {//� ������ �����
					state = left; m_speed = 0.4;
				}
				if (Keyboard::isKeyPressed(Keyboard::Right)) {
					state = right; m_speed = 0.4;
				}

				if ((Keyboard::isKeyPressed(Keyboard::Up)) && (m_onGround)) {//���� ������ ������� ����� � �� �� �����, �� ����� �������
					state = jump; m_acceleration.y = -0.9; m_onGround = false;//�������� ������ ������
				}

				if (Keyboard::isKeyPressed(Keyboard::Down)) {
					state = down;
				}
			}
		}

		if (m_name == "Player2"){
			if (Keyboard::isKeyPressed)
			{//���� ������ �������
				if (Keyboard::isKeyPressed(Keyboard::A)) {//� ������ �����
					state = left; m_speed = 0.4;
				}
				if (Keyboard::isKeyPressed(Keyboard::D)) {
					state = right; m_speed = 0.4;
				}

				if ((Keyboard::isKeyPressed(Keyboard::W) && (m_onGround)))
				{//���� ������ ������� ����� � �� �� �����, �� ����� �������
					state = jump; m_acceleration.y = -0.9; m_onGround = false;//�������� ������ ������
				}

				if (Keyboard::isKeyPressed(Keyboard::S)) {
					state = down;
				}
			}
		}
	}
	

	void checkCollisionWithMap(float Dx, float Dy)//� ��� �������� ������������ � ������
	{
		for (int yi = m_y / 32; yi < (m_y + m_sprite.getTextureRect().height) / 32; yi++){//���������� �� ��������� �����
			for (int xj = m_x / 32; xj<(m_x + m_sprite.getTextureRect().width) / 32; xj++)
			{
				if (TileMap[yi][xj] == '0')//���� ������� ��� ������ �����? ��
				{
					if (Dy>0){ m_y = yi * 32 - m_sprite.getTextureRect().height;  m_acceleration.y = 0; m_onGround = true; }//�� Y ����=>���� � ���(����� �� �����) ��� ������. � ���� ������ ���� ���������� ��������� � ��������� ��� �� �����, ��� ���� ������� ��� �� �� ����� ��� ����� ����� ����� �������
					if (Dy<0){ m_y = yi * 32 + 32;  m_acceleration.y = 0; }//������������ � �������� ������ �����(����� � �� �����������)
					if (Dx>0){ m_x = xj * 32 - m_sprite.getTextureRect().width; }//� ������ ����� �����
					if (Dx<0){ m_x = xj * 32 + 32; }// � ����� ����� �����
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
		control();//������� ���������� ����������
		switch (state)//��� �������� ��������� �������� � ����������� �� ���������
		{
			case right:m_acceleration.x = m_speed; break;//��������� ���� ������
			case left:m_acceleration.x = -m_speed; break;//��������� ���� �����
			case up: break;//����� ��������� �������� ������ (�������� �� ��������)
			case down: m_acceleration.x = 0; break;//����� ��������� �� ����� ������ ��������� (�������� �� ��������)
			case stay: break;//� ����� ����		
		}
		m_x += m_acceleration.x*time;
		checkCollisionWithMap(m_acceleration.x, 0);//������������ ������������ �� �
		m_y += m_acceleration.y*time;
		checkCollisionWithMap(0, m_acceleration.y);//������������ ������������ �� Y
		m_sprite.setPosition(m_x + m_sprite.getTextureRect().width / 2, m_y + m_sprite.getTextureRect().height / 2); //������ ������� ������� � ����� ��� ������
		checkGameOver();
		checkMovePlayer();
		m_acceleration.y = m_acceleration.y + 0.0015*time;//��������� ������������� � �����
	}
};

class Ball :public Entity {
	
public:
	bool m_isFeed;//������ ����
	
	
	
	int const topHit = 20;
	int const centerHit = 15;
	Ball(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		if (m_name == "Ball") {
			m_isFeed = true;//�������������� ���������� ������. ��� ������ ���� ���, �������, ����� �������
			m_sprite.setTextureRect(IntRect(0, 0, m_sprite.getTextureRect().width, m_sprite.getTextureRect().height));
		
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//� ��� �������� ������������ � ������
	{
		for (int i = m_y / 32; i < (m_y + m_sprite.getTextureRect().height) / 32; i++)//���������� �� ��������� �����
		for (int j = m_x / 32; j<(m_x + m_sprite.getTextureRect().width) / 32; j++)
		{
			if (TileMap[i][j] == '0')//���� ������� ��� ������ �����? ��
			{
				if (Dy>0){ m_y = i * 32 - m_sprite.getTextureRect().height; m_acceleration.y = 0; m_onGround = true; m_acceleration.y+= -0.3; }//��������� � ����� �������� �������� ���� � ����� ��� ��������������� ����� �����������
				if (Dy<0){ m_y = i * 32 + 32;  m_acceleration.y += 0.3; }//�� ����
				if (Dx>0){ m_x = j * 32 - m_sprite.getTextureRect().width;  m_acceleration.x += -0.4; }//�� ����
				if (Dx<0){ m_x = j * 32 + 32; m_acceleration.x += 0.4; }//�� ����
			}
		}
	}


	void checkCollisionWithPlayer(Player p, Player p2) {
		if (getRect().intersects(p.getRect())){//���� ��� ��������� � ������� 1
			if (m_isFeed == true){ m_isFeed = false; }//���� ���� ������ �� �������� ��� ��� ��� ����
			if (p.m_x + p.m_sprite.getTextureRect().width / 2 + topHit > m_x + m_sprite.getTextureRect().width / 2){ m_acceleration.x = -0.6; m_acceleration.y = -0.3; }//���� ��� ����� ������ ������ ������, �� �� ����� ������
			if (p.m_x + p.m_sprite.getTextureRect().width / 2 - topHit < m_x + m_sprite.getTextureRect().width / 2) { m_acceleration.x = 0.6; m_acceleration.y = -0.3; }//���������� �����
			if ((p.m_x + p.m_sprite.getTextureRect().width / 2 + 15 > m_x) && (p.m_x + p.m_sprite.getTextureRect().width / 2 - 15 < m_x)){ m_acceleration.y = -0.7; }//���� ������� ����� �������� � ������ ������ ������, �� ��� ��������� ���� ����
			if (!p.m_onGround){ m_acceleration.y -= 0.4; }//���� ����� ������� �� ������� ��� �������,��������� � ������� ��� �������� ��� 0,4
			if (p.m_y + p.m_sprite.getTextureRect().height - 2 < m_y) { m_acceleration.y = 0.6; }//���� ����� ����� ��������� �� ��� ������ �� ��� ����� ����
		}
		if (getRect().intersects(p2.getRect())){//���� ��� ��������� � ������� 2
			if (m_isFeed == true){ m_isFeed = false; }//���� ���� ������ �� �������� ��� ��� ��� ����
			if (p2.m_x + p2.m_sprite.getTextureRect().width / 2 + topHit > m_x + m_sprite.getTextureRect().width / 2){ m_acceleration.x = -0.6; m_acceleration.y = -0.3; }//���� ��� ����� ������ ������ ������ �� �� ����� ������
			if (p2.m_x + p2.m_sprite.getTextureRect().width / 2 - topHit < m_x + m_sprite.getTextureRect().width / 2) { m_acceleration.x = 0.6; m_acceleration.y = -0.3; }//���������� �����
			if ((p2.m_x + p2.m_sprite.getTextureRect().width / 2 + centerHit > m_x) && (p2.m_x + p2.m_sprite.getTextureRect().width / 2 - 15 <m_x)){ m_acceleration.y = -0.7; }//���� ������� ����� �������� � ������ ������ ������ �� ��� ��������� ���� ����
			if (!p2.m_onGround){ m_acceleration.y -= 0.4; }//���� ����� ������� �� ������� ��� �������,��������� � ������� ��� �������� ��� 0,4
			if (p2.m_y + p2.m_sprite.getTextureRect().height - 2<m_y) { m_acceleration.y = 0.6; }//���� ����� ����� ��������� �� ��� ������ �� ��� ����� ����
		}
	}

	void update(float time)
	{
		if (m_isFeed) { m_acceleration.y = 0; }//���� ������ ������, �� ��� ������ ������ �� ����� � ������ �� ������, �.� �������� 0
			else {
				m_acceleration.y = m_acceleration.y + 0.0010*time;//������ � �����
			}
		m_x += m_acceleration.x*time;
		checkCollisionWithMap(m_acceleration.x, 0);//������������ ������������ �� �
		m_y += m_acceleration.y*time;
		checkCollisionWithMap(0, m_acceleration.y);//������������ ������������ �� Y
		m_sprite.setPosition(m_x + m_sprite.getTextureRect().width / 2, m_y + m_sprite.getTextureRect().height / 2); //������ ������� ������� � ����� ��� ������
	}
};

void scoreAdd(Ball &ball, Player &p,Player &p2) { 
	//���������� �����
	if ((ball.m_onGround == true) && (ball.m_y > 600)) {//���� ��� �� ����� � ��� ���� ���� ����� (����� ����� ����������� ����� ������ �� �������� �����)
		ball.m_acceleration.y = 0; ball.m_acceleration.x = 0;//�������� ��� ��� ���������
		if (ball.m_x > 645) { p.m_score++; ball.m_x = 360; ball.m_y = 360; }//���� ������ ����� �� ���� ������� ������ �� ��������� ���� ������� � ������ ��� �� ������ 1 ��� ������
		else { //����� ������ �� ������� ������ ���� �����
			if (ball.m_x < 644) { p2.m_score++; ball.m_x = 900; ball.m_y = 360; }//���� ���� ������� ������, ������ ��� ��� �� ������
		}
		ball.m_onGround = false;//��� � ������� ���� ������
		ball.m_isFeed = true;//������
	}
}

void drawScore(Ball &ball,Player &p,Player &p2, Text &text, RenderWindow &window){
	

	String player1Won = "������ ����� �������! �������� ������, ����� ������ ������";
	String player2Won = "������ ����� �������! �������� ������, ����� ������ ������";

	//����� ����� �� �����
	std::ostringstream playerScoreString;//��� ������ ����
	if (((p.m_score < 16) || (p2.m_score < 16)) && ((p.m_score>0) || (p2.m_score>0)))//���� ���� ������ � ���� ��� �� �������� (������ �� 16��),��
	{
		playerScoreString << "���� " << p.m_score << ":" << p2.m_score;//�� ������� ���� ������� �� �����
		text.setString(playerScoreString.str());//���������� ������ ���������� �����
		text.setPosition(view.getCenter().x - 30, view.getCenter().y - 400);//������� ����� ������ ������������ ������ ����
		text.setColor(Color::Yellow);// ���� ������
	}


	if (p.m_score == 16) { ball.m_x = 300; ball.m_y = 400; text.setString(player1Won); text.setPosition(view.getCenter().x - 230, view.getCenter().y - 200); p.m_score = 0; p2.m_score = 0; }//���� � 1 ��� ������ 16 ����� �� �� �������, ��������� ������� �� ���� . �������� ����� ������ ������� � ���������� ���� �������. ��� ���� ��� ���������� �� �������� �������
	if (p2.m_score == 16) { ball.m_x = 300; ball.m_y = 400; text.setString(player2Won); text.setColor(Color::Red); text.setPosition(view.getCenter().x - 230, view.getCenter().y - 200); p.m_score = 0; p2.m_score = 0; }//��� ���� ����� ������ ��� 2 ������

	window.draw(text);//������ ����� (����, ��� ������� � ������)

}

void game() {

	HWND hWnd = GetConsoleWindow();//����� ����� �������, (�������� ������� (�������� ������ ��� windows))
	ShowWindow(hWnd, SW_HIDE);//�������� �������

	RenderWindow window(VideoMode(1300, 800), "Kursovaya");
	view.reset(FloatRect(0, 0, 1300, 800));

	Font font;//����� sfml
	font.loadFromFile("CyrilicOld.ttf");//��������� ���� ������
	Text text("", font, 20);//�������� ����� ���� � ���� ��� ���������� ������ ������, ����� ���� ����� � ������
	text.setColor(Color::Yellow);//���� ������
	text.setStyle(Text::Bold);//������ �����

	Image map_image;
	map_image.loadFromFile("images/map.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);

	Image heroImage;
	heroImage.loadFromFile("images/hero.png");
	heroImage.createMaskFromColor(Color(255, 255, 255));//������� ����� �� �����.�� ����� ���������� ����� ���������� ��������

	Image p2Image;
	p2Image.loadFromFile("images/hero2.png");
	p2Image.createMaskFromColor(Color(0, 0, 0));//������� ����� �� �����.�� ����� ���������� ����� ���������� ��������


	Image bImage;
	bImage.loadFromFile("images/ball.png");
	bImage.createMaskFromColor(Color(0, 0, 0));

	Player p(heroImage, 500, 100, 90, 90, "Player1");//������ ������ ������
	Player p2(p2Image, 700, 100, 90, 90, "Player2");//player2
	Ball  ball(bImage, 300, 360, 40, 40, "Ball");//������ ������ ���

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

		scoreAdd(ball,p,p2);//���������� �����


		window.setView(view);
		window.clear();


		drawMap(s_map, window);
		drawScore(ball, p, p2, text, window);

		window.draw(p2.m_sprite);
		window.draw(p.m_sprite);
		window.draw(ball.m_sprite);//������ ���
		window.display();
	}
}


int main()
{
	game();
	
	return 0;
}