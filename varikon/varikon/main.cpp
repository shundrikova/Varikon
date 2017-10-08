#include <array>
#include <iostream>
#include <iomanip>
#include <string>
#include <deque>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <set>
#include <queue>

const size_t size = 25;

std::array<char, size> Parser(std::string s) {
	std::array<char, size> st;
	for (int i = 0; i < s.length(); ++i) {
		st[i] = s[i];
	}
	return st;
}

enum Move { Up, Down, Left, Right };

class State {
	std::array<char, size> st;
	std::pair<char, size_t> notch;
	int null_index;

public:
	int g = 0;
	State *parent = 0;
	//конструкторы
	State() {}

	State(std::string &s, std::pair<char, size_t> n):notch(n) {
		st = Parser(s);
		if (n.first == 'E')
			null_index = -1;
		else {
			for (int i = 0; i < size; ++i)
				if (st[i] == 'E')
					null_index = i;
		}
	}

	//конструктор копии
	State(const State &s) {
		null_index = s.null_index;
		notch = s.notch;
		for (int i = 0; i < size; ++i)
			st[i] = s.st[i];
		g = s.g;
		parent = s.parent;
	}

	/*State(const State &s, const State *p) {
		null_index = s.null_index;
		notch = s.notch;
		for (int i = 0; i < size; ++i)
			st[i] = s.st[i];
		g = s.g;
		*parent = *p;
	}
*/
	//оператор проверки на равенство
	friend const bool operator == (const State &s1, const State &s2)
	{
		if (s1.st.size() != s2.st.size())
			return false;
		else {
			for (int i = 0; i < s1.st.size(); i++)
				if (s1.st[i] != s2.st[i])
					return false;
		}
		if (s1.notch.second != s2.notch.second)
			return false;
		return true;
	}

	friend bool operator<(const State& s1, const State& s2)
	{
		for (int i = 0; i < size; i++)
		{
			if (s1.st[i] < s2.st[i])
				return true;
			if (s1.st[i] > s2.st[i])
				return false;
		}
		return false;
	}

	//вывод состояния
	void printState() {
		int cnt = 0;
		for (int i = 0; i < 6; ++i) {
			for (int j = 0; j < 5; ++j) {
				if (!i) {
					if (j == notch.second)
						std::cout << notch.first;
					std::cout << "  ";
				}
				else std::cout << st[cnt++] << ' ';
			}
			std::cout << std::endl;
		}
	};

	//эвристика
	int heuristic() const {
		int h = 0;
		int j = 0;
		while (j < 5) {
			int r = 0, g = 0, b = 0, y = 0, w = 0;
			for (int i = j; i < j + 21; i += 5) {
				switch (st[i])
				{
				case 'R': ++r;
					break;
				case 'G': ++g;
					break;
				case 'B': ++b;
					break;
				case 'Y': ++y;
					break;
				case 'W': ++w;
					break;
				default:
					break;
				}
			}
			int max = std::max({ r, g, b, y, w });
			h += 5 - max;
			j++;
		}
		return h;
	};

	//выполнение хода
	State makeMove(Move m, int index) {
		State s(*this);

		switch (m){
		case Up:
			if (null_index != -1) {
				std::swap(s.st[s.null_index], s.st[s.null_index + 5]);
				s.null_index += 5;
			}
			else {
				std::swap(s.st[notch.second], s.notch.first);
				s.null_index = notch.second;
			}
			break;
		case Down: 
			if (null_index < 5) {
				std::swap(s.st[s.null_index], s.notch.first);
				s.null_index = -1;
			}
			else {
				std::swap(s.st[s.null_index], s.st[s.null_index - 5]);
				s.null_index -= 5;
			}
			break;
		case Left:
			if (index != -1)
				std::rotate(s.st.begin() + index * 5, s.st.begin() + index * 5 + 1, s.st.begin() + index * 5 + 5);
			else {
				s.notch.second = (s.notch.second + 4) % 5;
			}
			break;
		case Right:
			if (index != -1)
				std::rotate(s.st.begin() + index * 5, s.st.begin() + index * 5 + 4, s.st.begin() + index * 5 + 5);
			else {
				s.notch.second = (s.notch.second + 1) % 5;
			}
			break;
		default:
			break;
		}
		return s;
	};

	//можно ли сделать следующий ход
	bool canMove(Move m) {
		switch (m){
		case Up:
			if (null_index < 20)
				return true;
			return false;
			break;
		case Down:
			if (null_index < 5 && null_index % 5 != notch.second % 5)
				return false;
			return true;
			break;
		case Left:
		case Right: return true;
			break;
		}
	};

