// ezrc - eazy resource compiler
// Copyright (c) 2009 - 2025, Sean Farrell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <cctype>
#include <cstdlib>

#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <tinyformat.h>
#include <fkYAML/node.hpp>

constexpr auto VERSION = "0.4.0";

struct Options
{
    bool                                 show_version   = false;
    bool                                 show_help      = false;
    std::optional<std::filesystem::path> input_file;
    std::optional<std::filesystem::path> output_base;
};

void print_banner(std::ostream& out)
{
    tfm::format(out, "ezrc - easy resource compiler\n"
                     "Version: %s \n",
                     VERSION);
}

void print_help(std::ostream& out)
{
    tfm::format(out, "Usage:\n"
                     "  ezrc [Options] <Input Files>\n"
                     "\n"
                     "Options:\n"
                     "  --help    -h   This help message.\n"
                     "  --version -v   Display the program version.\n"
                     "  --output  -o   Base name for generated files (e.g. 'resources').\n"
                     "                 Will generate <base>.h and <base>.cpp\n");
}

Options parse_arguments(int argc, char* argv[])
{
    auto opt = Options{};

    for (int i = 1; i < argc; ++i)
    {
        auto arg = std::string_view{argv[i]};

        if (arg == "--version" || arg == "-v")
        {
            opt.show_version = true;
        }
        else if (arg == "--help" || arg == "-h")
        {
            opt.show_help = true;
        }
        else if (arg == "--output" || arg == "-o")
        {
            if (i + 1 >= argc)
            {
                throw std::runtime_error("Output base (--output) must have an argument.");
            }

            if (opt.output_base.has_value())
            {
                throw std::runtime_error("Output base can only be set once.");
            }

            auto param = std::string_view{argv[i + 1]};
            if (!param.empty() && param.front() != '-')
            {
                opt.output_base = std::filesystem::path{std::string(param)};
                ++i;
            }
            else
            {
                throw std::runtime_error("Output base (--output) argument is not valid.");
            }
        }
        else
        {
            if (!arg.empty() && arg.front() != '-')
            {
                if (opt.input_file.has_value())
                {
                    throw std::runtime_error("Only one input file can be set.");
                }

                opt.input_file = std::filesystem::path{std::string(arg)};
            }
            else
            {
                throw std::runtime_error(tfm::format("Unknown argument \"%s\".", arg));
            }
        }
    }

    if (opt.show_version || opt.show_help)
    {
        return opt;
    }

    if (opt.input_file.has_value() == false)
    {
        throw std::runtime_error("No input files specified.");
    }

    if (!opt.output_base)
    {
        auto stem = opt.input_file->stem().string();
        opt.output_base = opt.input_file->parent_path() / stem;
    }

    return opt;
}

struct ResourceYml
{
    std::string                        name_space = "ezrc";
    std::string                        api        = "";
    std::vector<std::filesystem::path> files;
};

ResourceYml read_resouces_yml(const std::filesystem::path& resouces_yml)
{
    auto input = std::ifstream(resouces_yml);
    if (!input)
    {
        throw std::runtime_error(tfm::format("Failed to open %s for reading.", resouces_yml));
    }

    auto base = resouces_yml.parent_path();

    auto yaml = fkyaml::node::deserialize(input);

    auto resouces = ResourceYml{};

    if (yaml.contains("namespace"))
    {
        resouces.name_space = yaml.at("namespace").get_value<std::string>();
    }

    if (yaml.contains("api"))
    {
        resouces.api = yaml.at("api").get_value<std::string>();
    }

    for (const auto& yfile : yaml.at("files"))
    {
        auto file = base / yfile.get_value<std::string>();
        resouces.files.push_back(std::filesystem::canonical(file));
    }

    return resouces;
}

std::string read_file(const std::filesystem::path& file)
{
    auto input = std::ifstream(file, std::ios::binary);
    if (!input)
    {
        throw std::runtime_error(tfm::format("Failed to open file %s for reading", file));
    }

    auto result = std::string{};
    input.seekg(0, std::ios::end);
    const auto size = input.tellg();
    if (size > 0)
    {
        result.resize(static_cast<std::size_t>(size));
        input.seekg(0, std::ios::beg);
        input.read(result.data(), size);
    }
    return result;
}

struct Resource
{
    std::filesystem::path path;
    std::string           var_name;
    std::string           data;
};

