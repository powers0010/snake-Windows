//------------------------������˵����---------------------
//��������GameDemoCore
//GDI��Ϸ�������


//------------------------��ͷ�ļ�����---------------------
#include <Windows.h>
#include <stdio.h>
#include <ctime>

//------------------------�����ļ����֡�-------------------
#pragma comment(lib,"Winmm.lib")//playsound�����Ŀ�
#pragma comment(lib,"Msimg32.lib")//transparentBlt�����Ŀ�
//------------------------���궨������---------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 640
#define WINDOW_TITLE  L"̰����1.0"

//�ṹ��
struct SNAKE	//����
{
	int x;
	int y;
	bool exsit;
};

//------------------------��ȫ�ֱ�����������---------------
HDC g_hdc=NULL,g_mdc=NULL,g_sdc=NULL;		//�豸����DC���ڴ滷��DC
HBITMAP g_hBg=NULL,g_hHead=NULL,g_hBody=NULL,g_hFood=NULL,g_hFailure=NULL,g_hGameStart=NULL,g_hRestart=NULL;//λͼ��Դ
DWORD g_Pre=0,g_Now=0;		//��¼ʱ�䣬g_Pre��¼��һ�λ�ͼʱ�䣬g_Now��¼�˴�׼����ͼʱ��
int g_iNum=0;				//��¼Body����
SNAKE g_Head,g_Food,g_Body[100]={0};	//��ͷ���꼰��������
int g_Director=0;			//��¼���� ��0  ��1  ��2  ��3
int g_GameState=3;			//��Ϸ״̬ 0��Ϸ�����С�1��Ϸʧ�ܽ��桢3��Ϸ��ʼ���� ��4 ��Ϸ���¿�ʼ����
DWORD g_Time=200;	//ǰ���ٶ� ÿ20һ���Ѷ�
int g_Score=0;//����
int g_Level=0;//�ȼ�
int g_Space=1;//��ͣ����
//-----------------------��ȫ�ֺ�����������-----------------
LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL Game_Init(HWND hwnd);//�ڴ˺����н�����Դ��ʼ��
VOID Game_Paint(HWND hwnd);//�ڴ˺����н��л�ͼ�������д
BOOL Game_CleanUp(HWND hwnd);//�ڴ˺���������Դ������
void GetHeadXY();//����̰��������
void GetBodyXY();//�����������
void GameState();//�ж���Ϸ״̬ 
bool GetFood();//�ж��Ƿ����ʳ��
void EatFood();//���г�ʳ��������任
bool Head_Body();//���ͷ�����Ƿ���
void SetFood();//����food����
bool Body_Food();//��������food��body�Ƿ��ص�
void LevelUp();//����
//------------------------��WinMain()������------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowcmd)
{
	//��1���Ĳ���֮һ���������
	WNDCLASSEX wndclass={0};//���崰����
	wndclass.cbSize=sizeof(WNDCLASSEX);//��ƽṹ���ֽ�����С
	wndclass.style=CS_HREDRAW|CS_VREDRAW;//��ƴ�����ʽ
	wndclass.lpfnWndProc=WndProc;//����ָ�򴰿ڹ��̺�����ָ��
	wndclass.cbClsExtra=0;//������ĸ����ڴ�
	wndclass.cbWndExtra=0;//���ڵĸ����ڴ�
	wndclass.hInstance=hInstance;//ָ���������ڹ��̵ĳ����ʵ�����
	wndclass.hIcon=(HICON)::LoadImage(NULL,L"image\\icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);//ָ��������Ĺ����
	wndclass.hbrBackground=(HBRUSH)::GetStockObject(GRAY_BRUSH);//ָ��һ����ɫ��ˢ���
	wndclass.lpszMenuName=NULL;//��һ���Կ��ַ���β���ַ�����ָ���˵���Դ������
	wndclass.lpszClassName=L"̰����1.0";//��һ���Կ��ַ���β���ַ���,ָ�������������

	//��2���Ĳ���֮����ע�ᴰ����
	if(!RegisterClassEx(&wndclass))//����ע�ᣬRegisterClassEx����ע��ɹ����ش������͵ı�ʶ��;�������ʧ�ܣ�����ֵΪ0
		return -1;

	//��3���Ĳ���֮������ʽ��������
	HWND hwnd=CreateWindow( //��������ɹ�������ֵΪ�´��ڵľ��:�������ʧ�ܣ�����ֵΪNULL�������ø��������Ϣ�������GetLastError������
		L"̰����1.0",	//֮ǰ�����Ĵ����������
		WINDOW_TITLE,	//ָ��һ��ָ���������Ŀս������ַ���ָ�롣
		WS_OVERLAPPEDWINDOW,//ָ���������ڵķ��
		CW_USEDEFAULT,//ָ�����ڵĳ�ʼˮƽλ��
		CW_USEDEFAULT,//ָ�����ڵĳ�ʼ��ֱλ
		WINDOW_WIDTH,//���豸��Ԫָ�����ڵĿ��
		WINDOW_HEIGHT,//���豸��Ԫָ�����ڵĸ߶�
		NULL,//ָ�򱻴������ڵĸ����ڻ������ߴ��ڵľ��
		NULL,//�˵�����������ݴ��ڷ��ָ��һ���Ӵ��ڱ�ʶ
		hInstance,//�봰���������ģ��ʵ���ľ��
		NULL//ָ��һ��ֵ��ָ�룬��ֵ���ݸ�����WM_CREATE��Ϣ
		);

	//��4���Ĳ���֮�ģ����ڵ��ƶ�����ʾ�͸���
	MoveWindow(hwnd,250,20,WINDOW_WIDTH,WINDOW_HEIGHT,true);//����������ʾ��λ��,λ�����Ͻǣ�250��80����

	ShowWindow(hwnd,nShowcmd);//����ShowWindow������ʾ����
	UpdateWindow(hwnd);//�Դ��ڽ��и���

	//��Ϸ��Դ�ĳ�ʼ���������ʼ��ʧ�ܣ�������Ϣ�򣬲�����FALSE
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd,L"��Դ��ʼ��ʧ��",L"��Ϣ����",0);
		return FALSE;
	}
	//���ű�������
	PlaySound(L"image\\demo01.wav",NULL,SND_FILENAME|SND_ASYNC|SND_LOOP);//ѭ�����ű�������

	//��5�����岽����Ϣѭ������	
	MSG msg={0};
	while(msg.message!=WM_QUIT)//ʹ��whileѭ���������Ϣ����WM_QUIT�ͼ���ѭ��
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))//�����Ϣ�ɵõ������ط���ֵ;���û����Ϣ�ɵõ�������ֵ���㡣
			//BOOL PeekMessage(LPMSG IpMsg,HWND hWnd,UINT wMSGfilterMin,UINT wMsgFilterMax,UINT wRemoveMsg);
		{
			TranslateMessage(&msg);//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);//�ַ�һ����Ϣ�����ڳ���,��Ϣ���ַ����ص�����(���̺���)����������Ϣ���ݸ�����ϵͳ��Ȼ�����ϵͳȥ�������ǵĻص�����
		}
		else
		{
			g_Now=GetTickCount();//��ȡ��ǰʱ��
			if((g_Now-g_Pre)>=g_Time)//���˴�ѭ�����ϴλ�ͼʱ�����0.1sʱ���в���
				Game_Paint(hwnd);
		}
	}

	//��6�����������������ע��
	UnregisterClass(L"DreamofGame",hInstance);//����׼��������ע��������

	return 0;
}

//---------------------------��WndProc������ ---------------
//���ڹ��̺������Դ�����Ϣ���д���
//---------------------------------------------------------
LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT paintStruct;//����һ��PAINTSTRUCT�ṹ���¼������Ϣ
	POINT pm;
	switch(message)//switch��俪ʼ
	{
	case WM_PAINT:
		g_hdc=BeginPaint(hwnd,&paintStruct);//ָ�����ڽ��л�ͼ׼�����������úͻ�ͼ�йص���Ϣ��䵽paintStruct�ṹ����
		Game_Paint(hwnd);
		EndPaint(hwnd,&paintStruct);//EndPaint�������ָ�����ڻ�ͼ���̽���
		ValidateRect(hwnd,NULL);//���¿ͻ�������ʾ
		break;
	case WM_LBUTTONDOWN:
		if (g_GameState==3)//����ڳ�ʼ����
		{
			pm.x=LOWORD(lParam);
			pm.y=HIWORD(lParam);
			if (pm.x>300&&pm.x<500&&pm.y>250&&pm.y<334)//������¿�ʼ��Ϸ
			{
				g_GameState=0;//��ʼ��Ϸ
			}
		}
		if (g_GameState==4)
		{
			pm.x=LOWORD(lParam);
			pm.y=HIWORD(lParam);
			if (pm.x>363&&pm.x<430&&pm.y>392&&pm.y<450)
			{
				g_GameState=0;
				for (int i=0;i<g_iNum;i++)
				{
					if (g_Body[i].exsit!=false)
					{
						g_Body[i].exsit=false;
					}
				}
				g_iNum=0;
				g_Head.x=10;
				g_Head.y=10;
				g_Time=200;
				g_Level=0;
				g_Score=0;
			}
		}
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);//���ٴ��ڣ�������һ��WM_DESTROY��Ϣ
			break;
		case 87://W��
			g_Director=0;
			break;
		case 68://D��
			g_Director=1;
			break;
		case 83://S��
			g_Director=2;
			break;
		case 65://A��
			g_Director=3;
			break;
		case VK_SPACE:
			if (g_Space==1)
			{
				g_Space=0;
			}
			else if (g_Space==0)
			{
				g_Space=1;
			}
			break;
		}		
		break;
		
	case WM_DESTROY://����Ǵ���������Ϣ
		Game_CleanUp(hwnd);//�����˳�ǰ����Դ����
		PostQuitMessage(NULL);//��ϵͳ�����и��߳�����ֹ����ͨ��������ӦWM_DESTROY��Ϣ
		break;

 	default:
 		return DefWindowProc(hwnd,message,wParam,lParam);//����Ĭ�ϵĴ��ڹ���
// 		//�ú���ȷ��ÿһ����Ϣ�õ�����	
	}
	return 0;//�����˳�
}

//-----------------��Game_Init()������-------------------------
//��ʼ������������һЩ�򵥳�ʼ��
BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));
	g_hdc = GetDC(hwnd);//��ȡ�豸�������
	g_mdc=CreateCompatibleDC(g_hdc);
	g_sdc=CreateCompatibleDC(g_hdc);

	HBITMAP map=CreateCompatibleBitmap(g_hdc,WINDOW_WIDTH,WINDOW_HEIGHT);
	SelectObject(g_mdc,map);
//-------------��1 ����λͼ��-----------
	g_hBg=(HBITMAP)LoadImage(NULL,L"image\\bg.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);//���ر���λͼ
	g_hHead=(HBITMAP)LoadImage(NULL,L"image\\Head.bmp",IMAGE_BITMAP,120,30,LR_LOADFROMFILE);//����Headλͼ
	g_hBody=(HBITMAP)LoadImage(NULL,L"image\\Body.bmp",IMAGE_BITMAP,30,30,LR_LOADFROMFILE);
	g_hFood=(HBITMAP)LoadImage(NULL,L"image\\Food.bmp",IMAGE_BITMAP,30,30,LR_LOADFROMFILE);
	g_hFailure=(HBITMAP)LoadImage(NULL,L"image\\Failure.bmp",IMAGE_BITMAP,360,100,LR_LOADFROMFILE);
	g_hGameStart=(HBITMAP)LoadImage(NULL,L"image\\GameStart.bmp",IMAGE_BITMAP,200,84,LR_LOADFROMFILE);
	g_hRestart=(HBITMAP)LoadImage(NULL,L"image\\GameRestart.bmp",IMAGE_BITMAP,400,300,LR_LOADFROMFILE);
	g_Head.x=10;
	g_Head.y=10;
	g_Head.exsit=true;

	g_Food.x=rand()%19;
	g_Food.y=rand()%19;
	g_Food.exsit=true;

	SetTextColor(g_mdc,RGB(0,0,0));//����������ɫ
	SetBkMode(g_mdc,TRANSPARENT);//���������������Ϊ͸��

	Game_Paint(hwnd);
	return TRUE;
}

//-----------------��Game_Paint()������-------------------------
//�ڴ˺����н��л�ͼ����
void Game_Paint(HWND hwnd)
{
	WCHAR score[10];
	HFONT hFont;
	SelectObject(g_sdc,g_hBg);//��λͼ����ѡ���ڴ�DC
	BitBlt(g_mdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_sdc,0,0,SRCCOPY);//������
	if (g_Space==1)
	{
		if (g_GameState==3)//��Ϸ��ʼ����
		{
			SelectObject(g_sdc,g_hGameStart);
			TransparentBlt(g_mdc,300,250,200,84,g_sdc,0,0,200,84,RGB(255,255,255));//����ʼ����
		}
		else if (g_GameState==4)//��Ϸʧ�����¿�ʼ
		{
			SelectObject(g_sdc,g_hRestart);
			BitBlt(g_mdc,200,150,400,300,g_sdc,0,0,SRCCOPY);//�����¿�ʼͼƬ
			hFont = CreateFont(40,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("����"));
			SelectObject(g_mdc,hFont);//������ѡ���豸����
			swprintf_s(score,L"%d",g_Score);
			TextOut(g_mdc,405,300,score,wcslen(score));
		}
		else if (g_GameState==1)//�����Ϸ����
		{
			SelectObject(g_sdc,g_hFailure);
			BitBlt(g_mdc,180,256,360,100,g_sdc,0,0,SRCCOPY);//����ʧ��	
			g_GameState=4;
		}
		if (g_GameState==0)//��Ϸ��������
		{
			GameState();
			if (GetFood())
			{
				EatFood();
			}
			if (g_Food.exsit==false)//�������ʳ���������ʳ��
			{
				SetFood();
			}
			SelectObject(g_sdc,g_hHead);
			TransparentBlt(g_mdc,g_Head.x*32+16,g_Head.y*32+16,30,30,g_sdc,g_Director*30,0,30,30,RGB(255,0,0));//����ͷ

			if (g_iNum!=0)
			{
				for (int i=0;i<g_iNum;i++)
				{
					if (g_Body[i].exsit==true)
					{
						SelectObject(g_sdc,g_hBody);
						TransparentBlt(g_mdc,g_Body[i].x*32+16,g_Body[i].y*32+16,30,30,g_sdc,0,0,30,30,RGB(255,255,255));//��body
					}
				}
			}
			SelectObject(g_sdc,g_hFood);
			TransparentBlt(g_mdc,g_Food.x*32+16,g_Food.y*32+16,30,30,g_sdc,0,0,30,30,RGB(255,255,255));//��ʳ��
			if ((g_Level==0&&g_Score>100)||(g_Level==1&&g_Score>200)||(g_Level==2&&g_Score>300)||(g_Level==3&&g_Score>400))
			{
				LevelUp();
			}
			GetBodyXY();
			GetHeadXY();
		}
	}
	
	//����÷�

	hFont = CreateFont(20,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("����"));
	SelectObject(g_mdc,hFont);//������ѡ���豸����
	swprintf_s(score,L"%d",g_Score);
	TextOut(g_mdc,690,390,score,wcslen(score));
	swprintf_s(score,L"%d",g_Level);
	TextOut(g_mdc,690,460,score,wcslen(score));

	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);//������Ļ
	g_Pre=GetTickCount();//��¼�˴λ�ͼʱ��
	DeleteObject(hFont);//�ͷ��������
}

