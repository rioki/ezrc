//
// qglrc - libqgl's resource compiler
// Copyright (c) 2009, Sean Farrell
// 
// This file is part of qglrc.
// 
// qglrc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// qglrc is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with qglrc.  If not, see <http://www.gnu.org/licenses/>.
// 

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

bool do_print_version = false;
bool do_print_help = false;
std::vector<std::string> input_files;
std::vector<std::string> input_data;
std::string output_file;

//------------------------------------------------------------------------------
void print_banner(std::ostream& os)
{
    os << "qglrc - libqgl's resource compiler\n"
          "Copyright (c) 2009 Sean Farrell \n"
          "Version: " VERSION "\n";
}

//------------------------------------------------------------------------------
void print_help(std::ostream& os)
{
    os << "Usage:\n"
          "  qglrc [Options] <Input Files>\n"
          "  \n"
          "Options:\n"
          "  --help -h       This help message.\n"
          "  --version -v    Display the program version.\n"
          "  --output -o     The file to output.\n";
}

//------------------------------------------------------------------------------
bool is_alpha_num(const std::string& value)
{
    for (unsigned int i = 0; i < value.size(); i++)
    {
        if (! (std::isalnum(value[i]) || value[i] == '_'))
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
std::string get_base_name(const std::string& path)
{
    size_t pos = path.find_last_of(".");
    if (pos == std::string::npos)
        return path;
    else
        return path.substr(0, pos);
}

//------------------------------------------------------------------------------
void decode_arguments(int argc, char* argv[])
{
    for (unsigned int i = 1; i < static_cast<unsigned int>(argc); i++)
    {
        std::string arg = argv[i];
        
        if ((arg == "--version") || (arg == "-v"))
        {
            do_print_version = true;
        }        
        else if ((arg == "--help") || (arg == "-h"))
        {
            do_print_help = true;
        }
        else if ((arg == "--output") || (arg == "-o"))
        {
            if (i + 1 < argc)
            {
                std::string param = argv[i + 1];
                if (param[0] != '-')
                {
                    output_file = param;
                    i++;
                }
                else
                {
                    std::stringstream msg;
                    msg << "Output file (--output) \"" << param << "\" is not a valid file.\n"
                        << "\n";
                    print_help(msg);
                    throw std::runtime_error(msg.str());
                }
            }
            else
            {
                std::stringstream msg;
                msg << "Output file (--output) must have an argument.\n"
                    << "\n";
                print_help(msg);
                throw std::runtime_error(msg.str());
            }
        }
        else
        {
            if (arg[0] != '-')
            {
                input_files.push_back(arg);
            }
            else
            {
                std::stringstream msg;
                msg << "Unknown argument \"" << arg << "\".\n"
                    << "\n";
                print_help(msg);
                throw std::runtime_error(msg.str());            
            }
        }
    }
    
    if (do_print_version || do_print_help)
        return;
    
    // Validate:
    if (input_files.empty())
    {
        throw std::runtime_error("No imput files specified.");            
    }
    
    if (output_file.empty() && input_files.size() == 1)
    {
        output_file = get_base_name(input_files[0]) + ".cpp";
    }
    if (output_file.empty())
    {
        throw std::runtime_error("No output file specified.");
    }
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
std::vector<std::string> read_files(const std::vector<std::string>& files)
{
    std::vector<std::string> result;
    for (unsigned int i = 0; i < files.size(); i++)
    {
        result.push_back(read_file(files[i]));
    }
    return result;
}

//------------------------------------------------------------------------------
std::string normalize_name(const std::string& name)
{
    std::string result(name.size(), '_');
    
    for (unsigned int i = 0; i < name.size(); i++)
    {
        if (std::isalnum(name[i]))
            result[i] = name[i];
    }
    
    return result;
}

//------------------------------------------------------------------------------
char to_hex(char c)
{
    switch (c)
    {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        case 10:
            return 'a';
        case 11:
            return 'b';
        case 12:
            return 'c';
        case 13:
            return 'd';
        case 14:
            return 'e';
        case 15:
            return 'f';
        default:
            assert(false && "not a hex char");
            return 0;
    }
}

//------------------------------------------------------------------------------
std::string to_hex_char(char c)
{
    char low_nibble = c & 0x0f;
    char heigh_nibble = (c & 0xf0) >> 4;
    
    
    return std::string("\\x") + to_hex(heigh_nibble) + to_hex(low_nibble);
}

//------------------------------------------------------------------------------
void write_data(std::ostream& output, const std::string& file, const std::string& data)
{
    std::string variable_name = normalize_name(file);
    output << "// " << file << ": " << data.size() << " bytes\n";
    output << "std::string " << variable_name << " = \"";
    for (unsigned int i = 0; i < data.size(); i++)
    {
        if (i % 20 == 0)
            output << "\"\n    \"";
        output << to_hex_char(data[i]);
        
    }
    output << "\";\n";
            
}

//------------------------------------------------------------------------------
void write_data(std::ostream& output, const std::vector<std::string>& files, const std::vector<std::string>& data)
{
    assert(files.size() == data.size());
    
    for (unsigned int i = 0; i < input_files.size(); i++)
    {
        output << "\n";
        write_data(output, files[i], data[i]);
    }
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    try
    {
        decode_arguments(argc, argv);
    
        if (do_print_version)
        {
            print_banner(std::cerr);
            return 0;
        }
        
        if (do_print_help)
        {
            print_banner(std::cerr);
            std::cerr << "\n";
            print_help(std::cerr);            
            return 0;
        }                
        
        input_data = read_files(input_files);
        
        std::ofstream output(output_file.c_str());
        if (output.bad())
        {
            std::stringstream msg;
            msg << "Failed to open " << output_file << " for writing.\n";
            throw std::runtime_error(msg.str());
        }            
                
        output << "// File generated with Iced Resource\n"
               << "\n"
               << "#include <string>\n"
               << "\n";
        
        write_data(output, input_files, input_data);
    
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
        return -1;
    }
    catch (...)
    {
        std::cerr << "Exit with unknown error.\n";
        return -1;
    }
}
