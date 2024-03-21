#pragma once
#include<map>
#include "state.h"
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/graphics.h"

class Order;
class Node;
class Track;
class Signal;
class Train;
class Gamestate;

class Tracksystem
{
friend class Node;
friend class Track;
friend class Gamestate;
public:
    Tracksystem();
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
    Vec getpos(State state, float transverseoffset=0);
    Vec getsignalpos(signalid signal);
    Vec getswitchpos(nodeid node, bool updown);
    State getcloseststate(Vec pos);
    float getorientation(State state);
    float getradius(State state);
    State travel(State state, float pixels);
    float distancefromto(State state1, State state2, float maxdist, bool mustalign=false);
    bool isendofline(State state);
    signalid addsignal(State state);
    void setsignal(signalid signal, int redgreenorflip);
    bool getsignalstate(signalid signal);
    bool isred(Train* train);
    void setswitch(nodeid node, bool updown, int switchstate);
    int getswitchstate(nodeid node, bool updown);
    bool checkblocks(std::vector<nodeid> switchblocks, std::vector<signalid> signalblocks, Train* fortrain);
    bool claimblocks(std::vector<nodeid> switchblocks, std::vector<signalid> signalblocks, Train* fortrain);
    void runoverblocks(State state, float pixels, Train* fortrain);
    signalid nextsignalontrack(State state, bool startfromtrackend=false, bool mustalign=true);
    void setblocksuptonextsignal(Signal* fromsignal);
    Game* game;
    nodeid selectednode = 0;
    bool placingsignal = false;
private:
    nodeid addnode(Vec pos, float dir);
    trackid addtrack(nodeid leftnode, nodeid rightnode);
    void removenode(nodeid toremove);
    void removetrack(trackid toremove);
    void removesignal(signalid toremove);
    State whatdidiclick(Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch);
    float distancetotrack(trackid track, Vec pos);
    float distancetonode(nodeid node, Vec pos);
    float distancetosignal(signalid node, Vec pos);
    float distancetoswitch(nodeid node, Vec pos, bool updown);
    nodeid getclosestnode(Vec pos);
    signalid getclosestsignal(Vec pos);
    nodeid getclosestswitch(Vec pos, bool* updown);
    Node* getnode(nodeid node);
    float getnodedir(nodeid node);
    float getradiusoriginatingfromnode(nodeid node, trackid track);
    Vec getnodepos(nodeid node);
    Track* gettrack(trackid track);
    trackid nexttrack(trackid track);
    trackid previoustrack(trackid track);
    State tryincrementingtrack(State state);
    nodeid extendtracktopos(nodeid fromnode, Vec pos);
    void connecttwonodes(nodeid node1, nodeid node2);
    Signal* getsignal(signalid signal);
    std::map<nodeid, Node*> nodes;
    std::map<trackid, Track*> tracks;
    std::map<signalid, Signal*> signals;
    nodeid nodecounter = 0;
    trackid trackcounter = 0;
    signalid signalcounter = 0;
    bool preparingtrack = false;
};

class Node
{
friend class Tracksystem;
private:
    Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid);
    void connecttrack(trackid track, bool fromupordown);
    void render(Rendering* r);
    trackid gettrackup();
    trackid gettrackdown();
    Vec getswitchpos(bool updown);
    void incrementswitch(bool updown);
    Tracksystem* tracksystem;
    nodeid id;
    Vec pos;
    float dir;
    int stateup = 0;
    int statedown = 0;
    std::vector<trackid> tracksup;
    std::vector<trackid> tracksdown;
    Train* reservedfor = nullptr;
    Sprite sprite;
};

class Track
{
friend class Tracksystem;
private:
    Track(Tracksystem& newtracksystem, nodeid previous, nodeid next, trackid myid);
    ~Track();
    Tracksystem* tracksystem;
    nodeid previousnode, nextnode;
    void render(Rendering* r);
    Vec getpos(float nodedist);
    Vec getpos(float nodedist, float transverseoffset);
    State getcloseststate(Vec pos);
    float getarclength(float nodedist);
    float getorientation(float nodedist);
    bool isabovepreviousnode();
    bool isbelownextnode();
    trackid id;
    float phi;
    float radius;
    float previousdir;
    float nextdir;
    Vec previouspos;
    Vec nextpos;
    std::map<float,signalid> signals;
};

class Signal
{
friend class Tracksystem;
public:
    Signal(Tracksystem& newtracksystem, State signalstate);
    void render(Rendering* r);
    bool isred(Train* train);
    bool isgreen = true;
private:
    State state;
    Vec pos;
    Tracksystem* tracksystem;
    std::vector<nodeid> switchblocks;
    std::vector<signalid> signalblocks;
    Train* reservedfor = nullptr;
    Sprite sprite;
};
