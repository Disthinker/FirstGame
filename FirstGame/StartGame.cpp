#include<graphics.h>
#include<string>

//定义动画帧索引
int idxCurrentAnim{ 0 };
//定义动画帧数量
const int PLAYER_ANIM_NUM{ 6 };
//定义动画循环
IMAGE img_Player_Left[PLAYER_ANIM_NUM];
IMAGE img_Player_Right[PLAYER_ANIM_NUM];

//初始化玩家位置
POINT player_pos{ 500,500 };
//初始化玩家速度
int PLAYER_SPEED{ 5 };
//初始化玩家移动
bool is_move_up{ false };
bool is_move_down{ false };
bool is_move_left{ false };
bool is_move_right{ false };

//封装原绘图函数，处理图片透明背景
#pragma comment(lib,"MSIMG32.LIB")
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w{ img->getwidth() };
	int h{ img->getheight() };
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//加载动画，动画分为左右朝向，分别调用不同图片
void LoadAnimation()
{
	for (size_t i{ 0 }; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path{ L"../Tivat/img/player_left_" + std::to_wstring(i) + L".png" };
		loadimage(&img_Player_Left[i], path.c_str());
	}
	for (size_t i{ 0 }; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path{ L"../Tivat/img/player_right_" + std::to_wstring(i) + L".png" };
		loadimage(&img_Player_Right[i], path.c_str());
	}
}

int main()
{
	//主窗口
	initgraph(1280, 720);

	//游戏主循环控制变量
	bool Gamming{ true };

	//玩家输入变量
	ExMessage msg;

	//性能优化（记录事件开始到结束的时间点，获得休息时间【帧持续时间减去事件时间】）
	//时间变量
	DWORD startTime{ GetTickCount() };
	DWORD endTime{ GetTickCount() };

	//定义图片对象
	IMAGE img_background;
	//导入美术资源（背景，角色动画）
	loadimage(&img_background,_T("../Tivat/img/background.png"));
	LoadAnimation();

	//批量绘图（渲染缓冲）{
	BeginBatchDraw();

	//游戏主循环
	while (Gamming)
	{
		//性能优化（记录事件开始时间点）
		startTime = GetTickCount();

		//事件循环（一次玩家输入为一个循环）
		while (peekmessage(&msg))
		{
			//处理键盘输入信息（无Alt按下方向键）
			if (msg.message == WM_KEYDOWN)
			{
				//当按下上下左右键时，玩家向对应方向开始移动
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
			else if (msg.message == WM_KEYUP)
			{
				//当抬起上下左右键时，玩家停止向对应方向移动
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
		//玩家移动一个游戏帧
		if (is_move_up) player_pos.y -= PLAYER_SPEED;
		if (is_move_down) player_pos.y += PLAYER_SPEED;
		if (is_move_left) player_pos.x -= PLAYER_SPEED;
		if (is_move_right) player_pos.x += PLAYER_SPEED;

		//记录当前动画帧一共播发了几个游戏帧
		static int counter{ 0 };
		//使每帧动画播放5个游戏帧
		if (++counter % 5 == 0)
		{
			idxCurrentAnim++;
		}
		//使动画循环播放
		idxCurrentAnim = idxCurrentAnim % PLAYER_ANIM_NUM;
		
		//玩家两次输入间隙刷新绘图
		cleardevice();

		//更新所有图片
		putimage(0, 0, &img_background);
		putimage_alpha(player_pos.x, player_pos.y, &img_Player_Left[idxCurrentAnim]);
		FlushBatchDraw();

		//性能优化（若事件时间小于帧持续时间，则休息剩余时间）
		endTime = GetTickCount();
		if (endTime - startTime < 1000 / 180)
		{
			Sleep(1000 / 180 - (endTime - startTime));
		}
	}
	
	//}结束批量绘图
	EndBatchDraw();

	return 0;
}