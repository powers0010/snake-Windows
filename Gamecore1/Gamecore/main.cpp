//------------------------【程序说明】---------------------
//程序名：GameDemoCore
//GDI游戏开发框架


//------------------------【头文件区】---------------------
#include <Windows.h>
#include <stdio.h>
#include <ctime>

//------------------------【库文件部分】-------------------
#pragma comment(lib,"Winmm.lib")//playsound函数的库
#pragma comment(lib,"Msimg32.lib")//transparentBlt函数的库
//------------------------【宏定义区】---------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 640
#define WINDOW_TITLE  L"贪吃蛇1.0"

//结构体
struct SNAKE	//坐标
{
	int x;
	int y;
	bool exsit;
};

//------------------------【全局变量声明区】---------------
HDC g_hdc=NULL,g_mdc=NULL,g_sdc=NULL;		//设备环境DC及内存环境DC
HBITMAP g_hBg=NULL,g_hHead=NULL,g_hBody=NULL,g_hFood=NULL,g_hFailure=NULL,g_hGameStart=NULL,g_hRestart=NULL;//位图资源
DWORD g_Pre=0,g_Now=0;		//记录时间，g_Pre记录上一次绘图时间，g_Now记录此次准备绘图时间
int g_iNum=0;				//记录Body数量
SNAKE g_Head,g_Food,g_Body[100]={0};	//蛇头坐标及蛇身坐标
int g_Director=0;			//记录方向 上0  右1  下2  左3
int g_GameState=3;			//游戏状态 0游戏进行中、1游戏失败界面、3游戏开始界面 、4 游戏重新开始界面
DWORD g_Time=200;	//前进速度 每20一个难度
int g_Score=0;//分数
int g_Level=0;//等级
int g_Space=1;//暂停功能
//-----------------------【全局函数声明区】-----------------
LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL Game_Init(HWND hwnd);//在此函数中进行资源初始化
VOID Game_Paint(HWND hwnd);//在此函数中进行绘图代码的书写
BOOL Game_CleanUp(HWND hwnd);//在此函数进行资源的清理
void GetHeadXY();//计算贪吃蛇坐标
void GetBodyXY();//获得身体坐标
void GameState();//判断游戏状态 
bool GetFood();//判断是否吃了食物
void EatFood();//进行吃食物后的坐标变换
bool Head_Body();//检查头和身是否触碰
void SetFood();//设置food坐标
bool Body_Food();//检查产生的food和body是否重叠
void LevelUp();//加速
//------------------------【WinMain()函数】------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowcmd)
{
	//【1】四部曲之一：窗口设计
	WNDCLASSEX wndclass={0};//定义窗口类
	wndclass.cbSize=sizeof(WNDCLASSEX);//设计结构体字节数大小
	wndclass.style=CS_HREDRAW|CS_VREDRAW;//设计窗口样式
	wndclass.lpfnWndProc=WndProc;//设置指向窗口过程函数的指针
	wndclass.cbClsExtra=0;//窗口类的附加内存
	wndclass.cbWndExtra=0;//窗口的附加内存
	wndclass.hInstance=hInstance;//指定包含窗口过程的程序的实例句柄
	wndclass.hIcon=(HICON)::LoadImage(NULL,L"image\\icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);//指定窗口类的光标句柄
	wndclass.hbrBackground=(HBRUSH)::GetStockObject(GRAY_BRUSH);//指定一个灰色笔刷句柄
	wndclass.lpszMenuName=NULL;//用一个以空字符结尾的字符串，指定菜单资源的名字
	wndclass.lpszClassName=L"贪吃蛇1.0";//用一个以空字符结尾的字符串,指定窗口类的名字

	//【2】四部曲之二：注册窗口类
	if(!RegisterClassEx(&wndclass))//窗口注册，RegisterClassEx函数注册成功返回窗口类型的标识号;如果函数失败，返回值为0
		return -1;

	//【3】四部曲之三：正式创建窗口
	HWND hwnd=CreateWindow( //如果函数成功，返回值为新窗口的句柄:如果函数失败，返回值为NULL。若想获得更多错误信息，请调用GetLastError函数。
		L"贪吃蛇1.0",	//之前创建的窗口类的名字
		WINDOW_TITLE,	//指向一个指定窗口名的空结束的字符串指针。
		WS_OVERLAPPEDWINDOW,//指定创建窗口的风格
		CW_USEDEFAULT,//指定窗口的初始水平位置
		CW_USEDEFAULT,//指定窗口的初始垂直位
		WINDOW_WIDTH,//以设备单元指明窗口的宽度
		WINDOW_HEIGHT,//以设备单元指明窗口的高度
		NULL,//指向被创建窗口的父窗口或所有者窗口的句柄
		NULL,//菜单句柄，或依据窗口风格指明一个子窗口标识
		hInstance,//与窗口相关联的模块实例的句柄
		NULL//指向一个值的指针，该值传递给窗口WM_CREATE消息
		);

	//【4】四部曲之四：窗口的移动、显示和更新
	MoveWindow(hwnd,250,20,WINDOW_WIDTH,WINDOW_HEIGHT,true);//调整窗口显示的位置,位于左上角（250，80）处

	ShowWindow(hwnd,nShowcmd);//调用ShowWindow函数显示窗口
	UpdateWindow(hwnd);//对窗口进行更新

	//游戏资源的初始化，如果初始化失败，弹出消息框，并返回FALSE
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd,L"资源初始化失败",L"消息窗口",0);
		return FALSE;
	}
	//播放背景音乐
	PlaySound(L"image\\demo01.wav",NULL,SND_FILENAME|SND_ASYNC|SND_LOOP);//循环播放背景音乐

	//【5】第五步：消息循环过程	
	MSG msg={0};
	while(msg.message!=WM_QUIT)//使用while循环，如果消息不是WM_QUIT就继续循环
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))//如果消息可得到，返回非零值;如果没有消息可得到，返回值是零。
			//BOOL PeekMessage(LPMSG IpMsg,HWND hWnd,UINT wMSGfilterMin,UINT wMsgFilterMax,UINT wRemoveMsg);
		{
			TranslateMessage(&msg);//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);//分发一个消息给窗口程序,消息被分发到回调函数(过程函数)，作用是消息传递给操作系统，然后操作系统去调用我们的回调函数
		}
		else
		{
			g_Now=GetTickCount();//获取当前时间
			if((g_Now-g_Pre)>=g_Time)//当此次循环与上次绘图时间相差0.1s时进行操作
				Game_Paint(hwnd);
		}
	}

	//【6】第六步：窗口类的注销
	UnregisterClass(L"DreamofGame",hInstance);//程序准备结束，注销窗口类

	return 0;
}

