# Makefile for CS5237 Project, Linux.

INCLUDE_FOLDER = ../include
SOURCE_FOLDER = ../src

BASICS_INCLUDE = $(INCLUDE_FOLDER)/basics $(INCLUDE_FOLDER)/basicsP2 $(INCLUDE_FOLDER)/basicsP3 $(INCLUDE_FOLDER)/linux/basics
BASICS_INCLUDE_PARAMS=$(foreach d, $(BASICS_INCLUDE), -I$d)

BASICS_SOURCE = $(SOURCE_FOLDER)/basics/*.cpp $(SOURCE_FOLDER)/basicsP2/*.cpp $(SOURCE_FOLDER)/basicsP3/*.cpp $(SOURCE_FOLDER)/linux/basics/*.cpp

MAIN_INCLUDE = $(BASICS_INCLUDE) $(INCLUDE_FOLDER)
MAIN_INCLUDE_PARAMS=$(foreach d, $(MAIN_INCLUDE), -I$d)

MAIN_SOURCE = $(BASICS_SOURCE) $(SOURCE_FOLDER)/*.cpp


default:    main

main:   
	echo $(MAIN_INCLUDE_PARAMS) $(MAIN_SOURCE) 
	echo "BUILDING:"
	g++ $(MAIN_INCLUDE_PARAMS) $(MAIN_SOURCE) -lglut -lGL -lGLU


