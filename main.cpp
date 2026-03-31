#include <cstdio> 
#include <typeinfo>
#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <string>
#include <charconv>
#include <unordered_map>



// to compile: g++ -std=c++17 main.cpp -O3 -o main_executable
struct Reader {



    Reader(const std::filesystem::path& file_path) {
        data_path = file_path;
        buf.resize(BUF_SIZE);
    }


    void establish_link() {
        //std::ifstream data_link(data_path, std::ios::binary);
        data_link.open(data_path, std::ios::binary);
        if (!data_link.is_open()) {
            std::cerr << "Error: Could not open the file, check the file path\n";
            throw std::runtime_error("Faulty link terminates program");
        } 
        std::cerr << "Opened file\n";
        //std::cout << data_link.rdbuf();
    }

    void data_to_buffer() {
        printf("Processing file in chunks...\n");
        //std::unordered_map<std::string, std::vector<double>> city_vals;
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
                double value;
                std::string city_name;
                const char* line_itr = lb;
                for (; line_itr != le; ++line_itr) {
                    if (*line_itr == ';') {
                        city_name = std::string(lb, line_itr);
                        std::string temp_val(line_itr + 1, le-2);
                        value = std::stod(temp_val);
                    }
                }
                city_vals[city_name].update(value);
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
                //double value;
                //std::string city_name;
                //const char* line_itr = line;
                //for (; line_itr != le; ++line_itr) {
                    //if (*line_itr == ';') {
                        //city_name = std::string(line, line_itr);
                        //std::string temp_val(line_itr + 1, le-2);
                        //value = std::stod(temp_val);
                    //}
                //}
                const char* line_itr = line;
                for (; line_itr != le; ++line_itr) {
                    if (*line_itr == ';') {
                        std::string_view name(line, line_itr);
                        std::cout << "city name: " << name << '\n';
                        double val = parse_num(line_itr + 1, le-2);
                    }
                }
                
                line = new_line + 1;
            }
            if (p < end) {
                carry.append(p, end - p);
            } 
        }
  
    }

    void map_process() {
        printf("here!\n");
        //std::cout << "size: " << city_vals.size() << '\n';
        int count = 0;
        for (const auto& [city, values] : city_vals) {
            if (count++ >= 10) break;
            std::cout << city << ": ";
            for (double v : values) {
                std::cout << v << " ";
            }
            std::cout << '\n';
        }
    }

    double parse_num(const char* start, const char* end) {
        
    }
    // Here are all the thing we want initialized for the reader
    //
    //
    struct CityStats {
        double min;
        double max;
        double sum;
        std::uint64_t count;
        CityStats(double val)
        : min(val), max(val), sum(val), count(1) {}

        void update(double val) {
            if (val < min) min = val;
            if (val > max) max = val;
            sum += val;
            count += 1;
        }
        double mean() const {
            return sum / count;
        }
    };
    std::unordered_map<std::string, CityStats>city_vals;
    std::ifstream data_link;
    std::filesystem::path data_path;
    static constexpr std::size_t BUF_SIZE = 8ull*1024*1024;
    std::vector<char> buf;
}; 




void orchestrator() {
    
    std::string path = "../billionrowchallenge/1brc/data/data.txt";
    Reader reader(path);
    try {
        reader.establish_link();
    } catch (const std::runtime_error& e) {
        printf("File unable to be read, double check the file path.\n");
        return;
    }
    reader.data_to_buffer();
    printf("Finished processing file, now mapping...\n");
    //reader.map_process();
    //printf("Done!\n");

}


int main(){
  orchestrator();
  return 0;
}