#include<iostream>
#include<stdio.h>
#include<string>
#include<graphics.h>
#include<Windows.h>
#include<time.h>
#include<fstream>
#include<Mmsystem.h>
#pragma comment(lib,"winmm.lib")
#define move_speed 1
#define maxn 10000
#define INFINITY 1e6
#define UNVISITED 0
#define VISITED 1
using namespace std;
int station_SUM;
double twoStation_length[1000][1000];
string subway_data[maxn];
int map_subwayStation_Place[maxn][2];//地图上地铁站的坐标
string eachSubway_name[25][50];
bool first_run = true;//第一次进入menu，用来控制音乐播放
bool function2_display_flag[1000];//标记function2中经过的地铁站
template<class T>
class stack
{
	private:
		int maxSize;
		int currentSize;
		int Top;
		T *arr;
	public:
		stack(int _maxSize);
		~stack();
		void push(T num);
		void pop();
		T top();
		int size();
};
template<class T>
stack<T>::stack(int _maxSize)
{
	maxSize = _maxSize;
	currentSize = 0;
	Top = -1;
	arr = new T[_maxSize];
}
template<class T>
stack<T>::~stack()
{
	delete[] arr;
}
template<class T>
void stack<T>::push(T num)
{
	if (Top != maxSize - 1)
	{
		arr[++Top] = num;
		currentSize++;
	}
}
template<class T>
void stack<T>::pop()
{
	if (Top != -1)
	{
		currentSize--;
		Top--;
	}
}
template<class T>
T stack<T>::top()
{
	if (Top != -1)
	{
		return arr[Top];
	}
}
template<class T>
int stack<T>::size()
{
	return currentSize;
}
stack<string> myStack_stationName(100);
stack<string> myStack_stationLine(100);
class Edge
{
public:
	int from;
	int to;
	double weight;
	Edge(int _from = -1, int _to = -1, double _weight = 0)
	{
		from = _from;
		to = _to;
		weight = _weight;
	}
};
class Graph
{
public:
	int vertexNum;
	int edgeNum;
	int* mark;
	int* indegree;
	Graph(int _vertexNum)
	{
		vertexNum = _vertexNum;
		edgeNum = 0;
		mark = new int[_vertexNum];
		indegree = new int[_vertexNum];
		for (int i = 0; i < _vertexNum; i++)
		{
			mark[i] = UNVISITED;
			indegree[i] = 0;
		}
	}
	~Graph()
	{
		delete[] mark;
		delete[] indegree;
	}
	bool isEdge(Edge e)
	{
		if (e.weight > 0 && e.weight < INFINITY && e.to >= 0)
			return true;
		return false;
	}
};
struct listUnit
{
	int subwayLine_num;//地铁线路编号
	string subwayLine_name;//地铁线路名称
	int subwayStation_num;//地铁站编号
	string subwayStation_name;//地铁站名称
	double length;
};
template<class T>
class Link
{
public:
	T element;
	Link* next;
	Link(T& _element, Link* _next)
	{
		element = _element;
		next = _next;
	}
	Link(Link* _next)
	{
		next = _next;
	}
};
template<class T>
class linkList
{
public:
	Link<T>* head;
	linkList()
	{
		head = new Link<T>(NULL);
	}
};
class _Graph :public Graph
{
public:
	linkList<listUnit>* graList;
	_Graph(int _vertexNum) :Graph(_vertexNum)
	{
		graList = new linkList<listUnit>[_vertexNum];
	}
	Edge firstEdge(int oneVertex)
	{
		Edge resEdge;
		resEdge.from = oneVertex;
		Link<listUnit>* temp = graList[oneVertex].head;
		if (temp->next)
		{
			resEdge.to = temp->next->element.subwayStation_num;
			resEdge.weight = temp->next->element.length;
		}
		return resEdge;
	}
	Edge nextEdge(Edge preEdge)
	{
		Edge resEdge;
		resEdge.from = preEdge.from;
		Link<listUnit>* temp = graList[preEdge.from].head;
		while (temp->next && temp->next->element.subwayStation_num <= preEdge.to)
		{
			temp = temp->next;
		}
		if (temp->next)
		{
			resEdge.to = temp->next->element.subwayStation_num;
			resEdge.weight = temp->next->element.length;
		}
		return resEdge;
	}
	void setEdge(int _from, int _to, int _subwayLine_num, string _subwayLine_name, string _subwayStation_name, double _weight)//_to即为subwayStation_num
	{
		Link<listUnit>* temp = graList[_from].head;
		while (temp->next && temp->next->element.subwayStation_num < _to)
		{
			temp = temp->next;
		}
		if (temp->next == NULL)
		{
			temp->next = new Link<listUnit>(NULL);
			temp->next->element.subwayLine_num = _subwayLine_num;
			temp->next->element.subwayLine_name = _subwayLine_name;
			temp->next->element.subwayStation_num = _to;
			temp->next->element.subwayStation_name = _subwayStation_name;
			temp->next->element.length = _weight;
			edgeNum++;
			indegree[_to]++;
			return;
		}
		if (temp->next->element.subwayStation_num == _to)
		{
			temp->next->element.subwayLine_num = _subwayLine_num;
			temp->next->element.subwayLine_name = _subwayLine_name;
			temp->next->element.subwayStation_name = _subwayStation_name;
			temp->next->element.length = _weight;
			return;
		}
		if (temp->next->element.subwayStation_num > _to)
		{
			Link<listUnit>* other = temp->next;
			temp->next = new Link<listUnit>(NULL);
			temp->next->element.subwayLine_num = _subwayLine_num;
			temp->next->element.subwayLine_name = _subwayLine_name;
			temp->next->element.subwayStation_num = _to;
			temp->next->element.subwayStation_name = _subwayStation_name;
			temp->next->element.length = _weight;
			temp->next->next = other;
			edgeNum++;
			indegree[_to]++;
			return;
		}
	}
	void delEdge(int _from, int _to)
	{
		Link<listUnit>* temp = graList[_from].head;
		while (temp->next && temp->next->element.subwayStation_num < _to)
			temp = temp->next;
		if (!temp->next) return;
		if (temp->next->element.subwayStation_num > _to) return;
		if (temp->next->element.subwayStation_num == _to)
		{
			Link<listUnit>* other = temp->next->next;
			delete temp->next;
			temp->next = other;
			edgeNum--;
			indegree[_to]--;
			return;
		}
	}
};
_Graph g(maxn);
class Dist
{
public:
	int index;
	double length;
	int preVertex;
};
int price_counting(double length)
{
	if (length == 0) return 0;
	else if (length > 0 && length <= 6) return 3;
	else if (length > 6 && length <= 12) return 4;
	else if (length > 12 && length <= 22) return 5;
	else if (length > 22 && length <= 32) return 6;
	else if (length > 32 && length <= 52) return 7;
	else if (length > 52 && length <= 72) return 8;
	else if (length > 72 && length <= 92) return 9;
}
int minVertex(_Graph& g, Dist* D)
{
	int flag = 0;
	int max_num = INFINITY;
	for (int i = 0; i <= station_SUM - 1; i++)
	{
		if (g.mark[i] == UNVISITED && D[i].length < max_num)
		{
			flag = i;
			max_num = D[i].length;
		}
	}
	return flag;
}
void search(int s, int vertex, Dist* D)
{
	myStack_stationName.push(subway_data[vertex]);
	for (int i = 0; i < station_SUM; i++)
	{
		Link<listUnit>* temp = g.graList[i].head;
		while (temp->next)
		{
			temp = temp->next;
			if (temp->element.subwayStation_name == subway_data[vertex])
			{
				myStack_stationLine.push(temp->element.subwayLine_name);
				goto end;
			}
		}
	}
	end:
	if (vertex != s) search(s, D[vertex].preVertex, D);
}
int Dijkstra(_Graph& g, int s, Dist* D, int function, int station_line, int e, int paint_beginPlace)
{
	int temp_price[10];
	for (int i = 0; i <= station_SUM - 1; i++)
	{
		g.mark[i] = UNVISITED;
		D[i].index = i;
		D[i].length = INFINITY;
		D[i].preVertex = s;
	}
	D[s].length = 0;
	for (int i = 0; i <= station_SUM - 1; i++)
	{
		int v = minVertex(g, D);
		if (D[v].length == INFINITY) break;
		g.mark[v] = VISITED;
		for (Edge e = g.firstEdge(v); g.isEdge(e); e = g.nextEdge(e))
		{
			if (g.mark[e.to] == UNVISITED && D[e.to].length > (D[v].length + e.weight))
			{
				D[e.to].length = D[v].length + e.weight;
				D[e.to].preVertex = v;
			}
		}
	}
	if (function == 1)
	{
		settextcolor(BLACK);
		settextstyle(25, 0, "行楷");
		setlinecolor(RED);
		setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 5);
		line(190, 130, 190, 540);
		int count = 0;
		int max_length;
		int temp_price[10];
		int putX = 0;
		int putY = 0;
		for (int NUM = 0; NUM < 50; NUM++)
		{
			max_length = -1;
			if (eachSubway_name[station_line][NUM] == "NULL") break;
			for (int i = 0; i <= station_SUM - 1; i++)
			{
				if (subway_data[D[i].index] == eachSubway_name[station_line][NUM])
				{
					if (count && count % 10 == 0 && count / 10 != 4)
					{
						line(190 + 190 * (putY + 1), 130, 190 + 190 * (putY + 1), 540);
					}
					putX = count % 10;
					putY = count / 10;
					const char* newStr = subway_data[i].data();
					outtextxy(10 + 190 * putY, 134 + putX * 40, newStr);
					int price = price_counting(D[i].length);
					temp_price[count % 10] = price_counting(D[i].length);
					max_length = max(max_length, eachSubway_name[station_line][NUM].length());
					TCHAR price_NUM[5];
					sprintf(price_NUM, _T("%d"), price);
					outtextxy(170 + 190 * putY, 134 + putX * 40, price_NUM);
					count++;
					break;
				}
			}
		}
		return 0;
	}
	else if (function == 2)
	{
		settextcolor(BLACK);
		settextstyle(30, 0, "行楷");
		while (myStack_stationName.size()) myStack_stationName.pop();
		while (myStack_stationLine.size()) myStack_stationLine.pop();
		search(s, e, D);
		int tempCount = 0;
		for (int i = 0; i < station_SUM; i++)
		{
			function2_display_flag[i] = false;
		}
		while (myStack_stationName.size())
		{
			string current_stationName;
			current_stationName = myStack_stationName.top();
			for (int i = 0; i < station_SUM; i++)
			{
				if (current_stationName == subway_data[i])
				{
					function2_display_flag[i] = true;
					break;
				}
			}
			myStack_stationName.pop();
			const char* newStr = current_stationName.data();
			settextstyle(30, 0, "行楷");
			settextcolor(BLACK);
			outtextxy(10 + (tempCount % 4) * 240 + (190 - current_stationName.length() * 15) / 2, 80 + (tempCount / 4) * 40, newStr);
			settextcolor(RED);
			if (myStack_stationName.size()) outtextxy(10 + (190 + (tempCount % 4) * 240), 80 + (tempCount / 4) * 40, "->");
			tempCount++;
		}
		return 0;
	}
	else if (function == 3)
	{
		int begin_place;
		int end_place;
		settextcolor(BLACK);
		settextstyle(30, 0, "行楷");
		while (myStack_stationName.size()) myStack_stationName.pop();
		while (myStack_stationLine.size()) myStack_stationLine.pop();
		search(s, e, D);
		int tempCount = 0;
		IMAGE begin_label, change_label, end_label, station_arrow;
		loadimage(&begin_label, ".\\地铁票价系统相关图片\\begin_label.jpg", 40, 40);
		loadimage(&change_label, ".\\地铁票价系统相关图片\\change_label.jpg", 40, 40);
		loadimage(&end_label, ".\\地铁票价系统相关图片\\end_label.jpg", 40, 40);
		loadimage(&station_arrow, ".\\地铁票价系统相关图片\\station_arrow.jpg", 30, 90);
		settextcolor(BLACK);
		settextstyle(40, 0, "行楷");
		while (myStack_stationName.size()) myStack_stationName.pop();
		while (myStack_stationLine.size()) myStack_stationLine.pop();
		search(s, e, D);
		string current_stationLine;
		string nextStationLine;
		int count = 0;
		int sum = 0;
		int length = myStack_stationName.size();
		string stationName[1000];
		const char* newStr;
		int current_place = 0;
		while (myStack_stationName.size())
		{
			stationName[current_place++] = myStack_stationName.top();
			myStack_stationName.pop();
		}
		putimage(300, paint_beginPlace + 80, &begin_label);
		begin_place = paint_beginPlace + 80;
		newStr = stationName[0].data();
		outtextxy(350, paint_beginPlace + 80, newStr);
		settextcolor(RED);
		outtextxy(0, 80, "起点：");
		settextcolor(BLACK);
		outtextxy(0, 120, newStr);
		settextcolor(RED);
		outtextxy(0, 160, "终点：");
		settextcolor(BLACK);
		newStr = stationName[length - 1].data();
		outtextxy(0, 200, newStr);
		settextcolor(RED);
		outtextxy(0, 240, "总站数：");
		settextcolor(BLACK);
		TCHAR newNum[5];
		sprintf(newNum, _T("%d"), length);
		outtextxy(135, 240, newNum);
		for (int i = 0; i <= length - 1; i++)
		{
			if (i == 0)
			{
				for (int j = 0; j < station_SUM; j++)
				{
					if (subway_data[j] == stationName[0])
					{
						Link<listUnit>* temp = g.graList[j].head;
						while (temp->next)
						{
							temp = temp->next;
							if (temp->element.subwayStation_name == stationName[1])
							{
								current_stationLine = temp->element.subwayLine_name;
								break;
							}
						}
						break;
					}
				}	
			}
			else if (i == length - 1)
			{
				sum++;
				count++;
				putimage(305, paint_beginPlace + 125 + (sum - 1) * 140, &station_arrow);
				settextstyle(20, 0, "行楷");
				newStr = current_stationLine.data();
				outtextxy(350, paint_beginPlace + 140 + (sum - 1) * 140, newStr);
				TCHAR newNum[5];
				sprintf(newNum, _T("%d"), count);
				outtextxy(350, paint_beginPlace + 160 + (sum - 1) * 140, "站数：");
				outtextxy(410, paint_beginPlace + 160 + (sum - 1) * 140, newNum);
				putimage(300, paint_beginPlace + 80 + 140 * sum, &end_label);
				end_place = paint_beginPlace + 80 + 140 * sum;
				newStr = stationName[i].data();
				settextstyle(40, 0, "行楷");
				outtextxy(350, paint_beginPlace + 80 + 140 * sum, newStr);
				settextcolor(RED);
				outtextxy(0, 280, "换乘数：");
				settextcolor(BLACK);
				TCHAR newNum1[5];
				sprintf(newNum1, _T("%d"), sum - 1);
				outtextxy(135, 280, newNum1);
				settextcolor(RED);
				outtextxy(0, 320, "距离：");
				settextcolor(BLACK);
				TCHAR newNum2[5];
				int integer_part = D[e].length;
				int decimal_part = (D[e].length - integer_part) * 100;
				sprintf(newNum2, _T("%d"), integer_part);
				if (integer_part >= 10) outtextxy(105, 320, newNum2);
				else
				{
					outtextxy(105, 320, '0');
					outtextxy(125, 320, newNum2);
				}
				outtextxy(146, 320, '.');
				sprintf(newNum2, _T("%d"), decimal_part);
				if (decimal_part >= 10) outtextxy(158, 320, newNum2);
				else
				{
					outtextxy(158, 320, '0');
					outtextxy(178, 320, newNum2);
				}
				settextcolor(RED);
				outtextxy(0, 360, "总票价：");
				settextcolor(BLACK);
				TCHAR newNum3[5];
				sprintf(newNum3, _T("%d"), price_counting(D[e].length));
				outtextxy(135, 360, newNum3);
			}
			else
			{
				count++;
				for (int j = 0; j < station_SUM; j++)
				{
					if (subway_data[j] == stationName[i])
					{
						Link<listUnit>* temp = g.graList[j].head;
						while (temp->next)
						{
							temp = temp->next;
							if (temp->element.subwayStation_name == stationName[i+1])
							{
								nextStationLine = temp->element.subwayLine_name;
								if (nextStationLine != current_stationLine)
								{
									sum++;
									putimage(305, paint_beginPlace + 125 + (sum - 1) * 140, &station_arrow);
									settextstyle(20, 0, "行楷");
									newStr = current_stationLine.data();
									outtextxy(350, paint_beginPlace + 140 + (sum - 1) * 140, newStr);
									TCHAR newNum[5];
									sprintf(newNum, _T("%d"), count);
									outtextxy(350, paint_beginPlace + 160 + (sum - 1) * 140, "站数：");
									outtextxy(410, paint_beginPlace + 160 + (sum - 1) * 140, newNum);
									putimage(300, paint_beginPlace + 80 + 140 * sum, &change_label);
									newStr = stationName[i].data();
									settextstyle(40, 0, "行楷");
									outtextxy(350, paint_beginPlace + 80 + 140 * sum, newStr);
									count = 0;
									current_stationLine = nextStationLine;
								}
							}
						}
						break;
					}
				}
			}
		}
		return end_place - begin_place;
	}
}
void newSleep(int ms)
{
	static clock_t last = clock();		
	last += ms * CLOCKS_PER_SEC / 1000;
	if (clock() > last)	last = clock();
	else
	{
		while (clock() < last) Sleep(1);
	}				
}
void web_visit(char webName[])
{
	HWND hWnd = GetConsoleWindow();
	ShellExecute(NULL, TEXT("open"), TEXT(webName), TEXT(""), NULL, SW_HIDE);
}
int menu(int* sound)
{
	IMAGE background, title_1, title_2, titleWhenClicked, sound_open, sound_close, information, map;
	loadimage(&background, ".\\地铁票价系统相关图片\\background.jpg", 960, 540);
	loadimage(&title_1, ".\\地铁票价系统相关图片\\title.jpg", 405, 84);
	loadimage(&title_2, ".\\地铁票价系统相关图片\\title.jpg", 192, 54);
	loadimage(&titleWhenClicked, ".\\地铁票价系统相关图片\\titleWhenClicked.jpg", 192, 54);
	loadimage(&sound_open, ".\\地铁票价系统相关图片\\sound_open.jpg", 60, 60);
	loadimage(&sound_close, ".\\地铁票价系统相关图片\\sound_close.jpg", 60, 60);
	loadimage(&information, ".\\地铁票价系统相关图片\\information.jpg", 60, 60);
	loadimage(&map, ".\\地铁票价系统相关图片\\map.jpg", 60, 60);
	putimage(0, 0, &background);
	settextcolor(BLACK);
	settextstyle(72, 0, "宋体");
	for (int i = 456; i >= 60; i -= 2 * move_speed)
	{
		IMAGE temp;
		getimage(&temp, 272, i, 687, i + 84);
		BeginBatchDraw();
		putimage(277, i, &title_1);
		outtextxy(286, i + 5, "地 铁 查 询");
		FlushBatchDraw();
		newSleep(1);
		if (i == 60) break;
		putimage(272, i, &temp);
	}
	EndBatchDraw();
	if (*sound && first_run)
	{
		first_run = false;
		PlaySound(".\\地铁票价系统相关音频\\bk_music.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	}
	settextstyle(48, 0, "宋体");
	for (int i = 0; i <= 384; i += move_speed)
	{
		IMAGE temp1, temp2, temp3;
		setcolor(BLACK);
		getimage(&temp1, i, 180, i + 192, 234);
		getimage(&temp2, 768 - i, 250, 960 - i, 304);
		getimage(&temp3, i, 320, i + 192, 374);
		BeginBatchDraw();
		putimage(i, 180, &title_2);
		putimage(768 - i, 250, &title_2);
		putimage(i, 320, &title_2);
		FlushBatchDraw();
		newSleep(1);
		if (i == 384) break;
		putimage(i, 180, &temp1);
		putimage(768 - i, 250, &temp2);
		putimage(i, 320, &temp3);
	}
	EndBatchDraw();
	FlushMouseMsgBuffer();
	outtextxy(384, 180, "开始查询");
	outtextxy(384, 250, "  帮助  ");
	outtextxy(384, 320, "  退出  ");
	if (*sound == 1)
	{
		putimage(885, 465, &sound_open);
	}
	else
	{
		putimage(885, 465, &sound_close);
	}
	putimage(15, 465, &information);
	putimage(885, 15, &map);
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 384 && m.x < 576 && m.y>180 && m.y < 234)//选择开始查询功能。
		{
			putimage(384, 180, &titleWhenClicked);
			outtextxy(384, 180, "开始查询");
			Sleep(100);
			putimage(384, 180, &title_2);
			outtextxy(384, 180, "开始查询");
			Sleep(100);
			return 1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 384 && m.x < 576 && m.y>250 && m.y < 304)//选择帮助功能。
		{
			putimage(384, 250, &titleWhenClicked);
			outtextxy(384, 250, "  帮助  ");
			Sleep(100);
			putimage(384, 250, &title_2);
			outtextxy(384, 250, "  帮助  ");
			Sleep(100);
			return 2;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 384 && m.x < 576 && m.y>320 && m.y < 374)//选择退出功能。
		{
			putimage(384, 320, &titleWhenClicked);
			outtextxy(384, 320, "  退出  ");
			Sleep(100);
			putimage(384, 320, &title_2);
			outtextxy(384, 320, "  退出  ");
			Sleep(100);
			return 3;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 885 && m.x < 945 && m.y>465 && m.y < 525)//选择开关音乐功能。
		{
			if (*sound == 1)
			{
				*sound = 0;
				putimage(885, 465, &sound_close);
				PlaySound(NULL, NULL, SND_FILENAME);
			}
			else
			{
				*sound = 1;
				putimage(885, 465, &sound_open);
				PlaySound(".\\地铁票价系统相关音频\\bk_music.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
			}
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 15 && m.x < 75 && m.y>465 && m.y < 525)//选择信息功能。
		{
			return 4;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 885 && m.x < 945 && m.y>15 && m.y < 75)//选择地图功能。
		{
			return 5;
		}
		else if (m.x > 384 && m.x < 576 && m.y>180 && m.y < 234)
		{
			settextcolor(RED);
			settextstyle(48, 0, "宋体");
			outtextxy(384, 180, "开始查询");
		}
		else if (m.x > 384 && m.x < 576 && m.y>250 && m.y < 304)
		{
			settextcolor(RED);
			settextstyle(48, 0, "宋体");
			outtextxy(384, 250, "  帮助  ");
		}
		else if (m.x > 384 && m.x < 576 && m.y>320 && m.y < 374)
		{
			settextcolor(RED);
			settextstyle(48, 0, "宋体");
			outtextxy(384, 320, "  退出  ");
		}
		else
		{
			settextcolor(BLACK);
			settextstyle(48, 0, "宋体");
			outtextxy(384, 180, "开始查询");
			outtextxy(384, 250, "  帮助  ");
			outtextxy(384, 320, "  退出  ");
		}
	}
}
void information(int sound)
{
	PlaySound(NULL, NULL, SND_FILENAME);
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE background, title, textbox, titleWhenClicked;
	loadimage(&background, ".\\地铁票价系统相关图片\\background.jpg", 960, 540);
	loadimage(&title, ".\\地铁票价系统相关图片\\title.jpg", 192, 54);
	loadimage(&textbox, ".\\地铁票价系统相关图片\\textbox.jpg", 600, 375);
	loadimage(&titleWhenClicked, ".\\地铁票价系统相关图片\\titleWhenClicked.jpg", 192, 54);
	putimage(0, 0, &background);
	putimage(180, 60, &textbox);
	putimage(710, 460, &title);
	settextcolor(BLACK);
	settextstyle(48, 0, "宋体");
	outtextxy(432, 90, "关于");
	outtextxy(758, 465, "返回");
	settextstyle(38, 0, "宋体");
	outtextxy(200, 160, "地铁查询");
	outtextxy(200, 220, "制作人：罗世琨");
	outtextxy(200, 280, "2020年9月至12月");
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		settextstyle(48, 0, "宋体");
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 710 && m.x < 902 && m.y>460 && m.y < 514)
		{
			if (sound)
			{
				PlaySound(".\\地铁票价系统相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(710, 460, &titleWhenClicked);
			outtextxy(758, 465, "返回");
			Sleep(100);
			putimage(710, 460, &title);
			outtextxy(758, 465, "返回");
			Sleep(100);
			return;
		}
		else if (m.x > 710 && m.x < 902 && m.y>460 && m.y < 514)
		{
			settextcolor(RED);
			outtextxy(758, 465, "返回");
		}
		else
		{
			settextcolor(BLACK);
			outtextxy(758, 465, "返回");
		}	
	}
}
void help(int sound)
{
	IMAGE background, title, textbox, titleWhenClicked, title2, titleWhenClicked2;
	loadimage(&background, ".\\地铁票价系统相关图片\\background.jpg", 960, 540);
	loadimage(&title, ".\\地铁票价系统相关图片\\title.jpg", 192, 54);
	loadimage(&textbox, ".\\地铁票价系统相关图片\\textbox.jpg", 600, 375);
	loadimage(&titleWhenClicked, ".\\地铁票价系统相关图片\\titleWhenClicked.jpg", 192, 54);
	loadimage(&title2, ".\\地铁票价系统相关图片\\title.jpg", 100, 50);
	loadimage(&titleWhenClicked2, ".\\地铁票价系统相关图片\\titleWhenClicked.jpg", 100, 50);
	putimage(0, 0, &background);
	putimage(0, 0, &title2);
	putimage(180, 65, &textbox);
	putimage(710, 465, &title);
	settextcolor(BLACK);
	settextstyle(48, 0, "宋体");
	outtextxy(432, 95, "帮助");
	outtextxy(758, 470, "返回");
	outtextxy(2, 1, "官网");
	settextstyle(30, 0, "宋体");
	outtextxy(180, 155, "    这是一个用于查询北京地铁票价的软件。");
	outtextxy(180, 195, "软件有两种查询方式。第一种，用户输入起始");
	outtextxy(180, 235, "站后，软件会给出该起始站到其他站点的最少");
	outtextxy(180, 275, "票价，并以不同线路分类显示。第二种，用户");
	outtextxy(180, 315, "同时输入起始站和终点站，软件会生成从起始");
	outtextxy(180, 355, "站到终点站的具体线路和分段信息。");
	
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		settextstyle(48, 0, "宋体");
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 710 && m.x < 902 && m.y>465 && m.y < 519)
		{
			putimage(710, 465, &titleWhenClicked);
			outtextxy(758, 470, "返回");
			Sleep(100);
			putimage(710, 465, &title);
			outtextxy(758, 470, "返回");
			Sleep(100);
			return;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 0 && m.x < 100 && m.y>0 && m.y < 50)
		{
			putimage(0, 0, &titleWhenClicked2);
			outtextxy(2, 1, "官网");
			Sleep(100);
			putimage(0, 0, &title2);
			outtextxy(2, 1, "官网");
			Sleep(100);
			web_visit("www.bjsubway.com/index.html");
		}
		else if (m.x > 710 && m.x < 902 && m.y>465 && m.y < 519)
		{
			settextcolor(RED);
			outtextxy(758, 470, "返回");
		}
		else if (m.x > 0 && m.x < 100 && m.y>0 && m.y < 50)
		{
			settextcolor(RED);
			outtextxy(2, 1, "官网");
		}
		else
		{
			settextcolor(BLACK);
			outtextxy(2, 1, "官网");
			outtextxy(758, 470, "返回");
		}
	}
}
void exit()
{
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE background, title;
	loadimage(&background, _T(".\\地铁票价系统相关图片\\background.jpg"), 960, 540);
	loadimage(&title, _T(".\\地铁票价系统相关图片\\title.jpg"), 315, 78);
	putimage(0, 0, &background);
	putimage(322, 200, &title);
	settextcolor(BLACK);
	settextstyle(72, 0, "宋体");
	outtextxy(332, 205, "程序结束");
	Sleep(500);
}
bool data_prepare()
{
	int current_num = 0;//记录string数组
	int subwayLine_sum;//地铁线路总数
	int subwayStation_sum;//一条地铁线路中的地铁站总数
	string currentStation_name;//当前地铁站名称
	int subwayLine_num;//地铁线路编号
	string subwayLine_name;//地铁线路名称
	int subwayStation_num;//目的地地铁站编号
	string subwayStation_name;//目的地地铁站名称
	double length;
	int changeStation_sum;//换乘站总数
	string changeStation_name;//换乘站名称
	int changeSum;//换乘站的换乘线路总数
	int changeNum[10];//换乘站的换乘线路编号
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			eachSubway_name[i][j] = "NULL";
		}
	}
	ifstream is(".\\地铁票价系统相关文档\\BaseSubWayInfo.txt");
	is >> subwayLine_sum;
	for (int i = 1; i <= subwayLine_sum; i++)
	{
		is >> subwayLine_num;
		is >> subwayLine_name;
		is >> subwayStation_sum;
		is >> currentStation_name;
		eachSubway_name[i - 1][0] = currentStation_name;
		for (int j = 1; j <= subwayStation_sum - 1; j++)
		{
			is >> length >> subwayStation_name;
			eachSubway_name[i - 1][j] = subwayStation_name;
			bool judge1 = true;
			bool judge2 = true;
			int tempPlace1;
			int tempPlace2;
			int solvedPlace1;
			int solvedPlace2;
			for (int k = 0; k < current_num; k++)
			{
				if (subway_data[k] == currentStation_name)
				{
					tempPlace1 = k;
					judge1 = false;
				}
				if (subway_data[k] == subwayStation_name)
				{
					tempPlace2 = k;
					judge2 = false;
				}
			}
			if (judge1)
			{
				subway_data[current_num] = currentStation_name;
				solvedPlace1 = current_num;
				current_num++;
			}
			else
			{
				solvedPlace1 = tempPlace1;
			}
			if (judge2)
			{
				subway_data[current_num] = subwayStation_name;
				solvedPlace2 = current_num;
				current_num++;
			}
			else
			{
				solvedPlace2 = tempPlace2;
			}
			g.setEdge(solvedPlace1, solvedPlace2, subwayLine_num, subwayLine_name, subwayStation_name, length);
			g.setEdge(solvedPlace2, solvedPlace1, subwayLine_num, subwayLine_name, currentStation_name, length);
			currentStation_name = subwayStation_name;
		}
	}
	station_SUM = current_num;
	is >> changeStation_sum;
	for (int i = 1; i <= changeStation_sum; i++)
	{
		is >> changeStation_name;
		is >> changeSum;
		for (int j = 0; j < changeSum; j++)
		{
			is >> changeNum[j];
		}
	}
	string tempStationName;
	int tempStationLocX;
	int tempStationLocY;
	int subwayStationLocation_num;
	ifstream is2(".\\地铁票价系统相关文档\\subwayStation_location.txt");
	is2 >> subwayStationLocation_num;
	for (int i = 1; i <= subwayStationLocation_num; i++)
	{
		is2 >> tempStationName >> tempStationLocX >> tempStationLocY;
		for (int j = 0; j < station_SUM; j++)
		{
			if (subway_data[j] == tempStationName)
			{
				map_subwayStation_Place[j][0] = tempStationLocX;
				map_subwayStation_Place[j][1] = tempStationLocY;
				break;
			}
		}
	}
	return true;
}
int pattern_choose()
{
	MOUSEMSG m;
	IMAGE chooseBox_1, chooseBox_2, back, back_whenMouseOver, temp_chooseBox_1[10], background, title_1;
	loadimage(&chooseBox_1, ".\\地铁票价系统相关图片\\chooseBox_1.jpg", 500, 300);
	loadimage(&chooseBox_2, ".\\地铁票价系统相关图片\\chooseBox_2.jpg", 150, 75);
	loadimage(&back, ".\\地铁票价系统相关图片\\back.jpg", 50, 50);
	loadimage(&back_whenMouseOver, ".\\地铁票价系统相关图片\\back_whenMouseOver.jpg", 50, 50);
	loadimage(&background, ".\\地铁票价系统相关图片\\background.jpg", 960, 540);
	loadimage(&title_1, ".\\地铁票价系统相关图片\\title.jpg", 405, 84);
	for (int i = 1; i <= 10; i++)
	{
		BeginBatchDraw();
		loadimage(&temp_chooseBox_1[i-1], ".\\地铁票价系统相关图片\\chooseBox_1.jpg", 50 * i, 30 * i);
		putimage((960 - 50 * i) / 2, 120 + (300 - 30 * i) / 2, &temp_chooseBox_1[i-1]);
		EndBatchDraw();
		newSleep(10);
	}
	settextcolor(BLACK);
	settextstyle(48, 0, "楷体");
	putimage(230, 120, &chooseBox_1);
	outtextxy(384, 140, "模式选择");
	Sleep(300);
	settextstyle(36, 0, "楷体");
	putimage(290, 295, &chooseBox_2);
	putimage(520, 295, &chooseBox_2);
	outtextxy(292, 318, "站点查询");
	outtextxy(522, 318, "路线查询");
	putimage(260, 140, &back);
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 290 && m.x < 440 && m.y> 295 && m.y < 370)
		{
			return 1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 520 && m.x < 670 && m.y> 295 && m.y < 370)
		{
			return 2;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 260 && m.x < 310 && m.y> 140 && m.y < 190)
		{
			settextstyle(72, 0, "宋体");
			for (int i = 10; i >= 1; i--)
			{
				BeginBatchDraw();
				putimage(0, 0, &background);
				putimage(277, 60, &title_1);
				outtextxy(286, 65, "地 铁 查 询");
				putimage((960 - 50 * i) / 2, 120 + (300 - 30 * i) / 2, &temp_chooseBox_1[i - 1]);
				EndBatchDraw();
				newSleep(10);
			}
			putimage(0, 0, &background);
			return -1;
		}
		else if (m.x > 290 && m.x < 440 && m.y> 295 && m.y < 370)
		{
			settextcolor(RED);
			outtextxy(292, 318, "站点查询");
		}
		else if (m.x > 520 && m.x < 670 && m.y> 295 && m.y < 370)
		{
			settextcolor(RED);
			outtextxy(522, 318, "路线查询");
		}
		else if (m.x > 260 && m.x < 310 && m.y> 140 && m.y < 190)
		{
			putimage(260, 140, &back_whenMouseOver);
		}
		else
		{
			settextcolor(BLACK);
			putimage(260, 140, &back);
			outtextxy(292, 318, "站点查询");
			outtextxy(522, 318, "路线查询");
		}
	}
}
void transit_line_display(bool station_display)
{
	MOUSEMSG m;
	IMAGE rail_transit_line, chooseBox_1, back, back_whenMouseOver;
	bool isDrag = false;
	bool isFirst = true;
	int frame_length = 80;//上方边框长度
	int paint_x = -1600;
	int paint_y = -1600;
	int current_x;
	int current_y;
	int new_x;
	int new_y;
	double picture_ratio = 1;
	int PaintLocX;
	int PaintLocY;
	setfillcolor(RED);
	settextcolor(BLACK);
	settextstyle(72, 0, "宋体");
	loadimage(&rail_transit_line, ".\\地铁票价系统相关图片\\rail_transit_line.jpg", 4079, 3060);
	loadimage(&chooseBox_1, ".\\地铁票价系统相关图片\\chooseBox_1.jpg", 960, frame_length);
	loadimage(&back, ".\\地铁票价系统相关图片\\back.jpg", 50, 50);
	loadimage(&back_whenMouseOver, ".\\地铁票价系统相关图片\\back_whenMouseOver.jpg", 50, 50);
	cleardevice();
	BeginBatchDraw();
	putimage(-1600, -1600, &rail_transit_line);
	if (station_display)
	{
		for (int i = 0; i < station_SUM; i++)
		{
			if (function2_display_flag[i])
			{
				PaintLocX = map_subwayStation_Place[i][0] * 1.0 / 768 * 4079 - 1600;
				PaintLocY = map_subwayStation_Place[i][1] * 1.0 / 576 * 3060 - 1600;
				solidcircle(PaintLocX, PaintLocY, 15);
			}
		}
	}
	putimage(0, 0, &chooseBox_1);
	putimage(10, 10, &back);
	outtextxy(336, 4, "地铁线路");
	EndBatchDraw();
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			if (m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60) break;
			isDrag = true;
			if (1)
			{
				current_x = m.x;
				current_y = m.y;
				isFirst = false;
			}
		}
		else if (m.uMsg == WM_LBUTTONUP)
		{
			isDrag = false;
			isFirst = true;
		}
		else if (m.uMsg == WM_MOUSEMOVE && isDrag)
		{
			new_x = m.x;
			new_y = m.y;
			int tempX= (current_x - new_x) / 40;
			int tempY= (current_y - new_y) / 40;
			paint_x -= tempX;
			paint_y -= tempY;
			if (paint_x >= 960 - 4079 * picture_ratio && paint_x <= 0 && paint_y >= 540 - 3060 * picture_ratio && paint_y <= frame_length)
			{
				cleardevice();
				BeginBatchDraw();
				putimage(paint_x, paint_y, &rail_transit_line);
				if (station_display)
				{
					if (picture_ratio == 1)
					{
						for (int i = 0; i < station_SUM; i++)
						{
							if (function2_display_flag[i])
							{
								PaintLocX = map_subwayStation_Place[i][0] * 1.0 / 768 * 4079 + paint_x;
								PaintLocY = map_subwayStation_Place[i][1] * 1.0 / 576 * 3060 + paint_y;
								solidcircle(PaintLocX, PaintLocY, 15);
							}
							
						}
					}
					else if (picture_ratio == 0.6)
					{
						for (int i = 0; i < station_SUM; i++)
						{
							if (function2_display_flag[i])
							{
								PaintLocX = map_subwayStation_Place[i][0] * 1.0 / 768 * 4079 * 0.6 + paint_x;
								PaintLocY = map_subwayStation_Place[i][1] * 1.0 / 576 * 3060 * 0.6 + paint_y;
								solidcircle(PaintLocX, PaintLocY, 10);
							}
						}
					}
				}
				putimage(0, 0, &chooseBox_1);
				putimage(10, 10, &back);
				outtextxy(336, 4, "地铁线路");
				EndBatchDraw();
			}
			else
			{
				paint_x += tempX;
				paint_y += tempY;
			}
		}
		else if (m.uMsg == WM_RBUTTONDOWN)
		{
			if (picture_ratio == 1)
			{	
				picture_ratio = 0.6;
				loadimage(&rail_transit_line, ".\\地铁票价系统相关图片\\rail_transit_line.jpg", 4079 * picture_ratio, 3060 * picture_ratio);
				int new_paintX = m.x - (m.x - paint_x)*picture_ratio;
				int new_paintY = m.y - (m.y - paint_y)*picture_ratio;
				if (new_paintX < 960 - 4079 * picture_ratio)
				{
					new_paintX = 960 - 4079 * picture_ratio;
				}
				else if(new_paintX > 0)
				{
					new_paintX = 0;
				}
				if (new_paintY < 540 - 3060 * picture_ratio)
				{
					new_paintY = 540 - 3060 * picture_ratio;
				}
				else if (new_paintY > frame_length)
				{
					new_paintY = frame_length;
				}
				BeginBatchDraw();
				putimage(new_paintX, new_paintY, &rail_transit_line);
				paint_x = new_paintX;
				paint_y = new_paintY;
				if (station_display)
				{
					for (int i = 0; i < station_SUM; i++)
					{
						if (function2_display_flag[i])
						{
							PaintLocX = map_subwayStation_Place[i][0] * 1.0 / 768 * 4079 * 0.6 + paint_x;
							PaintLocY = map_subwayStation_Place[i][1] * 1.0 / 576 * 3060 * 0.6 + paint_y;
							solidcircle(PaintLocX, PaintLocY, 10);
						}
					}
				}
				putimage(0, 0, &chooseBox_1);
				putimage(10, 10, &back);
				outtextxy(336, 4, "地铁线路");
				EndBatchDraw();
			}
			else
			{
				picture_ratio = 1;
				loadimage(&rail_transit_line, ".\\地铁票价系统相关图片\\rail_transit_line.jpg", 4079 * picture_ratio, 3060 * picture_ratio);
				int new_paintX = m.x - (m.x - paint_x)*(1/0.6);
				int new_paintY = m.y - (m.y - paint_y)*(1/0.6);
				BeginBatchDraw();
				putimage(new_paintX, new_paintY, &rail_transit_line);
				paint_x = new_paintX;
				paint_y = new_paintY;
				if (station_display)
				{
					for (int i = 0; i < station_SUM; i++)
					{
						if (function2_display_flag[i])
						{
							PaintLocX = map_subwayStation_Place[i][0] * 1.0 / 768 * 4079 + paint_x;
							PaintLocY = map_subwayStation_Place[i][1] * 1.0 / 576 * 3060 + paint_y;
							solidcircle(PaintLocX, PaintLocY, 15);
						}
					}
				}
				putimage(0, 0, &chooseBox_1);
				putimage(10, 10, &back);
				outtextxy(336, 4, "地铁线路");
				EndBatchDraw();
			}
		}
		else if (m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60)
		{
			putimage(10, 10, &back_whenMouseOver);
		}
		else
		{
			putimage(10, 10, &back);
		}
	}
}
void shell_window(string content)
{
	IMAGE shell_window, temp;
	loadimage(&shell_window, ".\\地铁票价系统相关图片\\shell_window.jpg", 500, 150);
	getimage(&temp, 230, 195, 500, 150);
	putimage(230, 195, &shell_window);
	const char* newStr = content.data();
	outtextxy(230 + (500 - 45 * content.length() / 2) / 2, 245, newStr);
	Sleep(1500);
	putimage(230, 195, &temp);
}
int function_search(string txtStr)//站点查询
{
	IMAGE background_2, right_arrowpoint, right_arrowpoint_whenMouseOver, back_2, back_whenMouseOver_2, new_choose_1, new_choose_2;
	loadimage(&background_2, ".\\地铁票价系统相关图片\\background_2.jpg", 960, 540);
	loadimage(&right_arrowpoint, ".\\地铁票价系统相关图片\\right_arrowpoint.jpg", 60, 45);
	loadimage(&right_arrowpoint_whenMouseOver, ".\\地铁票价系统相关图片\\right_arrowpoint_whenMouseOver.jpg", 60, 45);
	loadimage(&back_2, ".\\地铁票价系统相关图片\\back_2.jpg", 50, 50);
	loadimage(&back_whenMouseOver_2, ".\\地铁票价系统相关图片\\back_whenMouseOver_2.jpg", 50, 50);
	loadimage(&new_choose_1, ".\\地铁票价系统相关图片\\new_choose_1.jpg", 40, 40);
	loadimage(&new_choose_2, ".\\地铁票价系统相关图片\\new_choose_2.jpg", 40, 40);
	char in[20];
	string in_name;
	InputBox(in, 10, txtStr.data());
	int in_length = strlen(in);
	int num = 0;
	int count = 0;
	int total = 0;
	int total_page;
	int page = 0;
	int current_page_subway[7];//记录当前页面7个地铁站的编号
	bool current_judge[7];//记录当前页面7个地铁站的存在情况
	for (int i = 0; i < in_length; i++)
	{
		in_name += in[i];
	}
	putimage(0, 0, &background_2);
	setlinecolor(RED);
	settextcolor(BLACK);
	settextstyle(60, 0, "行楷");
	outtextxy(384, 0, "检索结果");
	line(0, 60, 960, 60);
	putimage(0, 0, &back_2);
	for (int i = 0; i < station_SUM; i++)
	{
		int temp = subway_data[i].length() - in_length;
		for (int j = 0; j <= temp; j += 2)
		{
			if (in_name == subway_data[i].substr(j,in_length))
			{
				total++;
				break;
			}	
		}
	}
	total_page = (total % 7 == 0 ? total / 7 : total / 7 + 1);
	settextstyle(25, 0, "行楷");
	settextcolor(RED);
	TCHAR NUM[5];
	sprintf(NUM, _T("%d"), total);
	outtextxy(820, 65, "结果数：");
	outtextxy(920, 65, NUM);
	settextcolor(BLACK);
	for (int i=0;i<station_SUM;i++)
	{
		if (count == 0) BeginBatchDraw();
		bool judge = false;
		int temp = subway_data[i].length() - in_length;
		for (int j = 0; j <= temp; j += 2)
		{
			if (in_name == subway_data[i].substr(j, in_length))
			{
				judge = true;
				break;
			}
		}
		if (judge)
		{
			settextcolor(BLACK);
			current_page_subway[num] = i;
			current_judge[num] = true;
			settextstyle(48, 0, "行楷");
			const char* newStr = subway_data[i].data();
			putimage(25, 64 + 64 * num, &new_choose_1);
			outtextxy(70, 60 + 64 * num, newStr);
			num++;
			count++;
		}
		if (count == 7 || i == station_SUM - 1)
		{
			EndBatchDraw();
			num = 0;
			count = 0;
			page++;
			MOUSEMSG m;
			while (1)
			{
				m = GetMouseMsg();
				if (m.uMsg == WM_LBUTTONDOWN && m.x > 890 && m.x < 950 && m.y> 0 && m.y < 45 && page != total_page)
				{
					putimage(0, 0, &background_2);
					settextcolor(RED);
					setlinecolor(RED);
					settextstyle(48, 0, "行楷");
					settextcolor(BLACK);
					outtextxy(384, 0, "检索结果");
					line(0, 60, 960, 60);
					putimage(0, 0, &back_2);
					settextstyle(25, 0, "行楷");
					TCHAR NUM[5];
					sprintf(NUM, _T("%d"), total);
					settextcolor(RED);
					outtextxy(820, 65, "结果数：");
					outtextxy(920, 65, NUM);
					if (page < total_page - 1)
					{
						putimage(890, 0, &right_arrowpoint);
					}
					break;
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 0 && m.x < 50 && m.y> 0 && m.y < 50)
				{
					return -1;//退出查询
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 65 && m.y> 54 && m.y < 94 && current_judge[0] == true)
				{
					return current_page_subway[0];
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 65 && m.y> 64+64*1 && m.y < 104+64*1 && current_judge[1] == true)
				{
					return current_page_subway[1];
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 65 && m.y> 64 + 64 * 2 && m.y < 104 + 64 * 2 && current_judge[2] == true)
				{
					return current_page_subway[2];
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 65 && m.y> 64 + 64 * 3 && m.y < 104 + 64 * 3 && current_judge[3] == true)
				{
					return current_page_subway[3];
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 65 && m.y> 64 + 64 * 4 && m.y < 104 + 64 * 4 && current_judge[4] == true)
				{
					return current_page_subway[4];
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 65 && m.y> 64 + 64 * 5 && m.y < 104 + 64 * 5 && current_judge[5] == true)
				{
					return current_page_subway[5];
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 65 && m.y> 64 + 64 * 6 && m.y < 104 + 64 * 6 && current_judge[6] == true)
				{
					return current_page_subway[6];
				}
				else if (m.x > 890 && m.x < 950 && m.y> 0 && m.y < 45 && page != total_page)
				{
					if (page < total_page)
					{
						putimage(890, 0, &right_arrowpoint_whenMouseOver);
					}
				}
				else if (m.x > 0 && m.x < 50 && m.y> 0 && m.y < 50)
				{
					putimage(0, 0, &back_whenMouseOver_2);
				}
				else if (m.x > 25 && m.x < 65 && m.y> 64 && m.y < 104 && current_judge[0] == true)
				{
					putimage(25, 64 + 64 * 0, &new_choose_2);
				}
				else if (m.x > 25 && m.x < 65 && m.y> 64 + 64 * 1 && m.y < 104 + 64 * 1 && current_judge[1] == true)
				{
					putimage(25, 64 + 64 * 1, &new_choose_2);
				}
				else if (m.x > 25 && m.x < 65 && m.y> 64 + 64 * 2 && m.y < 104 + 64 * 2 && current_judge[2] == true)
				{
					putimage(25, 64 + 64 * 2, &new_choose_2);
				}
				else if (m.x > 25 && m.x < 65 && m.y> 64 + 64 * 3 && m.y < 104 + 64 * 3 && current_judge[3] == true)
				{
					putimage(25, 64 + 64 * 3, &new_choose_2);
				}
				else if (m.x > 25 && m.x < 65 && m.y> 64 + 64 * 4 && m.y < 104 + 64 * 4 && current_judge[4] == true)
				{
					putimage(25, 64 + 64 * 4, &new_choose_2);
				}
				else if (m.x > 25 && m.x < 65 && m.y> 64 + 64 * 5 && m.y < 104 + 64 * 5 && current_judge[5] == true)
				{
					putimage(25, 64 + 64 * 5, &new_choose_2);
				}
				else if (m.x > 25 && m.x < 65 && m.y> 64 + 64 * 6 && m.y < 104 + 64 * 6 && current_judge[6] == true)
				{
					putimage(25, 64 + 64 * 6, &new_choose_2);
				}
				else
				{
					if (page < total_page)
					{
						putimage(890, 0, &right_arrowpoint);
					}
					putimage(0, 0, &back_2);
					for (int k = 0; k < 7; k++)
					{
						if (current_judge[k] == true)
						putimage(25, 64 + 64 * k, &new_choose_1);
					}
				}
			}
			memset(current_judge, false, sizeof(current_judge));
		}
	}
}
int function1_display_part(int num, int start)
{
	IMAGE background_2, back_2, back_whenMouseOver_2, chooseBox_1, chooseBox_2;
	loadimage(&background_2, ".\\地铁票价系统相关图片\\background_2.jpg", 960, 540);
	loadimage(&back_2, ".\\地铁票价系统相关图片\\back_2.jpg", 50, 50);
	loadimage(&back_whenMouseOver_2, ".\\地铁票价系统相关图片\\back_whenMouseOver_2.jpg", 50, 50);
	loadimage(&chooseBox_1, ".\\地铁票价系统相关图片\\chooseBox_1.jpg", 80, 50);
	loadimage(&chooseBox_2, ".\\地铁票价系统相关图片\\chooseBox_2.jpg", 80, 50);
	BeginBatchDraw();//////////////////////////////////
	putimage(0, 0, &background_2);
	putimage(10, 10, &back_2);
	int count = 0;
	for (int i = 108; i <= 860; i += 94)
	{
		count++;
		if (count == num) putimage(i, 10, &chooseBox_2);
		else putimage(i, 10, &chooseBox_1);
	}
	for (int i = 14; i <= 860; i += 94)
	{
		count++;
		if (count == num) putimage(i, 70, &chooseBox_2);
		else putimage(i, 70, &chooseBox_1);
	}
	setlinecolor(RED);
	setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 5);
	settextstyle(24, 0, "行楷");
	outtextxy(114, 23, "1号线");
	outtextxy(114 + 94 * 1, 23, "2号线");
	outtextxy(114 + 94 * 2, 23, "4号线");
	outtextxy(114 + 94 * 3, 23, "5号线");
	outtextxy(114 + 94 * 4, 23, "6号线");
	outtextxy(114 + 94 * 5, 23, "7号线");
	outtextxy(114 + 94 * 6, 23, "8号线");
	outtextxy(114 + 94 * 7, 23, "9号线");
	outtextxy(112 + 94 * 8, 23, "10号线");
	outtextxy(18, 83, "13号线");
	outtextxy(18 + 94 * 1, 83, "14(西)");
	outtextxy(18 + 94 * 2, 83, "14(东)");
	outtextxy(18 + 94 * 3, 83, "15号线");
	outtextxy(18 + 94 * 4, 83, "八通线");
	outtextxy(18 + 94 * 5, 83, "昌平线");
	outtextxy(18 + 94 * 6, 83, "亦庄线");
	outtextxy(18 + 94 * 7, 83, "大兴线");
	outtextxy(18 + 94 * 8, 83, "房山线");
	outtextxy(18 + 94 * 9, 83, "机场线");
	line(0, 130, 960, 130);
	Dist* D = new Dist[station_SUM];
	Dijkstra(g, start, D, 1, num - 1, 0, 0);
	EndBatchDraw();
	MOUSEMSG m;
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 && m.x < 108 + 80 && m.y> 10 && m.y < 60)
		{
			return 1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 && m.x < 108 + 94 + 80 && m.y> 10 && m.y < 60)
		{
			return 2;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 * 2 && m.x < 108 + 94 * 2 + 80 && m.y> 10 && m.y < 60)
		{
			return 3;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 * 3 && m.x < 108 + 94 * 3 + 80 && m.y> 10 && m.y < 60)
		{
			return 4;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 * 4 && m.x < 108 + 94 * 4 + 80 && m.y> 10 && m.y < 60)
		{
			return 5;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 * 5 && m.x < 108 + 94 * 5 + 80 && m.y> 10 && m.y < 60)
		{
			return 6;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 * 6 && m.x < 108 + 94 * 6 + 80 && m.y> 10 && m.y < 60)
		{
			return 7;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 * 7 && m.x < 108 + 94 * 7 + 80 && m.y> 10 && m.y < 60)
		{
			return 8;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 108 + 94 * 8 && m.x < 108 + 94 * 8 + 80 && m.y> 10 && m.y < 60)
		{
			return 9;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 && m.x < 14 + 80 && m.y> 50 && m.y < 120)
		{
			return 10;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 1 && m.x < 14 + 94 * 1 + 80 && m.y> 50 && m.y < 120)
		{
			return 11;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 2 && m.x < 14 + 94 * 2 + 80 && m.y> 50 && m.y < 120)
		{
			return 12;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 3 && m.x < 14 + 94 * 3 + 80 && m.y> 50 && m.y < 120)
		{
			return 13;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 4 && m.x < 14 + 94 * 4 + 80 && m.y> 50 && m.y < 120)
		{
			return 14;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 5 && m.x < 14 + 94 * 5 + 80 && m.y> 50 && m.y < 120)
		{
			return 15;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 6 && m.x < 14 + 94 * 6 + 80 && m.y> 50 && m.y < 120)
		{
			return 16;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 7 && m.x < 14 + 94 * 7 + 80 && m.y> 50 && m.y < 120)
		{
			return 17;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 8 && m.x < 14 + 94 * 8 + 80 && m.y> 50 && m.y < 120)
		{
			return 18;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 14 + 94 * 9 && m.x < 14 + 94 * 9 + 80 && m.y> 50 && m.y < 120)
		{
			return 19;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60)
		{
			return -1;
		}
		else if (m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60)
		{
			putimage(10, 10, &back_whenMouseOver_2);
		}
		else
		{
			putimage(10, 10, &back_2);
		}
	}
}
void function1_display(int start)
{
	int num;
	int res = function1_display_part(1, start);
	while (res != -1)
	{
		res = function1_display_part(res, start);
	}
}
int function2_display_part(int station_1, int station_2)
{
	int paint_length;
	int dijkstra_paint_place = 0;
	IMAGE background_2, back_2, back_whenMouseOver_2, displayType_change, displayType_change2, temp, up_page, down_page, paint_board, up_page2, down_page2, map2;
	loadimage(&background_2, ".\\地铁票价系统相关图片\\background_2.jpg", 960, 540);
	loadimage(&back_2, ".\\地铁票价系统相关图片\\back_2.jpg", 50, 50);
	loadimage(&back_whenMouseOver_2, ".\\地铁票价系统相关图片\\back_whenMouseOver_2.jpg", 50, 50);
	loadimage(&displayType_change, ".\\地铁票价系统相关图片\\displayType_change.jpg", 60, 60);
	loadimage(&displayType_change2, ".\\地铁票价系统相关图片\\displayType_change2.jpg", 60, 60);
	loadimage(&up_page, ".\\地铁票价系统相关图片\\up_page.jpg", 60, 60);
	loadimage(&down_page, ".\\地铁票价系统相关图片\\down_page.jpg", 60, 60);
	loadimage(&up_page2, ".\\地铁票价系统相关图片\\up_page2.jpg", 60, 60);
	loadimage(&down_page2, ".\\地铁票价系统相关图片\\down_page2.jpg", 60, 60);
	loadimage(&paint_board, ".\\地铁票价系统相关图片\\paint_board.jpg", 680, 474);
	loadimage(&map2, ".\\地铁票价系统相关图片\\map2.jpg", 50, 50);
	int display_type = 1;
	function2_display_part_again:
	cleardevice();
	BeginBatchDraw();
	putimage(0, 0, &background_2);
	putimage(10, 10, &back_2);
	putimage(885, 10, &map2);
	if (display_type == 2) putimage(280, 68, &paint_board);
	settextstyle(60, 0, "行楷");
	settextcolor(BLACK);
	if (display_type == 1) outtextxy(330, 0, "具体路线");
	else outtextxy(330, 0, "分段信息");
	putimage(570, 0, &displayType_change);
	setlinecolor(RED);
	setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 5);
	line(0, 65, 960, 65);
	getimage(&temp, 0, 0, 960, 70);
	Dist* D = new Dist[g.vertexNum];
	if (display_type == 1) Dijkstra(g, station_1, D, 2, 0, station_2, 0);
	else if (display_type == 2) paint_length = Dijkstra(g, station_1, D, 3, 0, station_2, dijkstra_paint_place);
	putimage(0, 0, &temp);
	if (display_type == 2)
	{
		putimage(850, 220, &up_page);
		putimage(850, 370, &down_page);
	}
	EndBatchDraw();
	MOUSEMSG m;
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60)
		{
			return -1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 570 && m.x < 630 && m.y> 0 && m.y < 60)
		{
			display_type = (display_type == 1 ? 2 : 1);
			goto function2_display_part_again;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 850 && m.x < 910 && m.y>220 && m.y < 280 && display_type == 2)
		{
			if (dijkstra_paint_place < 0) 
			dijkstra_paint_place += 50;
			goto function2_display_part_again;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 850 && m.x < 910 && m.y>370 && m.y < 430 && display_type == 2)
		{
			if (dijkstra_paint_place + paint_length  > 400)
			dijkstra_paint_place -= 50;
			goto function2_display_part_again;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 885 && m.x < 935 && m.y>10 && m.y < 60)
		{
			transit_line_display(true);
			goto function2_display_part_again;
		}
		else if (m.x > 850 && m.x < 910 && m.y>220 && m.y < 280 && display_type == 2)
		{
			putimage(850, 220, &up_page2);
		}
		else if (m.x > 850 && m.x < 910 && m.y>370 && m.y < 430 && display_type == 2)
		{
			putimage(850, 370, &down_page2);
		}
		else if (m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60)
		{
			putimage(10, 10, &back_whenMouseOver_2);
		}
		else if (m.x > 570 && m.x < 630 && m.y> 0 && m.y < 60)
		{
			putimage(570, 0, &displayType_change2);
		}
		else
		{
			putimage(10, 10, &back_2);
			putimage(570, 0, &displayType_change);
			if (display_type == 2)
			{
				putimage(850, 220, &up_page);
				putimage(850, 370, &down_page);
			}
		}
	}
}
int function2_display()
{
	again:
	IMAGE background_2, back_2, back_whenMouseOver_2, chooseBox_1, textbox, textbox_2, textbox_3, textbox_4;
	int station_1 = -1;
	int station_2 = -1;
	loadimage(&background_2, ".\\地铁票价系统相关图片\\background_2.jpg", 960, 540);
	loadimage(&back_2, ".\\地铁票价系统相关图片\\back_2.jpg", 50, 50);
	loadimage(&back_whenMouseOver_2, ".\\地铁票价系统相关图片\\back_whenMouseOver_2.jpg", 50, 50);
	loadimage(&chooseBox_1, ".\\地铁票价系统相关图片\\chooseBox_1.jpg", 300, 53);
	loadimage(&textbox, ".\\地铁票价系统相关图片\\textbox.jpg", 95, 53);
	loadimage(&textbox_2, ".\\地铁票价系统相关图片\\textbox_2.jpg", 95, 53);
	loadimage(&textbox_3, ".\\地铁票价系统相关图片\\textbox.jpg", 180, 60);
	loadimage(&textbox_4, ".\\地铁票价系统相关图片\\textbox_2.jpg", 180, 60);
	putimage(0, 0, &background_2);
	putimage(10, 10, &back_2);
	setlinecolor(RED);
	setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 5);
	settextstyle(60, 0, "行楷");
	settextcolor(RED);
	outtextxy(300, 0, "路线查询系统");
	line(0, 70, 960, 70);
	settextstyle(45, 0, "行楷");
	settextcolor(BLACK);
	outtextxy(180, 150, "起点：");
	outtextxy(180, 270, "终点：");
	putimage(320, 146, &chooseBox_1);
	putimage(320, 266, &chooseBox_1);
	putimage(630, 146, &textbox);
	putimage(630, 266, &textbox);
	putimage(390, 380, &textbox_3);
	outtextxy(630, 150, "选择");
	outtextxy(630, 270, "选择");
	outtextxy(390, 386, "开始查询");
	setlinestyle(PS_DASH | PS_ENDCAP_FLAT, 3);
	setlinecolor(RED);
	line(160, 120, 800, 120);
	line(160, 480, 800, 480);
	line(160, 120, 160, 480);
	line(800, 120, 800, 480);
	IMAGE temp;
	getimage(&temp, 0, 0, 960, 540);
	MOUSEMSG m;
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 630 && m.x < 725 && m.y> 146 && m.y < 219)
		{
			station_1 = function_search("请输入起始站：");
			putimage(0, 0, &temp);
			settextstyle(45, 0, "行楷");
			if (station_1 != -1)
			{
				const char* newStr1 = subway_data[station_1].data();
				outtextxy(320, 146, newStr1);
			}
			if (station_2 != -1)
			{
				const char* newStr2 = subway_data[station_2].data();
				outtextxy(320, 266, newStr2);
			}
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 630 && m.x < 725 && m.y> 266 && m.y < 319)
		{
			station_2 = function_search("请输入终点站：");
			putimage(0, 0, &temp);
			settextstyle(45, 0, "行楷");
			if (station_1 != -1)
			{
				const char* newStr1 = subway_data[station_1].data();
				outtextxy(320, 146, newStr1);
			}
			if (station_2 != -1)
			{
				const char* newStr2 = subway_data[station_2].data();
				outtextxy(320, 266, newStr2);
			}
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 390 && m.x < 570 && m.y> 380 && m.y < 440)
		{
			if (station_1 == -1 || station_2 == -1)
			{
				shell_window("起点与终点未填写完整");
			}
			else
			{
				if (station_1 == station_2)
				{
					shell_window("起点与终点不能相同");
				}
				else
				{
					if (function2_display_part(station_1, station_2) == -1) goto again;
				}
			}
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60)
		{
			return -1;
		}
		else if (m.x > 10 && m.x < 60 && m.y> 10 && m.y < 60)
		{
			putimage(10, 10, &back_whenMouseOver_2);
		}
		else if (m.x > 630 && m.x < 725 && m.y> 146 && m.y < 219)
		{
			BeginBatchDraw();
			putimage(630, 146, &textbox_2);
			settextstyle(45, 0, "行楷");
			outtextxy(630, 150, "选择");
			EndBatchDraw();
		}
		else if (m.x > 630 && m.x < 725 && m.y> 266 && m.y < 319)
		{
			BeginBatchDraw();
			putimage(630, 266, &textbox_2);
			settextstyle(45, 0, "行楷");
			outtextxy(630, 270, "选择");
			EndBatchDraw();
		}
		else if (m.x > 390 && m.x < 570 && m.y> 380 && m.y < 440)
		{
			BeginBatchDraw();
			putimage(390, 380, &textbox_4);
			settextstyle(45, 0, "行楷");
			outtextxy(390, 386, "开始查询");
			EndBatchDraw();
		}
		else
		{
			BeginBatchDraw();
			putimage(630, 146, &textbox);
			putimage(630, 266, &textbox);
			putimage(10, 10, &back_2);
			putimage(390, 380, &textbox_3);
			settextstyle(45, 0, "行楷");
			outtextxy(630, 150, "选择");
			outtextxy(630, 270, "选择");
			outtextxy(390, 386, "开始查询");
			EndBatchDraw();
		}
	}
	return 1;
}
int main()
{
	initgraph(960, 540);
	setbkmode(TRANSPARENT);
	int sound = 1;//数值为1代表声音开启，数值为0代表声音关闭。
	int choice_1;
	int choice_2;
	int res;
	while (1)
	{
		data_prepare();
		choice_1 = menu(&sound);
		switch (choice_1)
		{
			case 1: 
				choice_2 = pattern_choose();
				switch (choice_2)
				{
					case 1:
						res = function_search("请输入站点名称");
						if (res != -1) function1_display(res);
						break;
					case 2:
						function2_display();
						break;
					case -1:
						break;
				}
				break;
			case 2: help(sound); break;
			case 3: exit(); return 0;
			case 4: information(sound); break;
			case 5: transit_line_display(false); break;
		}
	}
	return 0;
}