//---------------------------【WndProc】函数 ---------------
//窗口过程函数，对窗口消息进行处理
//---------------------------------------------------------
LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT paintStruct;//定义一个PAINTSTRUCT结构体记录绘制消息
	POINT pm;
	switch(message)//switch语句开始
	{
	case WM_PAINT:
		g_hdc=BeginPaint(hwnd,&paintStruct);//指定窗口进行绘图准备工作，并用和绘图有关的信息填充到paintStruct结构体中
		Game_Paint(hwnd);
		EndPaint(hwnd,&paintStruct);//EndPaint函数标记指定窗口绘图过程结束
		ValidateRect(hwnd,NULL);//更新客户区的显示
		break;
	case WM_LBUTTONDOWN:
		if (g_GameState==3)//如果在初始界面
		{
			pm.x=LOWORD(lParam);
			pm.y=HIWORD(lParam);
			if (pm.x>300&&pm.x<500&&pm.y>250&&pm.y<334)//如果按下开始游戏
			{
				g_GameState=0;//开始游戏
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
			DestroyWindow(hwnd);//销毁窗口，并发送一条WM_DESTROY消息
			break;
		case 87://W键
			g_Director=0;
			break;
		case 68://D键
			g_Director=1;
			break;
		case 83://S键
			g_Director=2;
			break;
		case 65://A键
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
		
	case WM_DESTROY://如果是窗口销毁信息
		Game_CleanUp(hwnd);//进行退出前的资源清理
		PostQuitMessage(NULL);//向系统表明有个线程有终止请求。通常用来响应WM_DESTROY消息
		break;

 	default:
 		return DefWindowProc(hwnd,message,wParam,lParam);//调用默认的窗口过程
// 		//该函数确保每一个消息得到处理。	
	}
	return 0;//正常退出
}

//-----------------【Game_Init()函数】-------------------------
//初始化函数，进行一些简单初始化
BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));
	g_hdc = GetDC(hwnd);//获取设备环境句柄
	g_mdc=CreateCompatibleDC(g_hdc);
	g_sdc=CreateCompatibleDC(g_hdc);

	HBITMAP map=CreateCompatibleBitmap(g_hdc,WINDOW_WIDTH,WINDOW_HEIGHT);
	SelectObject(g_mdc,map);
