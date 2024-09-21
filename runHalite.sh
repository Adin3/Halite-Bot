g++ -std=c++11 src/MyBot.cpp -o MyBot.o
g++ -std=c++11 src/RandomBot.cpp -o RandomBot.o
mv -v MyBot.o RandomBot.o bots/
./halite -d "$1 $2" -n 1 -s $3 "./bots/MyBot.o" "./bots/DBotv4_linux_x64"