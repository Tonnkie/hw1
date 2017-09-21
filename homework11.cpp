#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<map>
#include<unordered_map>    
#include<math.h>
#include<queue>
#include<random>
#include<time.h>

using namespace std;

struct Node
{
	int x;
	int y;
	int depth;
	Node* parent;
};

//	q_i: i^th queen			m_n: matrix number			q_n: queen number
void init(string& method, int& n, int& lizards, vector<vector<int>>& matrix, vector<int>& block_row);
bool collision(int x, int y, int x2, int y2, vector<vector<int>> matrix);
int check(int row, int column, int q_i, vector<vector<int>>& matrix, vector<int> queen_x, vector<int> queen_y);
int DFS(int row, int column, int q_i, int m_n, int q_n, vector<vector<int>>& matrix, vector<int>& queen_x, vector<int>& queen_y, int& exist);
int BFS(int m_n, int q_n, vector<vector<int>>& matrix, int& exist);
void initSA(int m_n, int q_n, vector<vector<int>>& matrix, vector<int> block_row, vector<pair<int, int>>& q);
void exchange(int m_n, int q_n, vector<vector<int>>& matrix, vector<pair<int, int>>& q, vector<int> block_row, int dE);
double evaluate(int m_n, int q_n, vector<vector<int>> matrix, vector<pair<int, int>> q);
int evaluate(vector<int> matrix, int N);
int SA(int m_n, int q_n, vector<vector<int>>& matrix, vector<int> block_row, int& exist);
int SA_noTree(int m_n, int& exist);
void mprint(int n, vector<int> queen_x, vector<int> queen_y, vector<vector<int>> m);
void mprint(int n, vector<pair<int, int>> q, vector<vector<int>> m);
void mprint(vector<int> matrix, int N);

ofstream fw;
unordered_map<int, int> block_x;
unordered_map<int, int> block_y;
vector<vector<vector<vector<int>>>> cls;
default_random_engine e;
uniform_real_distribution<double> u(0, 1);

int main() {
	int n = 0, lizards = 0;
	string method;
	vector<vector<int>> matrix;
	vector<int> block_row;
	init(method, n, lizards, matrix, block_row);
	fw.open("./output.txt");

	

	int exist = 0;
	if (method == "DFS") {
		vector<vector<vector<int>>> v_temp(n, vector<vector<int> >(n, vector<int>(n, 0)));
		for (int i = 0; i < n; i++) {
			cls.push_back(v_temp);
		}
		clock_t start, finish;
		start = clock();
		
		vector<int> queen_x(lizards, 0), queen_y(lizards, 0);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (matrix[i][j] == 0) {
					queen_x[0] = i;
					queen_y[0] = j;
					DFS(i, j, 1, n, lizards, matrix, queen_x, queen_y, exist);
					queen_x[0] = 0;
					queen_y[0] = 0;
				}
			}
		}
		finish = clock();
		//fw << "\n" << (double)(finish - start) / CLOCKS_PER_SEC << endl;
	}
	if (method == "BFS") {
		vector<vector<vector<int>>> v_temp(n, vector<vector<int> >(n, vector<int>(n, 0)));
		for (int i = 0; i < n; i++) {
			cls.push_back(v_temp);
		}
		BFS(n, lizards, matrix, exist);
	}
	if (method == "SA") {
		clock_t start, finish;
		start = clock();
		if (block_row.size() > 0) {
			vector<vector<vector<int>>> v_temp(n, vector<vector<int> >(n, vector<int>(n, 0)));
			for (int i = 0; i < n; i++) {
				cls.push_back(v_temp);
			}
			SA(n, lizards, matrix, block_row, exist);
		}
		else
			SA_noTree(n, exist);
		finish = clock();
		//fw << "\n" <<(double)(finish - start) / CLOCKS_PER_SEC << endl;
	}

	if (exist == 0) {
		fw << "FAIL";
		//printf("FAIL\n");
	}
	fw.close();
	return 0;
}

