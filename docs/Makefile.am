#                                                           -*- Makefile.am -*-

dist_man1_MANS = meat.1 meat-grinder.1
pkgdata_DATA = Meat-Language-Reference.odt

LATEX_SRCS = Meat-Standard.tex Component-Reference.tex \
	Object-Reference.tex Class-Reference.tex \
	Context-Reference.tex BlockContext-Reference.tex \
	Null-Reference.tex Exception-Reference.tex \
	Boolean-Reference.tex Numeric-Reference.tex \
	Integer-Reference.tex Number-Reference.tex \
	Text-Reference.tex List-Reference.tex \
	Set-Reference.tex Index-Reference.tex \
	FDL.tex

EXTRA_DIST = $(LATEX_SRCS) \
	logo.svg meat-logo.jpg
EXTRA_DIST += Meat-Language-Reference.odt

if LATEXMK
pkgdata_DATA +=  Meat-Standard.pdf

Meat-Standard.pdf: $(LATEX_SRCS)
Meat-Standard.pdf: Meat-Standard.tex
	latexmk -pdf Meat-Standard.tex
	latexmk -c
endif
