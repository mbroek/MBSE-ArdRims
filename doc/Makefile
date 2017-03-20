# Makefile for the MBSE ArdRims documentation.
# Copyright (c) 2016-2017 by M. Broek.


SRC		= ardrims-ch1.sgml ardrims-ch2.sgml ardrims-ch3.sgml \
		  ardrims-ch4.sgml ardrims-ch5.sgml mbse-ardrims.sgml 
HTML		= mbse-ardrims.html
PDF		= mbse-ardrims.pdf
OTHER		= Makefile docbook-utils.dsl hoofdmenu.png pid.png \
		  pid-kp.png pid-ki.png pid-kd.png pid-sampletime.png \
		  pid-offset.png pid-heatinboil.png pid-hlttemp.png \
		  unit-boiltemp.png unit-pidpipe.png unit-pumpmashin.png \
		  unit-pumponboil.png unit-pumprest.png unit-skipadd.png \
	      	  unit-skipremove.png unit-iodinetime.png unit-pumpcyclus.png \
		  unit-pumpmashout.png unit-pumponmash.png unit-pumpstop.png \
		  unit-skipiodine.png unit.png mash-mashin.png mash-mashout-2.png \
		  mash-step1-2.png mash-step2-2.png mash-step3-1.png \
		  mash-mashout-1.png mash-step1-1.png mash-step2-1.png \
		  mash-step2-3.png mash.png boil-boiltime.png boil-cooltemp.png \
		  boil-hop1.png boil-hop2.png boil-nrofhops.png boil-whirl2.png \
		  boil-whirl6.png boil-whirl7.png boil-whirl9.png boil.png \
		  recipe-erase.png recipe-init-2.png recipe-load-2.png \
		  recipe-save-1.png recipe-save-3.png recipe.png recipe-init-1.png \
		  recipe-load-1.png recipe-load-3.png recipe-save-2.png \
		  recipe-saveas-1.png handbediening-1.png handbediening-3.png \
	     	  handbediening-hlt-2.png handbediening-mlt-2.png \
		  handbediening-pump-2.png handbediening-2.png \
		  handbediening-hlt-1.png handbediening-mlt-1.png \
		  handbediening-pump-1.png auto-1.png auto-klaar-1.png \
		  auto-koelen-3.png auto-mashout-1.png auto-startkoelen.png \
		  auto-step2-2.png auto-whirlpool-3.png auto-2.png \
	    	  auto-klaar-2.png auto-koken-1.png auto-opwarmen.png \
		  auto-step1-1.png auto-storten.png auto-whirlpool-4.png \
		  auto-hop1.png auto-koelen-1.png auto-koken-2.png auto-prime.png \
	     	  auto-step1-2.png auto-whirlpool-1.png auto-iodine.png \
		  auto-koelen-2.png auto-maischin.png auto-remove.png \
	      	  auto-step2-1.png auto-whirlpool-2.png


#############################################################################

.c.o:
		${CC} ${CFLAGS} ${INCLUDES} ${DEFINES} -c $<

all:		pdf

mbse-ardrims.pdf:	${SRC} docbook-utils.dsl
		docbook2pdf -d docbook-utils.dsl#print mbse-ardrims.sgml

mbse-ardrims.html:	${SRC} docbook-utils.dsl
		docbook2html -d docbook-utils.dsl#html -u mbse-ardrims.sgml

pdf:		${PDF}

html:		${HTML}

clean:
		rm -f *.pdf *.html

install:	all

depend:

