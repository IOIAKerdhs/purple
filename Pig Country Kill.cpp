#include <iostream>
#include <string>
#include <queue>
using namespace std;

struct pig {
	string type, waitype; //真实身份与外在身份（主猪，忠猪，反猪，未知，类反）
	int heart = 4; //血量
	queue<char> poker; //标记手牌顺序
	queue<char> newp;
	bool zb; // 猪哥连弩
	int total[9] = { 0 }; //手牌数量
};

pig a[11]; //10只猪
queue<char> heap; //牌堆
int n, m;
bool isend = false; //结束标记

int panpai(char p) //将char转换为int来方便存数组
{
	switch (p)
	{
	case 'P':
		return 1;
	case 'K':
		return 2;
	case 'D':
		return 3;
	case 'F':
		return 4;
	case 'N':
		return 5;
	case 'W':
		return 6;
	case 'J':
		return 7;
	case 'Z':
		return 8;
	}
}

void mopai(int i, int j) //第i只猪摸j张牌
{
	int k = j;
	while (j--)
	{
		a[i].poker.push(heap.front());
		a[i].total[panpai(heap.front())]++;
		if (k == 3)
		{
			a[i].newp.push(heap.front());
		}
		if (heap.size() > 1)
		{
			heap.pop();
		}
	}
}

void yongpai(int i, int pok)
{
	a[i].total[pok]--;
	int cnt = a[i].poker.size();
	bool u = false;
	while (cnt--)
	{

		char yp = a[i].poker.front();
		a[i].poker.pop();

		if (u == true)
		{
			a[i].poker.push(yp);
		}
		else if (panpai(yp) == pok)
		{
			u = true;
		}
		else
		{
			a[i].poker.push(yp);
		}
	}
}

bool enemy(int i, int j) //i要不要攻击j
{
	switch (a[i].type[0])
	{
	case 'M':
		if (a[j].waitype == "LF")
		{
			return true;
		}
	case 'Z':
		if (a[j].waitype == "FP")
		{
			return true;
		}
		return false;
	case 'F':
		if (a[j].waitype == "MP" || a[j].waitype == "ZP")
		{
			return true;
		}
		return false;
	}
}

bool frend(int i, int j)
{
	switch (a[i].type[0])
	{
	case 'M':
	case 'Z':
		if (a[j].waitype == "MP")
		{
			return true;
		}
		if (a[j].waitype == "ZP")
		{
			return true;
		}
		return false;
	case 'F':
		if (a[j].waitype == "FP")
		{
			return true;
		}
		return false;
	}
}

bool end() //检测结束
{
	if (a[1].heart == 0) //主猪死亡
	{
		isend = true;
		return true;
	}
	else //所有反猪死亡
	{
		int total = 0;
		for (int i = 1; i <= n; i++)
		{
			if (a[i].type == "FP" && a[i].heart > 0)
			{
				total++;
			}
		}
		if (total == 0)
		{
			isend = true;
			return true;
		}
	}
	return false;
}

int canKill(int i) //i可以攻击到的目标
{
	int j = i;
	while (true)
	{
		j++;
		if (j > n)
		{
			j = 1;
		}
		if (i == j)
		{
			throw "end判断失误";
		}
		if (a[j].heart > 0)
		{
			return j;
		}
	}
}

void hasKill(int i, int j) //i杀死j猪后的一些事
{
	if (a[j].type == "FP")
	{
		mopai(i, 3);
	}
	if (a[i].type == "MP" && a[j].type == "ZP")
	{
		a[i].poker = queue<char>();
		a[i].newp = queue<char>();
		for (int x = 1; x < 9; x++)
		{
			a[i].total[x] = 0;
		}
		a[i].zb = false;
	}
}

void hhwhx(int i) //i回合外回血
{
	while (a[i].heart <= 0)
	{
		if (a[i].total[1] == 0)
		{
			return;
		}
		yongpai(i, 1);
		a[i].heart++;
	}
}

void hurt(int i, int j)
{
	a[j].heart--;
	if (a[j].heart > 0)
	{
		return;
	}

	hhwhx(j);

	if (a[j].heart <= 0)
	{
		if (end())
		{
			return;
		}
		else
		{
			hasKill(i, j);
		}
	}
}

void kill(int i, int j) //i杀j（减血/出牌）
{
	a[i].waitype = a[i].type;
	if (a[j].total[3] > 0)
	{
		yongpai(j, 3);
		return;
	}
	hurt(i, j);
}

bool wxkj(int p1, int p2, int p3, int pok)
{
	bool flag = false;
	if ((pok == 5 || pok == 6) && a[p1].total[7] > 0 && frend(p1, p3))
	{
		yongpai(p1, 7);
		if (p1 != p3)
		{
			a[p1].waitype = a[p1].type;
		}
		flag = true;
	}
	else if (pok == 7 && a[p1].total[7] > 0 && enemy(p1, p2))
	{
		yongpai(p1, 7);
		a[p1].waitype = a[p1].type;
		flag = true;
	}
	else if (pok == 4 && a[p1].total[7] > 0 && enemy(p1, p2) && frend(p1, p3))
	{
		yongpai(p1, 7);
		a[p1].waitype = a[p1].type;
		flag = true;
	}
	if (!flag)
	{
		return false;
	}

	int j = p1;
	int cnt = n;
	while (cnt--)
	{
		if (a[j].heart > 0)
		{
			if (wxkj(j, p1, p2, 7))
			{
				return false;
			}
		}
		j++;
		if (j > n)
		{
			j = 1;
		}
	}

	return true;
}

bool juedou(int i, int j) //决斗
{
	a[i].waitype = a[i].type;

	int s = i;
	int cnt = n;
	while (cnt--)
	{
		if (a[s].heart > 0)
		{
			if (wxkj(s, i, j, 4))
			{
				return false;
			}
		}

		s++;
		if (s > n)
		{
			s = 1;
		}
	}

	if (a[i].type == "MP" && a[j].type == "ZP")
	{
		hurt(i, j);
		return false;
	}

	s = i;
	while (true)
	{
		s = i + j - s;
		if (a[s].total[2] == 0)
		{
			if (s == i)
			{
				a[i].heart--;
				return true;
			}
			hurt(i + j - s, s);
			return false;
		}

		yongpai(s, 2);
	}
}

void NandW(int i, int pok) //南猪入侵 && 万箭齐发
{
	int j = i;
	while (true)
	{
		j++;
		if (j > n)
		{
			j = 1;
		}
		if (i == j)
		{
			return;
		}
		if (a[j].heart <= 0)
		{
			continue;
		}

		bool wx = false;
		int s = i;
		int cnt = n;
		while (cnt--)
		{
			if (a[s].heart > 0 && !wx)
			{
				if (wxkj(s, i, j, pok + 3))
				{
					wx = true;
				}
			}
			s++;
			if (s > n)
			{
				s = 1;
			}
		}

		if (wx == true)
		{
			continue;
		}

		if (a[i].waitype == "DN" && a[j].type == "MP")
		{
			a[i].waitype = "LF";
		}
		if (a[j].total[pok] > 0)
		{
			yongpai(j, pok);
			continue;
		}
		hurt(i, j);
		if (isend)
		{
			return;
		}
	}
}

int jdmb(int i) //决斗目标
{
	if (a[i].type == "FP")
	{
		return 1;
	}

	int j = i;
	while (true)
	{
		j++;
		if (j > n)
		{
			j = 1;
		}
		if (i == j)
		{
			return -1;
		}
		if (a[j].heart <= 0)
		{
			continue;
		}

		if (enemy(i, j))
		{
			return j;
		}
	}
}

void print(int i)
{
	if (a[i].heart <= 0)
	{
		cout << "DEAD" << endl;
		return;
	}

	int cnt = a[i].poker.size();
	while (cnt--)
	{
		cout << a[i].poker.front() << ' ';
		a[i].poker.push(a[i].poker.front());
		a[i].poker.pop();
	}
	cout << endl;
}