int DFS(int row, int column, int q_i, int m_n, int q_n, vector<vector<int>>& matrix, vector<int>& queen_x, vector<int>& queen_y, int& exist) {
	if (exist == 1)
		return 1;

	if (q_i == q_n) {
		exist = 1;
		fw << "OK\n";
		mprint(m_n, queen_x, queen_y, matrix);
		return 1;
	}
	
	int flag = 0, flag2=0, column2;
	for (int temp = column + 1; temp < m_n; temp++) {
		while ( temp<m_n &&matrix[row][temp] == 2) {
			temp++;
			flag2 = 1;
		}
		if (flag2 == 1 && temp<m_n) {
			if (matrix[row][temp] == 0 && check(row, temp, q_i, matrix, queen_x, queen_y) == 1) {
				column2 = temp;
				flag = 1;
				break;
			}
		}
	}


	if (flag == 1) {
		queen_x[q_i] = row;
		queen_y[q_i] = column2;
		DFS(row, column2, q_i + 1, m_n, q_n, matrix, queen_x, queen_y, exist);
		queen_x[q_i] = 0;
		queen_y[q_i] = 0;
	}
	else {
		for (int i = row+1; i < m_n; i++) {
			for (int j = 0; j < m_n; j++) {
				//not at the point of block && valid point
				if (matrix[i][j] == 0 && check(i, j, q_i, matrix, queen_x, queen_y) == 1) {
					queen_x[q_i] = i;
					queen_y[q_i] = j;
					DFS(i, j, q_i + 1, m_n, q_n, matrix, queen_x, queen_y, exist);
					queen_x[q_i] = 0;
					queen_y[q_i] = 0;
					flag = 1;
				}
			}
			if (flag == 1)
				break;
		}
	}
	return 0;
}



int BFS(int m_n, int q_n, vector<vector<int>>& matrix, int& exist) {
	queue<Node*> q;
	int flag = 0;//if would not like to put all begin possible node in the beginning. 
	for (int i = 0; i < m_n; i++) {
		for (int j = 0; j < m_n; j++) {
			if (matrix[i][j] != 2) {
				Node* mynode = (Node*)malloc(sizeof(Node));
				mynode->x = i;
				mynode->y = j;
				mynode->depth = 0;
				mynode->parent = NULL;
				q.push(mynode);
				flag = 1;
			}
		}
		if (flag == 1)
			break;
	}

	while (!q.empty()) {
		Node* mynode = q.front();
		q.pop();

		vector<int> queen_x;
		vector<int> queen_y;
		Node* temp = mynode;
		do {
			queen_x.push_back(temp->x);
			queen_y.push_back(temp->y);
			temp = temp->parent;
		} while (temp != NULL);
		/*
		if (queen_x.size()>2)
				if (queen_x[0] == 2 && queen_y[0] == 3)
					if (queen_x[1] == 1 && queen_y[1] == 6)
						if (queen_x[2] == 1 && queen_y[2] == 0)
							cout << "here" << endl;
		*/

		if (mynode->depth == q_n - 1) {
			exist = 1;
			fw << "OK\n";
			mprint(m_n, queen_x, queen_y, matrix);
			return 1;
		}

		int depth = mynode->depth + 1, q_i = queen_x.size();
		int flag = 0, y = mynode->y, row2 = mynode->x, row = row2 + 1, column = 0;
		for (int temp = y + 1; temp < m_n; temp++) {
			while (temp<m_n && matrix[row2][temp] == 2) {
				flag = 1;
				temp++;
			}
			if (flag == 1 && temp<m_n) {
				column = temp;
				break;
			}
		}

		while (row < m_n) {
			int flag = 0;
			if (column > 0) {
				for (int i = column; i < m_n; i++) {
					if (matrix[row2][i] == 0 && check(row2, i, q_i, matrix, queen_x, queen_y) == 1) {
						Node* newnode = (Node*)malloc(sizeof(Node));
						newnode->x = row2;
						newnode->y = i;
						newnode->depth = depth;
						newnode->parent = mynode;
						q.push(newnode);
						flag = 1;
					}
				}
			}
			if(flag == 0){
				for (int i = 0; i < m_n; i++) {
					if (matrix[row][i] == 0 && check(row, i, q_i, matrix, queen_x, queen_y) == 1) {
						Node* newnode = (Node*)malloc(sizeof(Node));
						newnode->x = row;
						newnode->y = i;
						newnode->depth = depth;
						newnode->parent = mynode;
						q.push(newnode);
						flag = 1;
					}
				}
			}
			if (flag == 0) 
				row++;
			if (flag == 1)
				break;
		}
	}
	return 0;
}