std::string normalize_name(const std::filesystem::path& path)
{
    auto name = path.filename().string();
    auto result = std::string{};
    result.reserve(name.size() + 1);

    if (!name.empty() && std::isdigit(static_cast<unsigned char>(name[0])))
    {
        result.push_back('_');
    }

    for (unsigned char ch : name)
    {
        if (std::isalnum(ch) || ch == '_')
        {
            result.push_back(static_cast<char>(ch));
        }
        else
        {
            result.push_back('_');
        }
    }

    return result;
}

std::vector<Resource> read_files(const std::vector<std::filesystem::path>& files)
{
    auto result = std::vector<Resource>{};
    result.reserve(files.size());
    for (const auto& file : files)
    {
        result.emplace_back(file, normalize_name(file), read_file(file));
    }
    return result;
}

std::string to_hex_literal(unsigned char c)
{
    constexpr char hex_digits[] = "0123456789abcdef";
    auto s = std::string(4, '\0');
    s[0] = '0';
    s[1] = 'x';
    s[2] = hex_digits[(c >> 4) & 0x0f];
    s[3] = hex_digits[c & 0x0f];
    return s;
}

void write_header(std::ostream& out, const ResourceYml& resoruce_yml)
{
    tfm::format(out, "// File generated with ezrc\n\n"
                     "#pragma once\n\n"
                     "#include <string_view>\n\n");

    tfm::format(out, "namespace %s \n"
                     "{\n\n", resoruce_yml.name_space);
    tfm::format(out, "    %s std::string_view get_string_resource(const std::string_view file);", resoruce_yml.api);
    tfm::format(out, "}\n");
}

void write_cpp(std::ostream& out, const ResourceYml& resoruce_yml, const std::vector<Resource>& resources)
{
    tfm::format(out, "// File generated with ezrc\n\n"
                     "#include <array>\n"
                     "#include <map>\n"
                     "#include <string_view>\n\n");

    tfm::format(out, "namespace %s \n"
                     "{\n\n", resoruce_yml.name_space);

    for (const auto& resource : resources)
    {
        tfm::format(out, "const auto %s_data = std::array<unsigned char, %d>{\n    ", resource.var_name, resource.data.size() + 1);

        std::size_t col = 0;
        for (unsigned char c : resource.data)
        {
            if (col == 12)
            {
                tfm::format(out, "%s,\n    ", to_hex_literal(c));
                col = 0;
            }
            else
            {
                tfm::format(out, "%s,", to_hex_literal(c));
                ++col;
            }
        }

        tfm::format(out, "0x00\n};\n");
    }

    tfm::format(out, "    const auto resource_table = std::map<std::string_view, std::string_view>{\n");
    for (const auto& resource : resources)
    {
        tfm::format(out, "        {%s, std::string_view(reinterpret_cast<const char*>(%s_data.data()), %s_data.size()-1)},\n", resource.path.filename(), resource.var_name, resource.var_name);
    }
    tfm::format(out, "    };\n\n");

    tfm::format(out, "    std::string_view get_string_resource(const std::string_view file)\n"
                     "    {\n"
                     "        return resource_table.at(file);\n"
                     "    }\n");

    tfm::format(out, "}\n");
}

int main(int argc, char* argv[])
{
    try
    {
        const auto opt = parse_arguments(argc, argv);

        if (opt.show_version)
        {
            print_banner(std::cout);
            return EXIT_SUCCESS;
        }

        if (opt.show_help)
        {
            print_banner(std::cout);
            tfm::format(std::cout, "\n");
            print_help(std::cout);
            return EXIT_SUCCESS;
        }

        auto resource_yml = read_resouces_yml(*opt.input_file);

        const auto resources = read_files(resource_yml.files);

        auto base = *opt.output_base;
        auto header_path = base;
        header_path += ".h";

        auto cpp_path = base;
        cpp_path += ".cpp";

        auto header_out = std::ofstream(header_path, std::ios::binary);
        if (!header_out)
        {
            throw std::runtime_error(tfm::format("Failed to open %s for writing.", header_path));
        }

        auto cpp_out = std::ofstream(cpp_path, std::ios::binary);
        if (!cpp_out)
        {
            throw std::runtime_error(tfm::format("Failed to open %s for writing.", cpp_path));
        }

        write_header(header_out, resource_yml);
        write_cpp(cpp_out, resource_yml, resources);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& ex)
    {
        tfm::format(std::cerr, "%s\n", ex.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        tfm::format(std::cerr, "Exit with unknown error.\n");
        return EXIT_FAILURE;
    }
}
