    
    qglrc
    Version 0.1.0
    
    
1. About

    qglrc is a command line utility that packs data as C++ source code.    
    This aproach to data handling provides a robust and portable solution when 
    data is closly bound to source code. 

2. Usage

    qglrc [options] <files> 
    
2.1 Options
    
    --output -o Specify the output file to write. If only one input file is 
                given the base name is appended with .cpp. If multiple input
                files are specified qglrc will quit with an error.
    
3. Example Usage

3.1 A Single File

        qglrc SunIcon.jpg
    
    This will read SunIcon.jpg and create the file SunIcon.cpp. In this file 
    there will be a std::string SunIcon_jpg with the contents.
    
    To use the string use a simple extern declararion like so:
    
        extern std::string SunIcon_jpg;
    
3.2 Multiple Files

        qglrc Toon.vert Toon.frag ToonGradient.png -o resources.cpp
    
    This will read the date from Toon.vert, Toon.frag and ToonGradient.png
    and write the file resources.cpp. The file will contain the variables 
    Toon_vert, Toon_frag and ToonGradient_png with the respective data.

4. Building

    The pacakge comes bundled with a Code::Blocks project file. It can either
    be built with that or simply compiling the main.cpp file with any C++ 
    compiler should head the expected result.
    
5. Errors, Bugs and Suggestions

    Please report any errors, bugs and suggestions to:
    
    http://www.iced-liquid.com/pages/contact.php

6. License 

qglrc is distibuted under the GNU General Public License version 3. See COPYING.txt for details.
