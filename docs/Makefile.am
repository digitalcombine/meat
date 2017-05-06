#                                                           -*- Makefile.am -*-

dist_man1_MANS = meat.1 grinder.1
pkgdata_DATA = Meat-Language-Reference.odt

EXTRA_DIST = Meat-Standard.tex Class-Reference.tex Object-Reference.tex \
	Component-Reference.tex FDL.tex \
	logo.svg meat-logo.jpg
EXTRA_DIST += Meat-Language-Reference.odt

if LATEXMK
pkgdata_DATA +=  Meat-Standard.pdf

Meat-Standard.pdf: Class-Reference.tex Object-Reference.tex
Meat-Standard.pdf: Component-Reference.tex FDL.tex
Meat-Standard.pdf: Meat-Standard.tex
	latexmk -pdf Meat-Standard.tex
	latexmk -c
endif
