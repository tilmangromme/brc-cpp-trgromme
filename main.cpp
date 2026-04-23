#include <cstdio> 
#include <typeinfo>
#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <map>




struct CityStats {
    int min;
    int max;
    int64_t sum;
    std::uint64_t count;
    CityStats(int val)
    : min(val), max(val), sum(val), count(1) {}
    void update(int val) {
        if (val < min) min = val;
        if (val > max) max = val;
        sum += val;
        count += 1;
    }
    double mean() const {
        return static_cast<double>(sum) / count / 10.0;
    }
};

// to compile: g++ -std=c++17 main.cpp -O3 -o main_executable
struct Reader {
    Reader(const std::filesystem::path& file_path) {
        data_path = file_path;
        buf.resize(BUF_SIZE);
        city_vals.reserve(1000000);
    }
    void establish_link() {
        //std::ifstream data_link(data_path, std::ios::binary);
        data_link.open(data_path, std::ios::binary);
        if (!data_link.is_open()) {
            std::cerr << "Error: Could not open the file, check the file path\n";
            throw std::runtime_error("Faulty link terminates program");
        } 
        std::cerr << "Opened file\n";
        
    }
    void text_to_mapping() {
        std::string carry;
        carry.reserve(1024);
        while(true) { 
            data_link.read(buf.data(), (std::streamsize)buf.size());
            std::streamsize n = data_link.gcount();
            if (n <= 0) {
                break;
            }
            const char* p   = buf.data();
            const char* end = p + n;
            if (!carry.empty()) {
                const char* new_line = (const char*)std::memchr(p, '\n', (size_t)(end - p));
                if (!new_line) {
                    carry.append(p, end - p);
                    continue;
                }
                carry.append(p, new_line - p);
                const char* lb = carry.data();
                const char* le = carry.data() + carry.size();
                if (le > lb && le[-1] == '\n') {
                    le--;
                }
                const char* line_itr = lb;
                for (; line_itr != le; ++line_itr) {
                    if (*line_itr == ';') {
                        std::string_view sv(lb, line_itr - lb);
                        int val = parse_num(line_itr + 1, le);
                        auto key_it = city_vals.find(sv);
                        if (key_it == city_vals.end()) {
                            city_vals.emplace(std::string(sv), CityStats(val));
                        } else {
                            key_it -> second.update(val);
                        }
                    }
                }
                carry.clear();
                p = new_line + 1;
            }
            const char* line = p;
            while (line < end) {
                const char* new_line = (const char*)std::memchr(line, '\n', (size_t)(end - line));
                if (!new_line) break;
                const char* le = new_line;
                if (le > line && le[-1] == '\n') {
                    --le;
                }
                const char* line_itr = line;
                for (; line_itr != le; ++line_itr) {
                    if (*line_itr == ';') {
                        std::string_view sv(line, line_itr - line);
                        int val = parse_num(line_itr + 1, le);
                        auto key_it = city_vals.find(sv);
                        if (key_it == city_vals.end()) {
                            city_vals.emplace(std::string(sv), CityStats(val));
                        } else {
                            key_it -> second.update(val);
                        }
                    }
                }
                line = new_line + 1;
                p = line;
            }
            if (p < end) {
                carry.append(p, end - p);
            } 
        }
    }
    void map_process() {
        //std::map<std::string, CityStats> sorted_city_vals(city_vals.begin(), city_vals.end());
        std::vector<std::pair<std::string, CityStats>> entries(
            city_vals.begin(), city_vals.end()
        );
        std::sort(entries.begin(), entries.end(),
            [](const auto& a, const auto& b) {
            return a.first < b.first;
            });

        std::cout << std::fixed << std::setprecision(1);
        std::cout << "{";
        for (const auto& [city, stats] : entries) {
            std::cout << city << "=" 
                      << stats.min / 10 << "." << abs(stats.min % 10) << "/"
                      << stats.mean() << "/"
                      << stats.max / 10 << "." << abs(stats.max % 10) << ", ";
        }   
        std::cout << "}\n";
    }

    inline int parse_num(const char* start, const char* end) {
        int sign = 1;
        if (*start == '-') {
            sign = -1;
            ++start;
        }
        int val = 0;
        while (start < end && *start != '.') {
            val = val * 10 + (*start - '0');
            ++start;
        }
        val *= 10;
        if (start < end && *start == '.') {
            ++start;
            if (start < end) {
                val += (*start - '0');
            }
        }
        return sign * val;
    }
    struct TransparentHash {
        using is_transparent = void;
        std::size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }
        std::size_t operator()(const std::string& s) const noexcept {
            return std::hash<std::string_view>{}(s);
        }
    };
    struct TransparentEqual {
        using is_transparent = void;

        bool operator()(std::string_view left, std::string_view right) const noexcept {
            return left == right;
        }
        bool operator()(const std::string& left, const std::string& right) const noexcept {
            return left == right;
        }
        bool operator()(const std::string& left, std::string_view right) const noexcept {
            return left == right;
        }
        bool operator()(std::string_view left, const std::string& right) const noexcept {
            return left == right;
        }
        
    };
    std::unordered_map<std::string, CityStats, TransparentHash, TransparentEqual> city_vals;
    std::ifstream data_link;
    std::filesystem::path data_path;
    static constexpr std::size_t BUF_SIZE = 8ull*1024*1024;
    std::vector<char> buf;
}; 




void orchestrator() {
    std::string path = "../billionrowchallenge/1brc/data/measurements.txt";
    Reader reader(path);
    try {
        reader.establish_link();
    } catch (const std::runtime_error& e) {
        printf("File unable to be read, double check the file path.\n");
        return;
    }
    reader.text_to_mapping();
    //reader.map_process();
}
int main(){
    auto start = std::chrono::steady_clock::now();
    orchestrator();
    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << diff.count() << " ms\n";
    return 0;
}