	//создание случайного состояния
	void genRandomState() {
		srand(time(0));
		st = { 'R', 'R', 'R', 'R', 'R', 
			'G', 'G', 'G', 'G', 'G', 
			'B', 'B', 'B', 'B', 'B', 
			'Y', 'Y', 'Y', 'Y', 'Y', 
			'W', 'W', 'W', 'W', 'W'};
		std::random_shuffle(st.begin(), st.end());
		srand(time(0));
		size_t rndn = rand() % 5;
		notch = std::pair<char, size_t>({ 'E', rndn });
		null_index = -1;
	};

	//проверка на целевое
	bool isFinal() {
		return (heuristic() == 0);
	};
};



//вспомогательная функция для IDA* //TO DO
int Search(std::deque<State>&path, int g, int bound, bool &found) {
	State s = path.back();
	int f = g + s.heuristic();
	if (f > bound) return f;
	if (s.isFinal()) {
		found = true;
		return bound;
	}
	int min = INT_MAX;
	for (int i = 0; i < 4; ++i) {
		if (s.canMove((Move)i)) {
			int index = -1;
			if ((Move)i == Up || (Move)i == Down) index = 4;
			for (index; index < 5; ++index) {
				State new_state = s.makeMove((Move)i, index);
				if (std::find(path.begin(), path.end(), new_state) == path.end()) {
					path.push_back(new_state);
					bool fnd = false;
					int t = Search(path, g + 1, bound, fnd);
					if (fnd) {
						found = true;
						return bound;
					}
					if (t < min) min = t;
					path.pop_back();
				}
			}
		}
	}
	return min;
}

//IDA* //TO DO
void Solve(State state) {
	int bound = state.heuristic();
	std::deque<State> path;
	path.push_back(state);
	bool found = false;
	while (!found) {
		int t = Search(path, 0, bound, found);
		if (found) break;
		bound = t;
	}
	for each(State s in path) {
		s.printState();
		std::cout << std::endl;
	}

	std::cout << "Steps: " << path.size() - 1 << std::endl;
}

struct q_type
{
	std::set<State>::iterator it;
	int h;
	friend bool operator<(const q_type &p1, const q_type &p2) {
		return p1.h > p2.h;
	}
		
};

void print(State&s) {
	s.printState();
	std::cout << std::endl;
	if (s.parent != 0)
		print(*s.parent);
}

void AStar(State &state){
	std::multiset<State> seen;
	std::priority_queue<q_type> open;
	q_type qel;

	state.g = 0;
	auto emp_res = seen.insert(state);
	qel.h = state.g + state.heuristic();
	qel.it = emp_res;
	open.emplace(qel);
	while (!open.empty())
	{
		State cur = *open.top().it;
		open.pop();
		if (cur.isFinal())
		{
			cur.printState();
			std::cout << "Steps: " << cur.g << std::endl;
			return;
		}

		State child;
		for (int i = 0; i < 4; ++i) {
			if (cur.canMove((Move)i)) {
				int index = -1;
				if ((Move)i == Up || (Move)i == Down) index = 4;
				for (index; index < 5; ++index) {
					child = cur.makeMove((Move)i, index);
					child.g = cur.g + 1;
					child.parent = new State(cur);
					if (std::find(seen.begin(), seen.end(), child) == seen.end()) {
						emp_res = seen.insert(child);
					}
						if (child.isFinal()) {
							print(child);
							std::cout << "Steps: " << child.g << std::endl;
							return;
						}
							
						qel.h =
							child.g +
							child.heuristic();
						qel.it = emp_res;
						open.emplace(qel);					
					
				}
			}
		}
	}
}

// https://en.wikipedia.org/wiki/Fringe_search 

int main() {
	std::string s = "WGBGBGWWWGRRYRYYYBRBYBGWR";
	//std::string s = "RGBYWRGBYWRGBYWWRGBYGBYWR";
	//std::string s = "RGBYWRGBEWBYWRGWRGBYGBYWR";
	std::pair<char, size_t> n = std::pair<char, size_t>({ 'E', 4 });
	State state = State(s, n);
	//State state;
	//state.genRandomState();
	//state.printState();
	std::cout << std::endl;

	srand(time(0));
	time_t start = clock();
	//Solve(state);
	AStar(state);
	std::cout.precision(10);
	std::cout << "Time: " << double(clock() - start) / CLOCKS_PER_SEC << " seconds\n";	

	/*srand(time(0));
	time_t start = clock();
	std::cout.precision(10);
	std::cout << "Time: " << double(clock() - start) / CLOCKS_PER_SEC << " seconds\n";*/

}