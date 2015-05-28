#!/bin/bash
clear
c++ -O2 -o 2 2.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