int SA(int m_n, int q_n, vector<vector<int>>& matrix, vector<int> block_row, int& exist) {
	
	double rate = 0.99;
	double T0 = 5;
	double T = T0;
	double T_min = 1e-12;
	int iner_loop = 3;
	double dE = 0.0;
	int deTime = 0;
	vector<pair<int, int>> q;
	initSA(m_n, q_n, matrix, block_row, q);
	vector<vector<int>> matrix_new;
	vector<pair<int, int>>  q_new;
	int time = 0;
	exist=0;
	while (time < 10) {
		while (T > T_min) {
			//for (int iner = 0; iner < iner_loop; iner++) {
				int score = evaluate(m_n, q_n, matrix, q);
				//cout << score << "  " << T<< endl;
				if (score == 0) {
					exist = 1;
					fw << "OK\n";
					mprint(m_n, q, matrix);
					return 0;
				}
				matrix_new = matrix;
				q_new = q;
				exchange(m_n, q_n, matrix_new, q_new, block_row, dE);
				dE = score - evaluate(m_n, q_n, matrix_new, q_new);

				if ((dE==0 && u(e)>0.5) || dE > 0 ) {
					matrix = matrix_new;
					q = q_new;
				}
				else { 
					double pos = exp(dE / T);
					double pos2 = u(e);
					if (pos > pos2) {
						matrix = matrix_new;
						q = q_new;
					}
				}
			//}
			T = rate * T;
		}
		if (!exist) {
			T = T0;
			time++;
		}
	}

	return 1;
}


int SA_noTree(int m_n, int& exist) {
	int time = 0;
	exist = 0;
	double rate = 0.99;
	double T0 = 5;
	double T = T0;
	double T_min = 1e-10;
	if (m_n / 10) {
		T_min *= 0.1;
	}
	int iner_loop = 3;
	double dE = 0.0;
	vector<int> matrix(m_n, 0);
	vector<int> matrix_new(m_n, 0);;
	//init
	for (int i = 0; i<m_n; ++i) {
		matrix[i] = i;
	}
	for (int i = 0; i<m_n; ++i) {
		int r = rand() % m_n;;
		swap(matrix[r], matrix[m_n - r - 1]);
	}

	while (time < 10) {
		while (T > T_min) {
			int score = evaluate(matrix, m_n);
			//cout << score << "  " << T << endl;
			if (score == 0) {
				exist = 1;
				fw << "OK\n";
				mprint(matrix, m_n);
				return 0;
			}
			//exchange
			mt19937 rng;
			rng.seed(random_device()());
			uniform_int_distribution<mt19937::result_type> dist(0, m_n - 1);
			matrix_new = matrix;
			swap(matrix_new[dist(rng)], matrix_new[dist(rng)]);
			//-------
			dE = score - evaluate(matrix_new, m_n);
			if ((dE == 0 && u(e)>0.5) || dE > 0) {
				matrix = matrix_new;
			}
			else {
				double pos = exp(dE / T);
				double pos2 = u(e);
				if (pos > pos2) {
					matrix = matrix_new;
				}
			}
			T = rate * T;
		}
		if (!exist) {
			T = T0;
			time++;
		}
	}
	return 1;
}



int evaluate(vector<int> matrix, int N){

	int collision = 0;
	for (int i = 0; i<N; i++) {
		for (int j = 0; j<N; j++) {
			if (i != j) {
				if ((matrix[j] == matrix[i]) || abs(matrix[j]-matrix[i]) == abs(j - i)) {
					++collision;
				}
			}
		}
	}

	return collision;
}


void exchange(int m_n, int q_n, vector<vector<int>>& matrix, vector<pair<int, int>>& q, vector<int> block_row, int dE) {

	vector<int> c;
	int sum, min = -1, i0, i1, flag=0;
	mt19937 rng;
	rng.seed(random_device()());
	uniform_int_distribution<mt19937::result_type> dist(0, q_n - 1);	//dist(rng);
	i0 = dist(rng);

	if(dE>q_n && u(e) > 0.1){
		for (int i = 0; i < q_n; i++) {
			sum = 0;
			for (int j = 0; j < q_n; j++) {
				if (collision(q[i].first, q[i].second, q[j].first, q[j].second, matrix)) {
					sum++;
				}
			}
			if (min < sum) {
				i1 = i;
				min = sum;
			}
		}
		i0 = i1;
	}
	
	
	int x = q[i0].first;
	int y = q[i0].second;
	uniform_int_distribution<mt19937::result_type> dist2(0, m_n - 1);
	while (true) {
		int x2 = dist2(rng);
		int y2 = dist2(rng);
		if (matrix[x2][y2] == 0) {
			matrix[x2][y2] = 1;
			matrix[x][y] = 0;
			q[i0].first = x2;
			q[i0].second = y2;
			break;
		}
	}
}

//for DFS, BFS, SA
bool collision(int x, int y, int x2, int y2, vector<vector<int>> matrix) {
	bool res = false;
	if(cls[x][y][x2][y2] == 0){
		if (x == x2) {
			if (block_x[x] == 0)
				return true;
			res = true;
			int small = (y > y2) ? y2 : y;
			int large = (y > y2) ? y : y2;
			for (int i = small + 1; i < large; i++) {
				if (matrix[x][i] == 2) {
					res = false;
					break;
				}
			}
		}
		else if (y == y2) {
			if (block_y[y] == 0)
				return true;
			res = true;
			int small = (x > x2) ? x2 : x;
			int large = (x > x2) ? x : x2;
			for (int i = small + 1; i < large; i++) {
				if (matrix[i][y] == 2) {
					res = false;
					break;
				}
			}
		}
		else if ((x - x2) == (y - y2)) {
			res = true;
			int large = (x2 > x) ? x2 : x;
			int small = (x2 > x) ? x : x2;
			int large_y = (y2 > y) ? y2 : y;
			int small_y = (y2 > y) ? y : y2;
			while (small < large) {
				small++;	small_y++;
				if (matrix[small][small_y] == 2) {
					res = false;
					break;
				}
			}
		}
		else if ((x2 - x) == (y - y2)) {
			res = true;
			int large = (x2 > x) ? x2 : x;
			int small = (x2 > x) ? x : x2;
			int large_y = (y2 > y) ? y2 : y;
			int small_y = (y2 > y) ? y : y2;
			while (small < large) {
				small++;	large_y--;
				if (matrix[small][large_y] == 2) {
					res = false;
					break;
				}
			}
		}
		if (res) {//collised
			cls[x][y][x2][y2] = 2;
			cls[x2][y2][x][y] = 2;
		}
		else {//not coliised
			cls[x][y][x2][y2] = 1;
			cls[x2][y2][x][y] = 1;
		}
	}
	else {
		if (2 == cls[x][y][x2][y2]) {
			res = true;
		}
		if (1 == cls[x][y][x2][y2]) {
			res = false;
		}
	}
	return res;
}

//for BFS & DFS
int check(int row, int column, int q_i, vector<vector<int>>& matrix, vector<int> queen_x, vector<int> queen_y) {
	int i = 0;
	while (i < q_i) {
		bool flag = collision(row, column, queen_x[i], queen_y[i], matrix);
		if (flag)
			return 0;
		i++;
	}
	return 1;
}

