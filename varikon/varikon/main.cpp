#include <array>
#include <iostream>
#include <iomanip>
#include <string>
#include <deque>
#include <algorithm>
#include <ctime>
#include <cstdlib>

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
	//конструкторы
	State() {}

	State(std::array<char, size> init_st, size_t notch_index) :st(init_st) {
		null_index = -1;
		notch = std::pair<char, size_t>({ 'E', notch_index });
	}

	State(std::string &s, size_t notch_index) {
		st = Parser(s);
		null_index = -1;
		notch = std::pair<char, size_t>({ 'E', notch_index });
	}

	//конструктор копии
	State(const State &s) {
		null_index = s.null_index;
		notch = s.notch;
		for (int i = 0; i < size; ++i)
			st[i] = s.st[i];
	}

	//оператор проверки на равенство
	friend const bool operator == (const State &s1, const State &s2)
	{
		if (s1.st.size() != s2.st.size())
			return false;
		else {
			for (int i = 0; i < s1.st.size(); i++)
				if (s1.st[i] != s2.st[i])
					return false;
			if (s1.notch.second != s2.notch.second)
				return false;
		}
		return true;
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

	//эвристика //TO DO
	int heuristic() const {
		int md = 0;
		int value = 0;
		for (int i = 0; i < size; ++i) {
			if (st[i] >= '0' && st[i] <= '9')
				value = st[i] - '0';
			else
				value = st[i] + 10 - 'A';

			if (value != 0) {
				int drow = i / 4 - (value - 1) / 4; //curr - goalr 
				int dcol = i % 4 - (value - 1) % 4; //curc - goalc
				md += abs(drow) + abs(dcol);
			}
		}
		return md;
	};

	//можно ли получить решение
	//bool isValid() {
	//	int sum = 1;
	//	for (int i = 0; i < size - 1; ++i) {
	//		for (int j = i + 1; j < size; ++j) {
	//			if (st[i] > st[j] && st[j] != '0')
	//				++sum;
	//		}

	//	}
	//	sum += null_index / 4;
	//	return (sum % 2 == 0);
	//};

	//выполнение хода //TO DO
	State makeMove(Move m, size_t index) {
		State s(*this);

		switch (m){
		case Up:
			std::swap(s.st[s.null_index], s.st[s.null_index + 4]);
			s.null_index += 4;
			break;
		case Down:
			std::swap(s.st[s.null_index], s.st[s.null_index - 4]);
			s.null_index -= 4;
			break;
		case Left:
			std::swap(s.st[s.null_index], s.st[s.null_index + 1]);
			++s.null_index;
			break;
		case Right:
			std::swap(s.st[s.null_index], s.st[s.null_index - 1]);
			--s.null_index;
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
			return false;
			break;
		case Left: return true;
			break;
		case Right: return true;
			break;
		default:
			break;
		}
	};

	//создание случайного состояния
	void genRandomState() {
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

	//проверка на целевое ???
	/*bool isFinal() {
		std::array<char, size> temp = { '1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','0' };
		return std::equal(st.begin(), st.end(), temp.begin());
	};*/
};

//вспомогательная функция для IDA* //TO DO
//int Search(std::deque<State>&path, int g, int bound, bool &found) {
//	State s = path.back();
//	int f = g + s.heuristic();
//	if (f > bound) return f;
//	if (s.isFinal()) {
//		found = true;
//		return bound;
//	}
//	int min = INT_MAX;
//	for (int i = 0; i < 4; ++i) {
//		if (s.canMove((Move)i)) {
//			State new_state = s.makeMove((Move)i);
//			if (std::find(path.begin(), path.end(), new_state) == path.end()) {
//				path.push_back(new_state);
//				bool fnd = false;
//				int t = Search(path, g + 1, bound, fnd);
//				if (fnd) {
//					found = true;
//					return bound;
//				}
//				if (t < min) min = t;
//				path.pop_back();
//			}
//		}
//	}
//	return min;
//}

//IDA* //TO DO
//void Solve(State state) {
//	int bound = state.heuristic();
//	std::deque<State> path;
//	path.push_back(state);
//	bool found = false;
//	while (!found) {
//		int t = Search(path, 0, bound, found);
//		if (found) break;
//		bound = t;
//	}
//	for each(State s in path) {
//		s.printState();
//		std::cout << std::endl;
//	}
//
//	std::cout << "Steps: " << path.size() - 1 << std::endl;
//}

int main() {
	State state;
	state.genRandomState();
	state.printState();

	/*srand(time(0));
	time_t start = clock();
	std::cout.precision(10);
	std::cout << "Time: " << double(clock() - start) / CLOCKS_PER_SEC << " seconds\n";*/

}