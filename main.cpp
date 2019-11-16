#include "console_library/console_library.h"
#include <random>
#include <math.h>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <fstream>
#include <chrono>

constexpr int W = 40;
constexpr int H = 40;
int x = 0;
int y = 0;
int cur_x, cur_y;
int delta_x = 0;
int delta_y = 0;
int old_x, old_y;
int old_old_x, old_old_y;
int rand_x, rand_y;
int score = 0;
int barrier_num = 4;
int maxscore;
int oldscore;
auto start = std::chrono::system_clock::now();

console::Color color[15] = {
	console::Color::RED,
	console::Color::DARK_RED,
	console::Color::YELLOW,
	console::Color::DARK_YELLOW,
	console::Color::GREEN,
	console::Color::DARK_GREEN,
	console::Color::BLUE,
	console::Color::DARK_BLUE,
	console::Color::CYAN,
	console::Color::DARK_CYAN,
	console::Color::MAGENTA,
	console::Color::DARK_MAGENTA,
	console::Color::WHITE,
	console::Color::DARK_WHITE,
	console::Color::GRAY
};

console::State stateS;

struct Point
{
	int x;
	int y;
	Point(int theX, int theY) {
		x = theX;
		y = theY;
	}
	Point() {
		x = 0;
		y = 0;
	}
	bool operator==(const Point& p) {
		return (x == p.x) && (y == p.y);
	}
};

Point head(3, 10);
Point tail(2, 10);
Point food(std::rand() % (W - 1) + 1, std::rand() % (H - 1) + 1);
Point direction(1, 0);  // right(1,0);left(-1,0);up(1,0);down(-1,0)
Point barrier_direction;  // 每一面墙伸展的方向（1,0）or（0,1）
//typedef std::list<Point> Snake; 是下面这一行的另一种实现方法
using Snake = std::list<Point>;
Snake snake;
Snake::iterator iter;

using Barrier = std::list<Point>;
Barrier barrier;
Barrier::iterator iter_barrier;

void printStr(int posX, int posY) {
	HANDLE hd;
	COORD pos;
	pos.X = posX;
	pos.Y = posY;
	hd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hd, pos);

	auto color = static_cast<WORD>(console::Color::DARK_WHITE) >> 4;
	SetConsoleTextAttribute(hd, color);
	std::cout << "Score : " << score;
	SetConsoleTextAttribute(hd, 0);
}

console::Color intToColor(int num) {
	auto color = static_cast<console::Color>(num << 4);
	return color;
}

void init() {
	start = std::chrono::system_clock::now();
	srand(time(NULL));
	snake.push_back(head);
	snake.push_back(tail);
	stateS = console::State::RUNNING;
	int rand_x;
	int rand_y;
	int rand_direction = rand() % 2;
	for (int i = 0; i < barrier_num; ++i) {
		rand_x = rand() % (W / 3) + W / 3;
		rand_y = rand() % (H / 3) + H / 3;
		if (rand_direction == 1) {
			barrier_direction.x = 1;
			barrier_direction.y = 0;
		}
		else {
			barrier_direction.x = 0;
			barrier_direction.y = 1;
		}
		for (int i = 0; i < 3; ++i) {
			barrier.emplace_back(i*barrier_direction.x + rand_x, i*barrier_direction.y + rand_y);
		}
	}
}

void display() {
	console::draw::clear();
	// draw the barrier
	int i = 0;
	for (iter_barrier = barrier.begin(); iter_barrier != barrier.end(); iter_barrier++) {
		if (i % 2 == 0)	console::draw::point(iter_barrier->x, iter_barrier->y, console::Color::WHITE);
		else console::draw::point(iter_barrier->x, iter_barrier->y, console::Color::DARK_WHITE);
		i++;
	}
	// 下面这一段实现了一个五彩斑斓的墙壁
	//int j = 0;
	//for (int x = 0; x < W; ++x) {
	//	console::draw::point(x, 0, intToColor(j%15));  // 使用到了static_cast函数
	//	console::draw::point(x, H - 1, intToColor(j%15));
	//	j++;
	//}
	//for (int y = 0; y < H; ++y) {
	//	console::draw::point(0, y, color[j % 15]);
	//	console::draw::point(W - 1, y, color[j % 15]);
	//	j++;
	//}

	// TODO: draw the wall with Color::WHITE and Color::DARK_WHITE
	for (int x = 0; x < W; ++x) {
		if (x % 2 == 0) {
			console::draw::point(x, 0, console::Color::WHITE);
			console::draw::point(x, H-1, console::Color::WHITE);
		}
		else {
			console::draw::point(x, 0, console::Color::DARK_WHITE);
			console::draw::point(x, H-1, console::Color::DARK_WHITE);
		}
	}
	for (int y = 0; y < H; ++y) {
		if (y % 2 == 0) {
			console::draw::point(0, y, console::Color::WHITE);
			console::draw::point(W-1, y, console::Color::WHITE);
		}
		else {
			console::draw::point(0, y, console::Color::DARK_WHITE);
			console::draw::point(W-1, y, console::Color::DARK_WHITE);
		}
	}

	// draw the snake with Color::BLUE and Color::DARK_BLUE
	i = 0;
	for (iter = snake.begin(); iter != snake.end(); iter++) {
		x = iter->x;
		y = iter->y;
		if (i % 2 == 0)console::draw::point(x, y, console::Color::BLUE);
		else console::draw::point(x, y, console::Color::DARK_BLUE);
		i++;
	}

	// draw the food with Color::YELLOW
	console::draw::point(food.x, food.y, console::Color::YELLOW);
	console::draw::flush();
	// print the score
	printStr(35, 40);
}

