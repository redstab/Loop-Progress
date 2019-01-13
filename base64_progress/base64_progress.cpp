#include <vector>
#include "pch.h"
#include "loop.h"
using namespace std;
template<class duration_value>
struct Timer {
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		cout << endl << "Took function " << chrono::duration_cast<duration_value>(end - start).count() << " to Execute" << endl;
	}
};

template<class T, typename... Args>
auto time_function(T func, Args... args) -> decltype(func(args...)) {
	Timer<chrono::milliseconds> t;
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
			std::cout << std::setprecision(2) << std::fixed << name << " " << progress << "\% eta: " << current_eta << std::string(20, ' ') << "\r";
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

vector<string> encode_directory_r(string path) {
	vector<string> file_path;
	for (auto file : experimental::filesystem::recursive_directory_iterator(path)) {
		if (experimental::filesystem::is_regular_file(file)) {
			file_path.push_back(file.path().string());
		}
	}
	return encode_vector(file_path);
}
vector<string> encode_directory(string path) {
	vector<string> file_path;
	for (auto file : experimental::filesystem::directory_iterator(path)) {
		if (experimental::filesystem::is_regular_file(file)) {
			file_path.push_back(file.path().string());
		}
	}
	return encode_vector(file_path);
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
		cout << string(i).substr(0, 10) << endl;
	}
}

int main()
{
	{
		Timer<chrono::milliseconds> start;
		auto i = experimental::filesystem::directory_iterator("c:\\arc");
		for (experimental::filesystem::directory_entry s : i) {
			if (experimental::filesystem::is_regular_file(s)) {
				ifstream fin(s, ios::binary);
				
				string dout((istreambuf_iterator<char>(fin.rdbuf())), istreambuf_iterator<char>());
				cout << "Encrypted = " << base64_encode((const unsigned char *)dout.c_str(), dout.length(), s.path().string()).substr(0, 200) << "..." << endl << endl;
				fin.close();
			}

		}
	}

	//cout << time_function(encode64, "c:\\a\\s.nrg").substr(0, 100) << endl;
	encode_directory("c:\\windows");
}