
erzc
====
        
About
-----

ezrc is a command line utility that packs data as C++ source code.    
This aproach to data handling provides a robust and portable solution when 
data is closly bound to source code. 

Usage
-----

    ezrc [options] <files> 
    
### Options
    
* --help -h     Display a brief help and usage message.
* --version -v  Display the program version and exit.
* --output -o   Specify the output file to write. If only one input file is 
                given the base name is appended with .cpp. If multiple input
                files are specified ezrc will quit with an error.

Example Usage
-------------

### A Single File

    ezrc SunIcon.jpg

This will read SunIcon.jpg and create the file SunIcon.cpp. In this file 
there will be a std::string SunIcon_jpg with the contents.

To use the string use a simple extern declararion like so:

    extern std::string SunIcon_jpg;
    
### Multiple Files

    ezrc Toon.vert Toon.frag ToonGradient.png -o resources.cpp

This will read the date from Toon.vert, Toon.frag and ToonGradient.png
and write the file resources.cpp. The file will contain the variables 
Toon_vert, Toon_frag and ToonGradient_png with the respective data.

Building
--------

All you need to build this package is a make and a C++ compiler. just invoke 
make and it should build all you need. For convinience the package also comes
with a CodeLite project. Finally, since the package consists of only one source
file you can just compilte tha.
    
Errors, Bugs and Suggestions
----------------------------

Please report any errors, bugs and suggestions to:

https://github.com/rioki/ezrc

License 
-------

ezrc is distibuted under the GNU General Public License version 3. 
See COPYING.txt for details.
