#include <vector>
#include <numeric>
#include "pch.h"
#include "loop.h"
using namespace std;

template<class...Durations, class DurationIn>
std::tuple<Durations...> break_down_durations(DurationIn d) {
	std::tuple<Durations...> retval;
	using discard = int[];
	(void)discard {
		0, (void(((std::get<Durations>(retval) = std::chrono::duration_cast<Durations>(d)), (d -= std::chrono::duration_cast<DurationIn>(std::get<Durations>(retval))))), 0)...
	};
	return retval;
}

struct Timer {
	std::chrono::time_point<std::chrono::steady_clock> start, end;

	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		//https://en.cppreference.com/w/cpp/language/structured_binding
		auto[h, m, s, ms, ns] = break_down_durations<chrono::hours, chrono::minutes, chrono::seconds, chrono::milliseconds, chrono::nanoseconds>(end - start);
		cout << endl << "Took function " << h.count() << "h : " << m.count() << "m : " << s.count() << "s : " << ms.count() << "ms to Execute" << endl;
	}

};

template<class T, typename... Args>
auto time_function(T func, Args... args) -> decltype(func(args...)) {
	Timer t;
	return func(args...);
}


void cursor(bool f)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 25;
	info.bVisible = f;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void progress(int * active, double temp, string name = "Progress: ") {
	cursor(false);
	auto start = std::chrono::high_resolution_clock::now();
	double prev_progress = 0;
	double prev_result = 0;
	double eta = 0;
	double updatethresh = 0.01;
	double stepsto100 = 100 / updatethresh;
	auto FmilliTmin = [](double x) {return x / 1000 / 60; };

	while (*active > 0) {
		double progress = 100 * (1 - (*active / temp));
		if (progress - prev_progress >= updatethresh) {
			auto finish = std::chrono::high_resolution_clock::now();
			double diff_from_start = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
			eta = abs((FmilliTmin((diff_from_start - prev_result) * (stepsto100))) - FmilliTmin(diff_from_start));
			std::string current_eta = std::to_string(int(trunc(eta))) + ":" + std::to_string(abs(int(trunc((eta - trunc(eta)) * 60))));
			std::cout << std::setprecision(2) << std::fixed << name << " " << progress << "% eta: " << current_eta << std::string(20, ' ') << "\r";
			prev_result = diff_from_start;
			prev_progress = progress;
		}
	}
	std::cout << name << " 100% eta: zero" << string(20, ' ') << std::endl;
	cursor(true);
}

static inline bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }
static const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";

string base64_encode(unsigned char const* bytes_to_encode, int in_len, string name = "Progress:") {
	double temp = in_len;
	thread st(progress, &in_len, temp, name);
	string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];
	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			ret += base64_chars[char_array_4[0]];
			ret += base64_chars[char_array_4[1]];
			ret += base64_chars[char_array_4[2]];
			ret += base64_chars[char_array_4[3]];

			i = 0;
		}
	}
	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}
	st.join();
	return ret;
}

string base64_decode(std::string const& encoded_string, string name = "Progress:") {
	int lenlen = encoded_string.size();
	double temp = lenlen;
	thread st(progress, &lenlen, temp, name);
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	string ret;

	while (lenlen-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			ret += char_array_3[0];
			ret += char_array_3[1];
			ret += char_array_3[2];
			i = 0;
		}

	}
	st.join();
	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

string encode64(string filename) {
	ifstream fin(filename, ios::binary);
	stringstream osstring;
	osstring << fin.rdbuf();
	string dout = osstring.str();
	return base64_encode((const unsigned char *)dout.c_str(), dout.length());
}

vector<string> encode_vector(vector<string> filenames) {
	vector<string> base_64;
	base_64.resize(filenames.size());
	for (auto file : filenames) {
		ifstream fin(file, ios::binary);
		ostringstream ostring;
		ostring << fin.rdbuf();
		string dout = ostring.str();
		cout << "Encrypted = " << base64_encode((const unsigned char *)dout.c_str(), dout.length(), file).substr(0, 10) << "..." << endl;
		fin.close();
	}
	return base_64;
}

auto encode_file = [](experimental::filesystem::directory_entry s) {
	ifstream fin(s, ios::binary);
	ostringstream os;
	os << fin.rdbuf();
	string dout = os.str();
	cout << "Encrypted = " << base64_encode((const unsigned char *)dout.c_str(), dout.length(), s.path().string()).substr(0, 200) << "..." << endl << endl;
	fin.close();
};

void encode_directory_r(string path) {
	for (experimental::filesystem::directory_entry s : experimental::filesystem::recursive_directory_iterator(path)) {
		if (experimental::filesystem::is_regular_file(s)) {
			encode_file(s);
		}
	}
}
void encode_directory(string path) {
	for (experimental::filesystem::directory_entry s : experimental::filesystem::recursive_directory_iterator(path)) {
		if (experimental::filesystem::is_regular_file(s)) {
			encode_file(s);
		}
	}
}

void decode64(string b64, string output) {
	ofstream fout(output, ios::binary);
	string ofb64 = base64_decode(b64);
	fout << ofb64;
}

string string2b64(string input) {
	return  base64_encode((const unsigned char *)input.c_str(), input.length());
}

string b64tostring(string base64) {
	return base64_decode(base64);
}

void test_func() {
	Sleep(1000);
}

template<typename T>
void display_vector(vector<T> t) {
	for (auto i : t) {
		cout << i;
	}
}

