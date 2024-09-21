#!/bin/bash

g++ -std=c++11 src/MyBot.cpp -o MyBot.o
g++ -std=c++11 src/RandomBot.cpp -o RandomBot.o
mv -v MyBot.o RandomBot.o bots/
python run.py --cmd "bots/MyBot.o" --round 0 --visualizer "firefox"
