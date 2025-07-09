#include<graphics.h>
#include<string>
#include<vector>

// def window size
const int WINDOW_WIDTH{ 1280 };
const int WINDOW_HEIGHT{ 720 };

// const def player anim num 6
const int PLAYER_ANIM_NUM{ 6 };

// def img objects global
IMAGE img_background;
IMAGE img_shadow;

// forward declearation for encapslted putimage
inline void putimage_alpha(int x, int y, IMAGE* img);


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

// def player anim object 
Animation anim_left_player(_T("../Tivat/img/player_left_%d.png"), 6, 45);
Animation anim_right_player(_T("../Tivat/img/player_right_%d.png"), 6, 45);

// def img size of player and shadow 
const int PLAYER_WIDTH = 80;
const int PLAYER_HEIGHT = 80;
const int SHADOW_WIDTH = 32;

// init player position
POINT player_pos{ 500,500 };
// init player speed
int PLAYER_SPEED{ 5 };
// init move state
bool is_move_up{ false };
bool is_move_down{ false };
bool is_move_left{ false };
bool is_move_right{ false };

// load MSIMG32.LIB for transparent display
#pragma comment(lib,"MSIMG32.LIB")
// encaposlt putimage to enable transparent
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w{ img->getwidth() };
	int h{ img->getheight() };
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

// draw player anim (pass in game frame interval, move on x axis)
void DrawPlayer(int delta, int dir_x)
{
	// set shadow position and display
	int pos_shadow_x = player_pos.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
	int pos_shadow_y = player_pos.y + PLAYER_HEIGHT - 8;
	putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
	// init orientation of player
	static bool facing_left{ false };
	// get ori by move on x axis
	if (dir_x < 0)
		facing_left = true;
	else if (dir_x > 0)
		facing_left = false;

	// play anim
	if (facing_left)
		anim_left_player.Play(player_pos.x, player_pos.y, delta);
	else
		anim_right_player.Play(player_pos.x, player_pos.y, delta);
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
	loadimage(&img_shadow, _T("../Tivat/img/shadow_player.png"));

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
			// set state true when keydown without alt
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
			// set state false when keyup without alt
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
		// record event end
		endTime = GetTickCount();

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
			player_pos.x += (int)(PLAYER_SPEED * normalized_x);
			player_pos.y += (int)(PLAYER_SPEED * normalized_y);
		}

		// window edge collision
		if (player_pos.x < 0) player_pos.x = 0;
		if (player_pos.y < 0) player_pos.y = 0;
		if (player_pos.x + PLAYER_WIDTH > WINDOW_WIDTH) player_pos.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (player_pos.y + PLAYER_HEIGHT > WINDOW_HEIGHT) player_pos.y = WINDOW_HEIGHT - PLAYER_HEIGHT;

		// clear screen
		cleardevice();

		// render background
		putimage(0, 0, &img_background);

		// draw player anim
		DrawPlayer(1000 / 180, is_move_right - is_move_left);
		FlushBatchDraw();

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