//void keyboard(int vkey, int state) {
//	using console::keyboard::press_key;
//	if (press_key('A')) {
//		direction.x = -1;
//		direction.y = 0;
//	}
//	else if (press_key('D')) {
//		direction.x = +1;
//		direction.y = 0;
//	}
//	else if (press_key('W')) {
//		direction.x = 0;
//		direction.y = -1;
//	}
//	else if (press_key('S')) {
//		direction.x = 0;
//		direction.y = +1;
//	}
//}

void timer() {
	
	using console::keyboard::press_key;
	if (stateS == console::State::RUNNING) {
		if (press_key('X')) {
			stateS = console::State::STOP;
			return;
		}
		if (press_key('A')) {
			direction.x = -1;
			direction.y = 0;
		}
		else if (press_key('D')) {
			direction.x = +1;
			direction.y = 0;
		}
		else if (press_key('W')) {
			direction.x = 0;
			direction.y = -1;
		}
		else if (press_key('S')) {
			direction.x = 0;
			direction.y = +1;
		}

		srand(time(NULL));
		stateS = console::State::RUNNING;
		cur_x = snake.begin()->x + direction.x;
		cur_y = snake.begin()->y + direction.y;
		// judge whether it will hit the wall
		if (cur_x <= 0 || cur_x >= W - 1 || cur_y <= 0 || cur_y >= H - 1) {
			std::cout << "Fail because of hitting the wall!" << std::endl;
			stateS = console::State::FAIL;
		}
		// judge whether it will bite itself
		iter = std::find(snake.begin(), snake.end(), Point(cur_x, cur_y));
		if (iter != snake.end()) {//!!! OK
									   //if (iter_next == snake.end()) {//!!! BUG HERE
			std::cout << "Fail because of hitting itself!" << std::endl;
			stateS = console::State::BITE;
		}
		// judge whether it will hit the barrier
		iter_barrier = std::find(barrier.begin(), barrier.end(), Point(cur_x, cur_y));
		if (iter_barrier != barrier.end()) {
			std::cout << "Fail because of hitting the barrier!" << std::endl;
			stateS = console::State::FAIL;
		}
		if (stateS == console::State::FAIL || stateS == console::State::BITE) {
			std::ifstream fromscorefile("D:/snake/snake/score.txt", std::ios::in);
			if (!fromscorefile.is_open()) {
				console::prompt_box("", "The file open fails, you can not record your score");
				exit(0);
			}
			if (!fromscorefile.eof()) {
				fromscorefile >> oldscore;
				if (score >= oldscore) {
					maxscore = score;
				}
				else {
					maxscore = oldscore;
				}
				fromscorefile.close();
				std::ofstream toscorefile("D:/snake/snake/score.txt", std::ios::out);
				if (!toscorefile.is_open()) {
					console::prompt_box("", "The file open fails, you can not record your score");
					exit(0);
				}
				toscorefile << maxscore << std::endl;
				
				auto end = std::chrono::system_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
				if (maxscore == score) {
					toscorefile << "使用了"
						<< double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << std::endl
						<< "秒" << std::endl;
					console::prompt_box("", "Congratulate for your highest score!");
				}
				else console::prompt_box("", "I am sorry for that you failed");
				toscorefile.close();
				exit(0);
			}
			fromscorefile.close();
			std::ofstream scorefile("D:/snake/snake/score.txt", std::ios::out);
			if (!scorefile.is_open()) {
				console::prompt_box("", "The file open fails, you can not record your score");
				exit(0);
			}
			scorefile << score;
			auto end = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			scorefile << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << std::endl;
			scorefile.close();
			console::prompt_box("", "Congratulate for your highest score!");
			exit(0);
			return;
		}
		if (stateS == console::State::RUNNING) {

			if (cur_x == food.x&&cur_y == food.y) {
				score += 50;
				food.x = rand() % (W / 4) + W / 4;
				food.y = rand() % (H / 4) + W / 4;
				while ((iter = std::find(snake.begin(), snake.end(), Point(food.x, food.y)))!=snake.end()&&(iter_barrier = std::find(barrier.begin(), barrier.end(), Point(food.x, food.y))) != barrier.end()) {
					food.x++;
					food.y++;
				}
				snake.emplace_front(cur_x, cur_y);
				// 这句话的另外两种实现方法，但是用emplace_front最好
				//snake.push_front(Point(cur_x, cur_y));
				//snake.push_front({ cur_x, cur_y });
			}
			else {
				snake.emplace_front(cur_x, cur_y);
				snake.pop_back();
			}
		}
		console::draw::post_redisplay();
	}
	if (stateS == console::State::STOP) {
		while (press_key('C')) {
			stateS = console::State::RUNNING;
			break;
		}
	}
}

int main(int argc, char* argv[]) {
	if (!console::init(W, H)) return 1;

	init();
	console::set_display_func(display);
	console::set_timer_func(timer, 10);

	console::main_loop();

	return 0;
}
