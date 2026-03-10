#include <cstdio> 
#include <typeinfo>
#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstring>



// to compile: g++ -std=c++17 main.cpp -03 -o main_executable
struct Reader {



    Reader(const std::filesystem::path& file_path) {
        data_path = file_path;
        buf.resize(BUF_SIZE);
    }



    void establish_link() {
        std::ifstream data_link(data_path, std::ios::binary);
        if (!data_link.is_open()) {
            std::cerr << "Error: Could not open the file, check the file path\n";
            throw std::runtime_error("Faulty link terminates program");
        } 
        std::cerr << "Opend file\n";
        std::cout << data_link.rdbuf();
    }

    void data_to_buffer() {

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
                //processhere
                carry.clear();
                p = new_line + 1;
        }
        const char* line = p;
        while (line < end) {
            const char* new_line = (const char*)std::memchr(line, '\n', (size_t)(end - line));
            if (!new_line) {
                break;
            }
            const char* le = new_line;
            if (le > line && le[-1] == '\n') {
                --le;
            }
            //process_line(line, le);
            line = new_line + 1;
        }


        if (line < end) {
            carry.append(line, end - line);
        }


    }
            
        
        

        
    }
   
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
    //reader.data_to_buffer();

}


int main(){
  orchestrator();
  return 0;
}