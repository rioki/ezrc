
erzc
====
        
About
-----

ezrc is a command line utility that packs data as C source code.    
This approach to data handling provides a robust and portable solution when 
data is closely bound to source code. 

Usage
-----

    ezrc [options] <files> 
    
### Options
    
* --help -h     Display a brief help and usage message.
* --version -v  Display the program version and exit.
* --output -o   Specify the output file to write. If only one input file is 
                given the base name is appended with .c. If multiple input
                files are specified ezrc will quit with an error.

Example Usage
-------------

### A Single File

    ezrc SunIcon.jpg

This will read SunIcon.jpg and create the file SunIcon.c. In this file 
there will be a unsigned char SunIcon_jpg[] with the contents and a
unsigned int SunIcon_jpg_size with the size in bytes of contents.

To use the resource use a simple extern declararion like so:

    extern unsigned char SunIcon_jpg[];
    extern unsinged int SunIcon_jpg_size;
    
### Multiple Files

    ezrc Toon.vert Toon.frag ToonGradient.png -o resources.c

This will read the data from Toon.vert, Toon.frag and ToonGradient.png
and write the file resources.c. The file will contain the variables 
Toon_vert, Toon_frag and ToonGradient_png with the respective data.

Building
--------

All you need to build this package is a make and a C++ compiler. just invoke 
make and it should build all you need.
    
Errors, Bugs and Suggestions
----------------------------

Please report any errors, bugs and suggestions to:

https://github.com/rioki/ezrc

License 
-------

ezrc is distibuted under the MIT License. 
