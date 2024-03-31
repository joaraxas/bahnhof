#pragma once
#include<map>
#include "state.h"
#include "bahnhof/common/math.h"

class Order;
class Train;
class Gamestate;

namespace Tracks
{
class Node;
class Track;
class Switch;
class Signal;
struct Trackblock;
struct Tracksection
{
    Tracksection() {};
    Tracksection(std::vector<Track*> t, std::vector<Node*> n) : tracks(t), nodes(n) {};
    std::vector<Track*> tracks;
    std::vector<Node*> nodes;
    std::unordered_map<Node*,State> tracksplits;
};

struct Tracksystem
{
public:
    Tracksystem(Game& whatgame, std::vector<float> xs, std::vector<float> ys);
    ~Tracksystem();
    Node* getnode(nodeid node);
    Track* gettrack(trackid track);
    Switch* getswitch(switchid _switch);
    Signal* getsignal(signalid signal);
    nodeid addnode(Node& node);
    trackid addtrack(Track& track);
    switchid addswitchtolist(Switch* _switch);
    signalid addsignal(State state);
    void removenode(nodeid toremove);
    void removetrack(trackid toremove);
    void removeswitchfromlist(switchid toremove);
    void removesignal(signalid toremove);
    std::vector<Track*> alltracks();
    std::vector<Node*> allnodes();
    std::vector<Switch*> allswitches();
    std::vector<Signal*> allsignals();
    Game* game;
private:
    std::map<nodeid, Node*> nodes;
    std::map<trackid, Track*> tracks;
    std::map<switchid, Switch*> switches;
    std::map<signalid, Signal*> signals;
    nodeid nodecounter = 0;
    trackid trackcounter = 0;
    switchid switchcounter = 0;
    signalid signalcounter = 0;
};

namespace Input
{
    signalid buildsignalat(Tracksystem& tracksystem, Vec pos);
    Tracksection buildat(Tracksystem& tracksystem, Node* fromnode, Vec pos);
    Tracksection planconstructionto(Tracksystem& tracksystem, Node* fromnode, Vec pos);
    nodeid selectat(Tracksystem& tracksystem, Vec pos);
    bool switchat(Tracksystem& tracksystem, Vec pos);
    Order* generateorderat(Tracksystem& tracksystem, Vec pos);
    float getcostoftracks(Tracksection);
};

namespace Signaling
{
    void update(Tracksystem& tracksystem, int ms);
    bool isred(Tracksystem& tracksystem, Train* train);
    Trackblock getblocksuptonextsignal(Tracksystem& tracksystem, State state);
    void runoverblocks(Tracksystem& tracksystem, State state, float pixels, Train* fortrain);
    bool checkblocks(Tracksystem&, Trackblock blocks, Train* fortrain);
    bool claimblocks(Tracksystem&, Trackblock blocks, Train* fortrain);
};

namespace Construction
{
    Tracksection extendtracktopos(Tracksystem& tracksystem, Node* fromnode, Vec pos);
    Tracksection connecttwonodes(Tracksystem& tracksystem, Node* node1, Node* node2);
    void splittrack(Tracksystem&, Node* node, State state);
};

    void render(Tracksystem&, Rendering* r);
    void render(Tracksection section, Rendering* r, int mode=0);
    void renderabovetrains(Tracksystem&, Rendering* r);

    State travel(Tracksystem&, State state, float pixels);
    float distancefromto(Tracksystem&, State state1, State state2, float maxdist, bool mustalign=false);
    bool isendofline(Tracksystem&, State state);

    Vec getpos(Tracksystem&, State state, float transverseoffset=0);
    float getradius(Tracksystem&, State state);
    float getorientation(Tracksystem&, State state);

    Vec getswitchpos(Tracksystem&, switchid _switch);
    Vec getsignalpos(Tracksystem&, signalid signal);
    void setsignal(Tracksystem&, signalid signal, int redgreenorflip);
    void setswitch(Tracksystem&, switchid _switch, int switchstate);
};