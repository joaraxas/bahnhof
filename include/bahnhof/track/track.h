#pragma once
#include<map>
#include "state.h"
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/sprite.h"

class Order;
class Node;
class Track;
class Switch;
class Signal;
class Train;
class Gamestate;

class Tracksystem
{
friend class Node;
friend class Track;
friend class Switch;
friend class Gamestate;
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
    Vec getpos(State state, float transverseoffset=0);
    Vec getsignalpos(signalid signal);
    State getcloseststate(Vec pos);
    float getorientation(State state);
    float getradius(State state);
    State travel(State state, float pixels);
    float distancefromto(State state1, State state2, float maxdist, bool mustalign=false);
    bool isendofline(State state);
    void setsignal(signalid signal, int redgreenorflip);
    bool getsignalstate(signalid signal);
    bool isred(Train* train);
    void setswitch(switchid _switch, int switchstate);
    Vec getswitchpos(switchid _switch);
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
    switchid addswitchtolist(Switch* _switch);
    signalid addsignal(State state);
    void removenode(nodeid toremove);
    void removetrack(trackid toremove);
    void removesignal(signalid toremove);
    State whatdidiclick(Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch);
    float distancetotrack(trackid track, Vec pos);
    float distancetonode(nodeid node, Vec pos);
    float distancetosignal(signalid node, Vec pos);
    float distancetoswitch(nodeid node, Vec pos);
    nodeid getclosestnode(Vec pos);
    signalid getclosestsignal(Vec pos);
    nodeid getclosestswitch(Vec pos);
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
    Switch* getswitch(switchid _switch);
    int getswitchstate(switchid _switch);
    Signal* getsignal(signalid signal);
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

class Node
{
friend class Tracksystem;
friend class Switch;
private:
    Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid);
    void connecttrack(trackid track, bool fromupordown);
    void render(Rendering* r);
    Tracksystem* tracksystem;
    trackid trackup = 0;
    trackid trackdown = 0;
    std::unique_ptr<Switch> switchup = nullptr;
    std::unique_ptr<Switch> switchdown = nullptr;
    Train* reservedfor = nullptr;
    nodeid id;
    Vec pos;
    float dir;
};

Vec getswitchpos(Vec nodepos, float nodedir, bool updown);

class Switch
{
friend class Tracksystem;
friend class Node;
private:
    Switch(Node* node, trackid track, bool updown);
    void addtrack(trackid track);
    void setswitch(int newstate);
    void render(Rendering* r);
    Vec pos();
    Tracksystem* tracksystem;
    Node* node;
    bool updown;
    switchid id;
    int switchstate = 0;
    std::vector<trackid> tracks;
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
private:
    Signal(Tracksystem& newtracksystem, State signalstate);
    void render(Rendering* r);
    bool isred(Train* train);
    bool isgreen = true;
    State state;
    Vec pos;
    Tracksystem* tracksystem;
    std::vector<nodeid> switchblocks;
    std::vector<signalid> signalblocks;
    Train* reservedfor = nullptr;
    Sprite sprite;
};