bool usepok(int i, char pok, bool& sha, queue<char>& use)
{
	if (a[i].heart < 4 && pok == 'P') //吃桃
	{
		yongpai(i, 1);
		a[i].heart++;
		return true;
	}
	else if (pok == 'Z') //穿装备
	{
		a[i].zb = true;

		yongpai(i, 8);
		return true;
	}
	else if (pok == 'N')
	{
		yongpai(i, 5);
		NandW(i, 2);
		return true;
	}
	else if (pok == 'W')
	{
		yongpai(i, 6);
		NandW(i, 3);
		return true;
	}
	else if (pok == 'F' && a[i].total[4] > 0)
	{
		int jds = jdmb(i);
		if (jds != -1)
		{
			yongpai(i, 4);
			if (juedou(i, jds))
			{
				while (a[i].heart <= 0)
				{
					if (a[i].total[1] == 0)
					{
						break;
					}
					yongpai(i, 1);

					int npok = 1;
					int ncnt = use.size();
					bool u = false;
					while (ncnt--)
					{
						char yp = use.front();
						use.pop();
						if (u == true)
						{
							use.push(yp);
						}
						else if (panpai(yp) == npok)
						{
							u = true;
						}
						else
						{
							use.push(yp);
						}
					}

					a[i].heart++;
				}
				if (a[i].heart <= 0)
				{
					hasKill(jds, i);
				}
			}
			return true;
		}
	}
	else if (pok == 'K' && a[i].total[2] > 0)
	{
		int s = canKill(i);
		if (enemy(i, s) && (!sha || a[i].zb))
		{
			yongpai(i, 2);
			sha = true;

			kill(i, s);
			return true;
		}
	}
	return false;
}

void chupai(int i) //第i只猪的出牌阶段
{
	bool sha = false;
	int up;
	do
	{
		up = 0;

		int cnt = a[i].poker.size();
		queue<char> use = a[i].poker;
		while (cnt--)
		{
			if (a[i].poker.empty())
			{
				break;
			}

			char pok = use.front();
			use.pop();
			bool nowuse = false;
			if (isend || a[i].heart <= 0)
			{
				use.push(pok);
				continue;
			}
			if (usepok(i, pok, sha, use))
			{
				up++;
			}
			if (a[i].poker.empty())
			{
				return;
			}
		}

		cnt = a[i].newp.size();
		while (cnt--)
		{
			if (a[i].newp.empty())
			{
				return;
			}

			char pok = a[i].newp.front();
			a[i].newp.pop();
			bool nowuse = false;
			if (isend || a[i].heart <= 0)
			{
				a[i].newp.push(pok);
				continue;
			}
			if (usepok(i, pok, sha, a[i].newp))
			{
				up++;
			}
			if (a[i].poker.empty())
			{
				return;
			}
		}
	} while (up != 0);
}

int main()
{
	cin >> n >> m;
	for (int i = 1; i <= n; i++)
	{
		cin >> a[i].type;
		if (a[i].type == "MP")
		{
			a[i].waitype = "MP";
		}
		else
		{
			a[i].waitype = "DN";
		}

		char pok;
		for (int j = 0; j < 4; j++)
		{
			cin >> pok;
			a[i].poker.push(pok);
			a[i].total[panpai(pok)]++;
		}
		a[i].zb = false;
	}
	for (int i = 0; i < m; i++)
	{
		char z;
		cin >> z;
		heap.push(z);
	}

	while (!isend)
	{
		for (int i = 1; i <= n; i++)
		{
			if (a[i].heart <= 0)
			{
				continue;
			}
			mopai(i, 2);
			chupai(i);
			if (isend)
			{
				break;
			}
		}
	}

	if (a[1].heart <= 0)
	{
		cout << "FP" << endl;
	}
	else
	{
		cout << "MP" << endl;
	}

	for (int i = 1; i <= n; i++)
	{
		print(i);
	}
}