void throw_error(string message, int error_code) {
	MessageBoxA(NULL, message.c_str(), "Error!", MB_ICONERROR | MB_OK);
	exit(error_code);
}
void display_min_max(vector<char> min, vector<char> max) {
	cout << endl << "max: ";
	for (auto c : max) {
		cout << "|" << c << "| ";
	}
	cout << endl << "min: ";
	for (auto c : min) {
		cout << "|" << c << "| ";
	}
}
template<typename T>
void reverse_vector(vector<T> & input) {
	reverse(input.begin(), input.end());
}

class unlimint {
public:
	unlimint(const char * input);
	unlimint(string input);
	friend ostream &operator<<(ostream &output, const unlimint &I) {
		for (auto c : I.digits) {
			output << c;
		}
		return output;
	}
	/*
	>>> 10 << 1
	20
	>>> 10 >> 1
	5
	>>>*/
	int operator[](int index) {
		if (index <= num_digits) {
			return digits[index] - '0';
		}
		else {
			throw_error(to_string(index) + " is out of the vector range of " + to_string(num_digits) + ".", 1);
		}
	};
	unlimint operator*(unlimint input) {
		vector<char> x = digits, y = input.digits;
		//cout << "x = ";
		//display_vector(x);
		//cout << endl;
		//cout << "y = ";
		//display_vector(y);
		//cout << endl << endl;
		vector<string> half;
		vector<string> doubl;
		for (char c : x) { //HALF
			int cd = c - '0';
			cd = (cd >> 1);
			//cout << "c = " << cd << endl;
			half.push_back(to_string(cd));
		}
		//cout << endl;
		for (char v : y) { //DOUBLE
			int vd = v - '0';
			vd = (vd << 1);
			//cout << "v = " << vd << endl;
			doubl.push_back(to_string(vd));
		}
		/*cout << "half = ";
		display_vector(half);
		cout << endl;
		cout << "doubl = ";
		display_vector(doubl);*/
		
		half.resize(half.size() * 2);
		doubl.resize(doubl.size() * 2);
		//cout << endl;

		for (auto i = 0; i < half.size()/2; i++) {
			//cout << "half[" << i << "] = " << half[i] << endl;
			if (stoi(half[i]) >= 10) {
				int base = ((10 % stoi(half[i])) / 10);
				string number = to_string(stoi(half[i]) % 10);
				int next = atoi(half[i + 1].c_str());
				string result_number = to_string(next + base);
				half[i] = number;
				half[i + 1] = result_number;
			}
		}

		for (auto i = 0; i < doubl.size()/2; i++) {
			if (stoi(doubl[i]) >= 10) {
				int base = ((10 % stoi(doubl[i])) / 10);
				string number = to_string(stoi(doubl[i]) % 10);
				int next = atoi(doubl[i + 1].c_str());
				string result_number = to_string(next + base);
				doubl[i] = number;
				doubl[i + 1] = result_number;
			}
		}

		reverse_vector(half);
		reverse_vector(doubl);
		cout << "Halfed -> ";
		display_vector(half);
		cout << endl;
		cout << "Doubled -> ";
		display_vector(doubl);

		return unlimint("123");
	}
	unlimint operator+(unlimint input) {
		vector<string> result;
		vector<char> max_input;
		vector<char> min_input;
		if (digits.size() == input.digits.size()) {
			max_input = digits;
			min_input = input.digits;
		}
		else {
			max_input = ((digits.size() < input.digits.size()) ? input.digits : digits);
			min_input = ((digits.size() > input.digits.size()) ? input.digits : digits);
		}

		reverse_vector(max_input);
		reverse_vector(min_input);

		int required_padding = max_input.size() - min_input.size();

		for (auto i = 0; i < required_padding; i++) {
			min_input.push_back('0');
		}

		int vector_size = (max_input.size() + min_input.size()) / 2;
		for (auto i = 0; i < vector_size; i++) {
			int result_value = ((max_input[i] - '0') + (min_input[i] - '0'));
			result.push_back(to_string(result_value));
		}
		result.resize(result.size() + 1);
		for (auto i = 0; i < vector_size; i++) {
			int current_number = atoi(result[i].c_str());
			if (current_number >= 10) {
				int base = ((10 % current_number) / 10);
				string number = to_string(current_number % 10);
				int next = atoi(result[i + 1].c_str());
				string result_number = to_string(next + base);
				result[i] = number;
				result[i + 1] = result_number;
			}
		}
		reverse_vector(result);
		string output;
		for (auto const& s : result) { output += s; }
		return unlimint(output);
	}
	int number_of_digits() {
		return num_digits;
	}
private:
	bool isInt(string input) { return [&](bool return_value = true) {for (auto c : input) { if (!isdigit(c)) { return_value = false; break; } } return return_value; }(); };
	vector<char> digits;
	int num_digits;
	enum parity { EVEN, ODD };
	parity ev_od;
};


int main()
{
	unlimint s("222222");
	unlimint a("999999");
	unlimint ss = "123123123";
	cout << s + a;
	cout << ss << endl;
	cout << endl << "s * a = "<< s * a << endl << endl;
	//encode_directory("c:\\");
}

unlimint::unlimint(const char * args)
{
	string input(args);
	if (isInt(input)) {
		copy(input.begin(), input.end(), back_inserter(digits));
		num_digits = digits.size();
	}
	else {
		throw_error("Non digit detected in constructor.", 1);
	}
}

unlimint::unlimint(string input)
{
	if (isInt(input)) {
		copy(input.begin(), input.end(), back_inserter(digits));
		num_digits = digits.size();
	}
	else {
		throw_error("Non digit detected in constructor.", 1);
	}
}
