#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"

#define EMPTY_ID 0
unsigned char MY_ID;

#define _3PI4_ 2.35
#define _PI4_ 0.77

hlt::GameMap presentMap;
std::ofstream f("log.out");

// componenete universale
std::vector<std::vector<int>> productionMap;
std::vector<hlt::Location> ownedLocation;
hlt::Location currentLocation;

enum MatchState {
    EarlyGame,
    MidGame,
    LateGame
};
MatchState matchState = MatchState::EarlyGame;

struct NextLocation {
    hlt::Location location;
    hlt::Site site;
    NextLocation(hlt::Location loc, hlt::Site sit) {
        location = loc;
        site = sit;
    }
};
std::vector<NextLocation> nextLocation;
std::vector<NextLocation> nextEnemyLocation;

struct Tile {
    int direction; /// Vezi define-urile din hlt.hpp
    int points; /// cu cat mai multe cu atat sansa mai mare sa fie ales
    //TileStatus status;
    //TileState state;
    hlt::Site site;
    Tile(hlt::Site site, int direction) {
        this->site = site;
        this->direction = direction;
        this->points = 0;
    }
};

// Sa verifice daca iese de pe mapa
// pentru nu stiu ce motiv, functia asta e mai rapida decat getLocation din hlt.hpp :/
hlt::Site verifySite(int b, int a) {
    if (b < 0) b = presentMap.width - 1;
    if (b > presentMap.width - 1) b = 0;
    if (a < 0) a = presentMap.height - 1;
    if (a > presentMap.height - 1) a = 0;
    
    return presentMap.contents[a][b];
}

// functie pentru sortarea directilor
bool checkBestTile(Tile t1, Tile t2) 
{ 
    if (t1.points == t2.points) {
        float ratio_1 =  (float) t1.site.production / t1.site.strength;
        float ratio_2 = (float) t2.site.production / t2.site.strength;
        return ratio_1 < ratio_2;
    }
    return t1.points < t2.points;
}

// traduce unghi -> directie
// todo: sa te mai uiti peste functie, pare sa fie ceva putred la mijloc
int directionAttack(float angle) {
    if (-_3PI4_ <= angle && angle <= -_PI4_) return NORTH;
    else if (_3PI4_ >= angle &&  _PI4_ <= angle) return SOUTH;
    else if (-_PI4_ <= angle && angle <= _PI4_) return EAST;
    else return WEST;
}

// verifica daca este block interior
bool checkIfInner(std::vector<Tile>& tiles) {
    int collect = 0;
    for (int i = 1; i <= 4; i++) {
        if (tiles[i].site.owner == MY_ID) {
            collect++;
            tiles[i].points--;
        }
    }

    return collect == 4;
}

int directionTaken(std::vector<Tile>& tiles) {

    if (checkIfInner(tiles)) {
        if (tiles[STILL].site.strength <= 25) return STILL;
        else {
            int minimum = 99999;
            hlt::Location minLoc;
            if (nextLocation.empty()) return STILL;
            for (auto& c: nextLocation) {
                int dist = presentMap.getDistance(currentLocation, c.location);
                if (minimum > dist) {
                    minimum = dist;
                    minLoc = c.location;
                }
            }
            return directionAttack(presentMap.getAngle(currentLocation, minLoc));
        }
    } else {
        std::sort(tiles.begin() + 1, tiles.end(), checkBestTile);
        if (tiles[STILL].site.strength > tiles[4].site.strength) return tiles[4].direction;
        else return STILL;
    }
}

int directionTakenMid(std::vector<Tile> tiles) {
    
    if (checkIfInner(tiles)) {
        if (nextLocation.empty()) return STILL;

        int minimum = 99999;
        hlt::Location minLoc;
        for (auto& c: nextEnemyLocation) {
            int dist = presentMap.getDistance(currentLocation, c.location);
            if (minimum > dist) {
                minimum = dist;
                minLoc = c.location;
            }
        }
        int thres = 25;
        if (tiles[STILL].site.production > 5) thres = 15;
        if (tiles[STILL].site.strength <= thres) return STILL;
        return directionAttack(presentMap.getAngle(currentLocation, minLoc));
    } else {
        if (nextEnemyLocation.empty()) return STILL;

        int minimum = 99999;
        hlt::Location minLoc;
        for (auto& c: nextEnemyLocation) {
            int dist = presentMap.getDistance(currentLocation, c.location);
            if (minimum > dist) {
                minimum = dist;
                minLoc = c.location;
            }
        }
        // f << minimum << " " << currentLocation.x << " " << currentLocation.y << "\n";
        if (minimum <= 2 && tiles[STILL].site.strength != 0) 
            return directionAttack(presentMap.getAngle(currentLocation, minLoc));

        std::sort(tiles.begin() + 1, tiles.end(), checkBestTile);
        if (tiles[STILL].site.strength > tiles[4].site.strength)
            return tiles[4].direction;
        else 
            return STILL;
    }
}

