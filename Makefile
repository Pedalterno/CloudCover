SHELL = /bin/bash
PKGNAME =Cloud Cover
VERS = 1.0
FACADESRCDIR = facade-src
TESTSRCDIR = test-src
PRGSRCDIR = clcv-src
INCLUDEDIR = include

all : facade tests program docs

facade :
			cd $(FACADESRCDIR) && $(MAKE)

tests :  facade
			cd $(TESTSRCDIR) && $(MAKE)

program : facade tests
			cd $(PRGSRCDIR) && $(MAKE)

docs :
			doxygen Doxyfile
			sed -f intro.txt doc/html/index.html > tmp
			mv -f tmp doc/html/index.html

clean :
				find . -name ".*~"  -exec rm -f {} \;
				find . -name "*~"  -exec rm -f {} \;
				rm -rf facade-obj test-bin bin
				rm -rf doc
