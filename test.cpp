
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

// defined in README.c
extern unsigned char README_md[];
extern size_t README_md_size;

std::string read_file(const std::string& file)
{
    std::string result;
    
    std::ifstream input(file.c_str(), std::ios::binary);
    if (input.bad())
    {
        std::stringstream msg;
        msg << "Failed to open file " << file << " for reading.";
        throw std::runtime_error(msg.str());
    }
    
    int c = input.get();
    while (c != EOF)
    {
        result.push_back(c);
        c = input.get();
    }
    
    return result;
}


bool compare(const std::string& a, const std::string& b)
{    
    if (a.size() != b.size())
    {
        std::cerr << "Expected size of " << a.size() << " but got " << b.size() << "." << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i]) 
        {
            std::cerr << "Difference at " << i << "(" << a.size() << "): "
                      << "expected " << a[i] << " but got " << b[i];
            return false;
        }
    }
    return true;
}

int main()
{
    std::string ref = read_file("README.md");
    std::string res((const char*)README_md, README_md_size);
    
    if (compare(ref, res))
    {
        std::cerr << "OK" << std::endl;
    }
    else
    {
        std::cerr << "NOT OK" << std::endl;
    }
}