void earlyGame(std::set<hlt::Move>& moves) {
    for (auto& owned : ownedLocation) {
        short unsigned int x = owned.x, y = owned.y;
        Tile currentTile = Tile(presentMap.contents[y][x], STILL);
        
        if (currentTile.site.owner == MY_ID) {
            std::vector<Tile> neighbourTiles {
                currentTile,
                Tile(verifySite(x, y - 1), NORTH),
                Tile(verifySite(x + 1, y), EAST),
                Tile(verifySite(x, y + 1), SOUTH),
                Tile(verifySite(x - 1, y), WEST),
            };
            currentLocation = owned;
            moves.insert({ { x, y }, (unsigned char)(directionTaken(neighbourTiles)) });
        }
    }
}

void midGame(std::set<hlt::Move>& moves) {
    for (auto& owned : ownedLocation) {
        short unsigned int x = owned.x, y = owned.y;
        Tile currentTile = Tile(presentMap.contents[y][x], STILL);
        
        if (currentTile.site.owner == MY_ID) {
            std::vector<Tile> neighbourTiles {
                currentTile,
                Tile(verifySite(x, y - 1), NORTH),
                Tile(verifySite(x + 1, y), EAST),
                Tile(verifySite(x, y + 1), SOUTH),
                Tile(verifySite(x - 1, y), WEST),
            };
            currentLocation = owned;
            moves.insert({ { x, y }, (unsigned char)(directionTakenMid(neighbourTiles)) });
        }
    }
}

void beforeMoving() {
    for (int i = 0; i < presentMap.height; i++) {
        for (int j = 0; j < presentMap.width; j++) {
            int x = j, y = i;
            if (presentMap.contents[i][j].owner == MY_ID) {
                hlt::Location l; l.x = x; l.y = y;
                ownedLocation.push_back(l);
                continue;
            }
            std::vector<Tile> neighbourTiles {
                Tile(verifySite(x, y - 1), NORTH),
                Tile(verifySite(x + 1, y), EAST),
                Tile(verifySite(x, y + 1), SOUTH),
                Tile(verifySite(x - 1, y), WEST),
            };
            bool neigh = false;
            bool enm = false;
            for (auto& c : neighbourTiles) {
                if (c.site.owner == MY_ID) {
                    neigh = true;
                }
                if (c.site.owner > MY_ID) {
                    enm = true;
                }
            }
            if (neigh) {
                if (enm) {
                    //f << "inamic gasit\n";
                    matchState = MatchState::MidGame;
                }
                hlt::Location l; l.x = x; l.y = y;
                NextLocation next(l, presentMap.contents[i][j]);
                nextLocation.push_back(next);
            }
            neigh = false;
            for (auto& c : neighbourTiles) {
                if (c.site.owner == EMPTY_ID) {
                    neigh = true;
                }
            }
            if (neigh && presentMap.contents[i][j].owner > MY_ID) {
                f << x << " " << y << "\n";
                hlt::Location l; l.x = x; l.y = y;
                NextLocation next(l, presentMap.contents[i][j]);
                nextEnemyLocation.push_back(next);
            }
        }
    }
}

void startingCalculations() {
    for (int i = 0; i < presentMap.height; i++) {
        std::vector<int> row;
        for (int j = 0; j < presentMap.width; j++) {
            row.push_back(presentMap.contents[i][j].production);
        }
        productionMap.push_back(row);
    }
}

int main() {
    srand(time(NULL));

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    getInit(myID, presentMap);
    sendInit("Gandacelul");
    MY_ID = myID;
    startingCalculations();

    std::set<hlt::Move> moves;
    int round = 0;
    while(true) {
        moves.clear();
        nextLocation.clear();
        nextEnemyLocation.clear();
        ownedLocation.clear();
        getFrame(presentMap);
        f << ++round << "\n";

        beforeMoving();

        switch (matchState) {
            case MatchState::EarlyGame:
                earlyGame(moves);
                break;
            case MatchState::MidGame:
                midGame(moves);
                break;
            case MatchState::LateGame:
                // todo
                break;
        }

        sendFrame(moves);
        // f.flush();
    }
    return 0;
}