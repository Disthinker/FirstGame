#include<graphics.h>
#include<string>

//���嶯��֡����
int idxCurrentAnim{ 0 };
//���嶯��֡����
const int PLAYER_ANIM_NUM{ 6 };
//���嶯��ѭ��
IMAGE img_Player_Left[PLAYER_ANIM_NUM];
IMAGE img_Player_Right[PLAYER_ANIM_NUM];

//��ʼ�����λ��
POINT player_pos{ 500,500 };
//��ʼ������ٶ�
int PLAYER_SPEED{ 5 };
//��ʼ������ƶ�
bool is_move_up{ false };
bool is_move_down{ false };
bool is_move_left{ false };
bool is_move_right{ false };

//��װԭ��ͼ����������ͼƬ͸������
#pragma comment(lib,"MSIMG32.LIB")
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w{ img->getwidth() };
	int h{ img->getheight() };
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//���ض�����������Ϊ���ҳ��򣬷ֱ���ò�ͬͼƬ
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
	//������
	initgraph(1280, 720);

	//��Ϸ��ѭ�����Ʊ���
	bool Gamming{ true };

	//����������
	ExMessage msg;

	//�����Ż�����¼�¼���ʼ��������ʱ��㣬�����Ϣʱ�䡾֡����ʱ���ȥ�¼�ʱ�䡿��
	//ʱ�����
	DWORD startTime{ GetTickCount() };
	DWORD endTime{ GetTickCount() };

	//����ͼƬ����
	IMAGE img_background;
	//����������Դ����������ɫ������
	loadimage(&img_background,_T("../Tivat/img/background.png"));
	LoadAnimation();

	//������ͼ����Ⱦ���壩{
	BeginBatchDraw();

	//��Ϸ��ѭ��
	while (Gamming)
	{
		//�����Ż�����¼�¼���ʼʱ��㣩
		startTime = GetTickCount();

		//�¼�ѭ����һ���������Ϊһ��ѭ����
		while (peekmessage(&msg))
		{
			//�������������Ϣ����Alt���·������
			if (msg.message == WM_KEYDOWN)
			{
				//�������������Ҽ�ʱ��������Ӧ����ʼ�ƶ�
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
				//��̧���������Ҽ�ʱ�����ֹͣ���Ӧ�����ƶ�
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
		//����ƶ�һ����Ϸ֡
		if (is_move_up) player_pos.y -= PLAYER_SPEED;
		if (is_move_down) player_pos.y += PLAYER_SPEED;
		if (is_move_left) player_pos.x -= PLAYER_SPEED;
		if (is_move_right) player_pos.x += PLAYER_SPEED;

		//��¼��ǰ����֡һ�������˼�����Ϸ֡
		static int counter{ 0 };
		//ʹÿ֡��������5����Ϸ֡
		if (++counter % 5 == 0)
		{
			idxCurrentAnim++;
		}
		//ʹ����ѭ������
		idxCurrentAnim = idxCurrentAnim % PLAYER_ANIM_NUM;
		
		//������������϶ˢ�»�ͼ
		cleardevice();

		//��������ͼƬ
		putimage(0, 0, &img_background);
		putimage_alpha(player_pos.x, player_pos.y, &img_Player_Left[idxCurrentAnim]);
		FlushBatchDraw();

		//�����Ż������¼�ʱ��С��֡����ʱ�䣬����Ϣʣ��ʱ�䣩
		endTime = GetTickCount();
		if (endTime - startTime < 1000 / 180)
		{
			Sleep(1000 / 180 - (endTime - startTime));
		}
	}
	
	//}����������ͼ
	EndBatchDraw();

	return 0;
}