//for SA
double evaluate(int m_n, int q_n, vector<vector<int>> matrix, vector<pair<int, int>> q) {
	int x, y, x2, y2;
	double sum = 0;
	for (int i = 0; i < q_n; i++) {
		x = q[i].first;
		y = q[i].second;
		for (int j = i + 1; j < q_n; j++) {
			if (j != i) {
				x2 = q[j].first;
				y2 = q[j].second;
				if (collision(x, y, x2, y2, matrix))
					sum++;
			}
		}
	}
	return sum;
}


void initSA(int m_n, int q_n, vector<vector<int>>& matrix, vector<int> block_row, vector<pair<int, int>>& q) {
	mt19937 rng;
	rng.seed(random_device()());
	uniform_int_distribution<mt19937::result_type> dist(0, m_n - 1);	//dist(rng);

	int temp, i = 0;
	int row = 0;
	while (i < q_n) {
		int flag = 0;
		while (row < m_n) {
			for (int j = 0; j < m_n; j++) {
				if (matrix[row][j] == 0) {
					flag = 1;
					break;
				}
			}
			if (flag == 0)
				row++;
			else
				break;
		}
		while (row < m_n) {
			temp = dist(rng);
			if (matrix[row][temp] == 0) {
				matrix[row][temp] = 1;
				q.push_back(make_pair(row, temp));
				break;
			}
		}
		while (row >= m_n) {
			uniform_int_distribution<mt19937::result_type> dist2(0, block_row.size() - 1);
			int x = dist2(rng);
			int y = dist(rng);
			if (matrix[x][y] == 0) {
				matrix[x][y] = 1;
				q.push_back(make_pair(x, y));
				break;
			}
		}
		row++;
		i++;
	}
}


//--------------------------------------------- Initialization

void mprint(int n, vector<int> queen_x, vector<int> queen_y, vector<vector<int>> m) {
	for (int i = 0; i<queen_x.size(); i++)
		m[queen_x[i]][queen_y[i]] = 1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			fw << m[i][j];
			//printf("%d", m[i][j]);
		}
		if (i != n - 1) {
			fw << "\n";
		}
		//printf("\n");
	}
	//printf("-----------\n");
}

void mprint(int n, vector<pair<int, int>> q, vector<vector<int>> m) {
	for (int i = 0; i<q.size(); i++)
		m[q[i].first][q[i].second] = 1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			fw << m[i][j];
			//printf("%d", m[i][j]);
		}
		if (i != n - 1) {
			fw << "\n";
		}
		//printf("\n");
	}
	//printf("-----------\n");
}

void mprint(vector<int> matrix, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (matrix[i] == j) {
				fw << "1";
			}
			else {
				fw << "0";
			}
		}
		if (i != n - 1) {
			fw << "\n";
		}
	}
}


void init(string& method, int& n, int& lizards, vector<vector<int>>& matrix, vector<int>& block_row) {
	fstream fr;
	fr.open("./input.txt");
	if (!fr.is_open()) { cout << "error" << endl; return; }
	fr >> method;
	fr >> n;
	fr >> lizards;
	string stemp;
	getline(fr, stemp);
	vector<int> tmatrix;
	int row = 0, flag = 0;
	while (getline(fr, stemp)) {
		for (int column = 0; column < n; column++) {
			tmatrix.push_back(stemp[column] - '0');
			if (stemp[column] - '0' == 2) {
				flag = 1;
			}
		}
		if (flag == 1) {
			block_row.push_back(row);
			block_x[row]= 1;
			flag = 0;
		}
		else {
			block_x[row] = 0;
		}
		row++;
		matrix.push_back(tmatrix);
		tmatrix.clear();
	}
	flag = 0;
	for (int j = 0; j < n; j++) {
		for (int i = 0; i < n; i++) {
			if (matrix[i][j] == 2) {
				flag = 1;
			}
		}
		if (flag == 1) {
			block_y[j] = 1;
			flag = 0;
		}else {
			block_y[j] = 0;
		}
	}

}