//-----------------��Game_CleanUp()������-------------------------
//��Դ������
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hBg);
	DeleteObject(g_hHead);
	DeleteObject(g_hBody);
	DeleteObject(g_hFood);
	DeleteObject(g_hFailure);
	DeleteObject(g_hGameStart);
	DeleteObject(g_hRestart);
	DeleteDC(g_mdc);//��getDC��Ӧ
	DeleteDC(g_sdc);
	ReleaseDC(hwnd,g_hdc);
	return TRUE;
}
void GetHeadXY()
{
	if (g_GameState!=0)
	{
		
	}
	else 
	{
		if (g_Director==0)
		{
			g_Head.y -=1;
		}
		else if (g_Director==1)
		{
			g_Head.x+=1;
		}
		else if (g_Director==2)
		{
			g_Head.y+=1;
		}
		else
		{
			g_Head.x-=1;
		}
	}
}
void GameState()
{
	if(g_Head.x<0||g_Head.x>=19||g_Head.y<0||g_Head.y>=19)
		g_GameState=1;
	else if(Head_Body())
		g_GameState=1;
}
bool GetFood()
{
	int l_x=g_Head.x,l_y=g_Head.y;
	if (g_Director==0)
	{
		l_y=l_y-1;
	}
	else if (g_Director==1)
	{
		l_x=l_x+1;
	}
	else if (g_Director==2)
	{
		l_y=l_y+1;
	}
	else
	{
		l_x=l_x-1;
	}
	if (l_x==g_Food.x && l_y==g_Food.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void EatFood()
{
	g_Food.exsit=false;
	g_Body[g_iNum].exsit=true;
	g_Body[g_iNum].x=g_Head.x;
	g_Body[g_iNum].y=g_Head.y;
	g_Head.x=g_Food.x;
	g_Head.y=g_Food.y;
	g_iNum++;
	g_Score+=10;
}
void GetBodyXY()
{
	if (g_GameState!=0)
	{

	}
	else
	{
		if (g_iNum==1)
		{
			g_Body[0]=g_Head;
		}
		else if(g_iNum>1)
		{
			for (int i=1;i<g_iNum;i++)
			{
				g_Body[i-1]=g_Body[i];
			}
			g_Body[g_iNum-1]=g_Head;
		}
	}
}
bool Head_Body()
{
	for (int i=0;i<g_iNum;i++)
	{
		if (g_Body[i].x==g_Head.x&&g_Body[i].y==g_Head.y)
		{
			return true;
		}
	}
	return false;
}
void SetFood()
{
	do 
	{
		g_Food.x=rand()%18;
		g_Food.y=rand()%18;
		g_Food.exsit=true;
	} while (Body_Food());
}
void LevelUp()
{
	if(g_Time>80)
	{
		g_Time-=40;
		g_Level++;
	}
}
bool Body_Food()
{
	bool bf=false;
	for (int i=0;i<g_iNum;i++)
	{
		if (g_Body[i].x==g_Food.x&&g_Body[i].y==g_Food.y)
		{
			bf=true;
		}
	}
	if (g_Head.x==g_Food.x&&g_Head.y==g_Food.y)
	{
		bf=true;
	}
	return bf;
}