//-------------【1 加载位图】-----------
	g_hBg=(HBITMAP)LoadImage(NULL,L"image\\bg.bmp",IMAGE_BITMAP,WINDOW_WIDTH,WINDOW_HEIGHT,LR_LOADFROMFILE);//加载背景位图
	g_hHead=(HBITMAP)LoadImage(NULL,L"image\\Head.bmp",IMAGE_BITMAP,120,30,LR_LOADFROMFILE);//加载Head位图
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

	SetTextColor(g_mdc,RGB(0,0,0));//设置文字颜色
	SetBkMode(g_mdc,TRANSPARENT);//设置文字输出背景为透明

	Game_Paint(hwnd);
	return TRUE;
}

//-----------------【Game_Paint()函数】-------------------------
//在此函数中进行绘图工作
void Game_Paint(HWND hwnd)
{
	WCHAR score[10];
	HFONT hFont;
	SelectObject(g_sdc,g_hBg);//将位图对象选入内存DC
	BitBlt(g_mdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_sdc,0,0,SRCCOPY);//贴背景
	if (g_Space==1)
	{
		if (g_GameState==3)//游戏初始界面
		{
			SelectObject(g_sdc,g_hGameStart);
			TransparentBlt(g_mdc,300,250,200,84,g_sdc,0,0,200,84,RGB(255,255,255));//贴初始界面
		}
		else if (g_GameState==4)//游戏失败重新开始
		{
			SelectObject(g_sdc,g_hRestart);
			BitBlt(g_mdc,200,150,400,300,g_sdc,0,0,SRCCOPY);//贴重新开始图片
			hFont = CreateFont(40,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("宋体"));
			SelectObject(g_mdc,hFont);//将字体选入设备环境
			swprintf_s(score,L"%d",g_Score);
			TextOut(g_mdc,405,300,score,wcslen(score));
		}
		else if (g_GameState==1)//如果游戏结束
		{
			SelectObject(g_sdc,g_hFailure);
			BitBlt(g_mdc,180,256,360,100,g_sdc,0,0,SRCCOPY);//贴出失败	
			g_GameState=4;
		}
		if (g_GameState==0)//游戏继续运行
		{
			GameState();
			if (GetFood())
			{
				EatFood();
			}
			if (g_Food.exsit==false)//如果吃了食物，重新设置食物
			{
				SetFood();
			}
			SelectObject(g_sdc,g_hHead);
			TransparentBlt(g_mdc,g_Head.x*32+16,g_Head.y*32+16,30,30,g_sdc,g_Director*30,0,30,30,RGB(255,0,0));//贴蛇头

			if (g_iNum!=0)
			{
				for (int i=0;i<g_iNum;i++)
				{
					if (g_Body[i].exsit==true)
					{
						SelectObject(g_sdc,g_hBody);
						TransparentBlt(g_mdc,g_Body[i].x*32+16,g_Body[i].y*32+16,30,30,g_sdc,0,0,30,30,RGB(255,255,255));//贴body
					}
				}
			}
			SelectObject(g_sdc,g_hFood);
			TransparentBlt(g_mdc,g_Food.x*32+16,g_Food.y*32+16,30,30,g_sdc,0,0,30,30,RGB(255,255,255));//贴食物
			if ((g_Level==0&&g_Score>100)||(g_Level==1&&g_Score>200)||(g_Level==2&&g_Score>300)||(g_Level==3&&g_Score>400))
			{
				LevelUp();
			}
			GetBodyXY();
			GetHeadXY();
		}
	}
	
	//输出得分

	hFont = CreateFont(20,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("宋体"));
	SelectObject(g_mdc,hFont);//将字体选入设备环境
	swprintf_s(score,L"%d",g_Score);
	TextOut(g_mdc,690,390,score,wcslen(score));
	swprintf_s(score,L"%d",g_Level);
	TextOut(g_mdc,690,460,score,wcslen(score));

	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);//贴到屏幕
	g_Pre=GetTickCount();//记录此次绘图时间
	DeleteObject(hFont);//释放字体对象
}

//-----------------【Game_CleanUp()函数】-------------------------
//资源清理函数
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hBg);
	DeleteObject(g_hHead);
	DeleteObject(g_hBody);
	DeleteObject(g_hFood);
	DeleteObject(g_hFailure);
	DeleteObject(g_hGameStart);
	DeleteObject(g_hRestart);
	DeleteDC(g_mdc);//与getDC对应
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