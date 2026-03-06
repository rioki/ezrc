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

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>
#include <tinyformat.h>

#include <freeimage.h>

#include "resources.h"

std::string read_file(const std::filesystem::path& file)
{
    std::string result;

    std::ifstream input(file, std::ios::binary);
    if (input.bad())
    {
        throw std::runtime_error(tfm::format("Failed to open %s for reading.", file));
    }

    int c = input.get();
    while (c != EOF)
    {
        result.push_back(c);
        c = input.get();
    }

    return result;
}


TEST(ezrc, math_glsl)
{
    auto reference = read_file(std::filesystem::path(PROJECT_DIR) / "math.glsl");
    auto result    = test::get_string_resource("math.glsl");
    EXPECT_EQ(reference, result);
}

void free_image_trace(FREE_IMAGE_FORMAT, const char *message)
{
    std::cerr << message << std::endl;
}

TEST(ezrc, lena_png)
{
    FreeImage_Initialise();
    FreeImage_SetOutputMessage(free_image_trace);

    auto data = test::get_string_resource("lena.png");

    auto stream = FreeImage_OpenMemory(reinterpret_cast<BYTE*>(const_cast<char*>(data.data())), static_cast<DWORD>(data.size()));
    auto bitmap = FreeImage_LoadFromMemory(FIF_PNG, stream, JPEG_ACCURATE);
    FreeImage_CloseMemory(stream);

    ASSERT_NE(bitmap, nullptr);

    EXPECT_EQ(FreeImage_GetWidth(bitmap), 250u);
    EXPECT_EQ(FreeImage_GetHeight(bitmap), 250u);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}