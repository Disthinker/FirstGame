#include<graphics.h> // For EasyX
#include<string> // For TCHAR, _stprintf_s etc
#include<vector> // For std::vector<IMAGE*> frame_list

// def window size
const int WINDOW_WIDTH{ 1280 };
const int WINDOW_HEIGHT{ 720 };

// def img objects global
IMAGE img_background;

// load MSIMG32.LIB for transparent display
#pragma comment(lib,"MSIMG32.LIB")
// encaposlt putimage to enable transparent
inline const void putimage_alpha(int x, int y, IMAGE* img)
{
	int w{ img->getwidth() };
	int h{ img->getheight() };
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

// def Animation class(passed in img path, img num, and img frame interval)
class Animation
{
public:
	// constructor for anim by 3 params
	Animation(LPCTSTR path, int num, int interval)
	{
		// def frame interval
		interval_ms = interval;
		// save path to 1 img				TCHAR str[256] defines a string variable named str,
		//									which can store up to 255 characters.
		//									The type of this variable is Tchar, a data type used in Windows API,
		//									which can be automatically converted to CHAR or wchar_t type 
		//									according to compiler settings.

		TCHAR path_file[256];

		for (size_t i = 0; i < num; i++)
		{
			// format path_i.img			The first param of the _stprintf_s is the string to be formatted,
			// save corrt path to str		and the second parameter is the value to be inserted into the string.
			_stprintf_s(path_file, path, i);
			// creat new space for frame
			IMAGE* frame = new IMAGE();
			// load frame by path
			loadimage(frame, path_file);
			// link frame to vector			push_back is used to insert an element at the end of the vector.
			frame_list.push_back(frame);
		}
	}
	// destructor to release anim space
	~Animation()
	{
		// traverse vector to release
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i];
	}
	// play anim on (x,y) when timer += delta >= interval
	void Play(int x, int y, int delta)
	{
		// timer add up to interval
		timer += delta;
		// time to switch to the next
		if (timer >= interval_ms)
		{
			// change over to the next frame in loop, and set timer to 0
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}
		// render frame on (x,y)
		putimage_alpha(x, y, frame_list[idx_frame]);
	}

private:
	// init def timer 0
	int timer{ 0 };
	// init def frame idx 
	int idx_frame = 0;
	// init def interval
	int interval_ms{ 0 };
	// def frame vector
	std::vector<IMAGE*> frame_list;
};

// def player class(no pass in, default is ok)
class Player
{
public:
	// constructor to load shadow and player anim
	Player()
	{
		loadimage(&img_shadow, _T("../Tivat/img/shadow_player.png"));
		anim_left = new Animation(_T("../Tivat/img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("../Tivat/img/player_right_%d.png"), 6, 45);
	}
	// release space of player's anim
	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	// def how to execute player input
	void ProcessEvent(const ExMessage& msg)
	{
		// set move state true when keydown without alt
		if (msg.message == WM_KEYDOWN)
		{
			// set move state by vk
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			}
		}
		// set move state false when keyup without alt
		else if (msg.message == WM_KEYUP)
		{
			// set move state by vk
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			}
		}
	}

	// def player's motion
	void Move()
	{
		// convert speed and state to vector movement
		// if move in both x and y
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			//  component of motion on x or y
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		// window edge collision
		if (position.x < 0) position.x = 0;
		if (position.y < 0) position.y = 0;
		if (position.x + FRAME_WIDTH > WINDOW_WIDTH) position.x = WINDOW_WIDTH - FRAME_WIDTH;
		if (position.y + FRAME_HEIGHT > WINDOW_HEIGHT) position.y = WINDOW_HEIGHT - FRAME_HEIGHT;
	}

	// draw player anim (pass in game frame interval)
	void Draw(int delta)
	{
		// set shadow position and display
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + FRAME_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		// init orientation of player
		static bool facing_left{ false };
		// get ori by move on x axis
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

		// play anim
		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
	}

	const POINT& GetPosition() const
	{
		return position;
	}

private:
	// def const properties of player
	const int SPEED{ 5 };
	const int FRAME_WIDTH{ 80 };
	const int FRAME_HEIGHT{ 80 };
	const int SHADOW_WIDTH{ 32 };
	IMAGE img_shadow;
	// def dync properties of player
	Animation* anim_left;
	Animation* anim_right;
	POINT position{ 500,500 };
	bool is_move_up{ false };
	bool is_move_down{ false };
	bool is_move_left{ false };
	bool is_move_right{ false };
};

// def Bullet
class Bullet
{
public:
	Bullet() = default;
	~Bullet() = default;
	POINT position{ 0,0 };

	void Draw() const
	{
		// color of fill circle and draw 
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}

private:
	// RADIUS^2 PI = S
	const int RADIUS = 10;
};

class Enemy
{
public:
	// constructor like Player
	Enemy()
	{
		loadimage(&img_shadow, _T("../Tivat/img/shadow_enemy.png"));
		anim_left = new Animation(_T("../Tivat/img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("../Tivat/img/enemy_right_%d.png"), 6, 45);

		// edge of generating enemy
		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};

		// randomly place the enemy outside the map near the edge
		SpawnEdge edge{ (SpawnEdge)(rand() % 4) };
		switch (edge)
		{
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_HEIGHT;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		default:
			break;
		}
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		return false;
	}

	bool CheckPlayerCollision(const Player& player)
	{
		return false;
	}

	void Move(const Player& player)
	{
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
	}

	void Draw(int delta)
	{
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
	}

	// destructor like Player
	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}

private:
	const int SPEED{ 3 };
	const int FRAME_WIDTH{ 80 };
	const int FRAME_HEIGHT{ 80 };
	const int SHADOW_WIDTH{ 48 };

	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position{ 0,0 };
	bool facing_left{ false };
};

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL{ 100 };
	static int counter{ 0 };
	if ((++counter) % INTERVAL == 0)
		enemy_list.push_back(new Enemy());
}

int main()
{
	// initailize main window
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

	// def game loop
	bool Gamming{ true };

	// def player msg
	ExMessage msg;

	// timer to optimize render
	DWORD startTime{ GetTickCount() };
	DWORD endTime{ GetTickCount() };

	// def static img(background, player shadow)
	loadimage(&img_background,_T("../Tivat/img/background.png"));

	//init player
	Player player1{};

	//inti enemy
	std::vector<Enemy*> enemy_list;

	// batch draw for drawing smoothly
	BeginBatchDraw();

	// begin game loop
	while (Gamming)
	{
		// event timer
		startTime = GetTickCount();

		// begin event loop
		while (peekmessage(&msg))
		{
			player1.ProcessEvent(msg);
		}
		// get new position of player
		player1.Move();

		// generate enemy
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy->Move(player1);

		// clear screen
		cleardevice();

		// render background
		putimage(0, 0, &img_background);

		// draw player and enemy anim
		player1.Draw(1000 / 180);
		for (Enemy* enemy : enemy_list)
		{
			enemy->Draw(1000 / 180);
		}

		FlushBatchDraw();

		// record event end
		endTime = GetTickCount();
		// take a break for the rest of the event time(1000/180 - event time)
		if (endTime - startTime < 1000 / 180)
		{
			Sleep(1000 / 180 - (endTime - startTime));
		}
	}
	
	// end batch draw
	EndBatchDraw();

	return 0;
}
