#pragma once
#include<map>
#include "state.h"
#include "bahnhof/common/math.h"

class Order;
class Node;
class Track;
class Switch;
class Signal;
class Train;
class Gamestate;
struct Trackblock;

class Tracksystem
{
public:
    Tracksystem(Game* whatgame, std::vector<float> xs, std::vector<float> ys);
    ~Tracksystem();
    void update(int ms);
    void render(Rendering* r);
    void renderabovetrains(Rendering* r);
    void deleteclick(int xMouse, int yMouse);
    void selectat(Vec pos);
    float buildat(Vec pos);
    bool switchat(Vec pos);
    Order* generateorderat(Vec pos);
    State travel(State state, float pixels);
    float distancefromto(State state1, State state2, float maxdist, bool mustalign=false);
    bool isendofline(State state);
    bool isred(Train* train);
    Trackblock getblocksuptonextsignal(State state);
    Game* game;
    nodeid selectednode = 0;
    bool placingsignal = false;
    Node* getnode(nodeid node);
    Track* gettrack(trackid track);
    Switch* getswitch(switchid _switch);
    Signal* getsignal(signalid signal);
    switchid addswitchtolist(Switch* _switch);

    Vec getpos(State state, float transverseoffset=0);
    Vec getsignalpos(signalid signal);
    float getradius(State state);
    float getorientation(State state);
    void setsignal(signalid signal, int redgreenorflip);
    void setswitch(switchid _switch, int switchstate);
    Vec getswitchpos(switchid _switch);
    bool checkblocks(Trackblock blocks, Train* fortrain);
    bool claimblocks(Trackblock blocks, Train* fortrain);
    void runoverblocks(State state, float pixels, Train* fortrain);
private:
    nodeid addnode(Vec pos, float dir);
    trackid addtrack(nodeid leftnode, nodeid rightnode);
    signalid addsignal(State state);
    void removenode(nodeid toremove);
    void removetrack(trackid toremove);
    void removesignal(signalid toremove);
    State whatdidiclick(Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch);
    State getcloseststate(Vec pos);
    nodeid getclosestnode(Vec pos);
    signalid getclosestsignal(Vec pos);
    nodeid getclosestswitch(Vec pos);
    trackid nexttrack(trackid track);
    trackid previoustrack(trackid track);
    State tryincrementingtrack(State state);
    nodeid extendtracktopos(nodeid fromnode, Vec pos);
    void connecttwonodes(nodeid node1, nodeid node2);
    std::map<nodeid, Node*> nodes;
    std::map<trackid, Track*> tracks;
    std::map<switchid, Switch*> switches;
    std::map<signalid, Signal*> signals;
    nodeid nodecounter = 0;
    trackid trackcounter = 0;
    switchid switchcounter = 0;
    signalid signalcounter = 0;
    bool preparingtrack = false;
};

Vec getswitchpos(Vec nodepos, float nodedir, bool updown);
float distancebetween(Vec, Vec);