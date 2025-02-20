#pragma once
#include<map>
#include "state.h"
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/sprite.h"

class Rendering;
class Train;
class Game;

namespace Tracks
{
class Node;
class Track;
class Switch;
class Signal;
struct Tracksystem;
struct Tracksection;

class Node
{
friend class Switch;
public:
    Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid);
    void connecttrack(Track* track, bool fromupordown);
    void disconnecttrack(Track* track, bool fromupordown);
    void render(Rendering* r);
    Vec getpos();
    float getdir();
    bool hasswitch();
    Track* trackup = nullptr;
    Track* trackdown = nullptr;
    Train* reservedfor = nullptr;
    nodeid id;
private:
    Tracksystem* tracksystem;
    std::unique_ptr<Switch> switchup = nullptr;
    std::unique_ptr<Switch> switchdown = nullptr;
    Vec pos;
    float dir;
};

Vec getswitchpos(Vec nodepos, float nodedir, bool updown);

class Switch
{
friend class Node;
public:
    Switch(Node* node, Track* track, bool updown);
    ~Switch();
    void render(Rendering* r);
    void setswitch(int newstate);
    Vec pos();
    int getstateforordergeneration();
    switchid id;
private:
    void addtrack(Track* track);
    void removetrack(Track* track);
    Tracksystem* tracksystem;
    Node* node;
    bool updown;
    int switchstate = 0;
    std::vector<Track*> tracks;
    Icon sprite;
};

float getradiusoriginatingfromnode(Track* track, nodeid node);

class Track
{
public:
    Track(Tracksystem& newtracksystem, Node& previous, Node& next, trackid myid);
    ~Track();
    void initnodes();
    void render(Rendering* r, int mode);
    Vec getpos(float nodedist, float transverseoffset=0);
    State getcloseststate(Vec pos);
    float getarclength(float nodedist);
    float getorientation(float nodedist);
    float getradius(State state);
    Track* nexttrack();
    Track* previoustrack();
    void split(Track& track1, Track& track2, State where);
    State getsplitstate(Track& track1, Track& track2, State where, State oldstate);
    void connectsignal(State signalstate, signalid signal);
    void disconnectsignal(signalid signal);
    signalid nextsignal(State state, bool startfromtrackend=false, bool mustalign=true);
    Node* previousnode;
    Node* nextnode;
    trackid id;
private:
    bool isabovepreviousnode();
    bool isbelownextnode();
    Tracksystem* tracksystem;
    float phi;
    float radius;
    std::map<float,signalid> signals;
};

struct Trackblock
{
    std::vector<nodeid> switchblocks;
    std::vector<signalid> signalblocks;
};

class Signal
{
public:
    Signal(Tracksystem& newtracksystem, State signalstate, signalid myid);
    void addtotrack();
    void disconnectfromtrack();
    void render(Rendering* r);
    void update();
    void set(int redgreenorflip);
    int getcolorforordergeneration();
    Vec pos();
    bool isred(Train* train);
    State state;
    Train* reservedfor = nullptr;
    signalid id;
private:
    Tracksystem* tracksystem;
    bool isgreen = true;
    Trackblock blocks;
    Icon sprite;
};

    State tryincrementingtrack(Tracksystem&, State state);
    float distancebetween(Vec, Vec);

namespace Input
{
    State whatdidiclick(Tracksystem& tracksystem, Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch);
    State getcloseststate(Tracksystem& tracksystem, Vec pos);
    nodeid getclosestnode(Tracksystem& tracksystem, Vec pos);
    signalid getclosestsignal(Tracksystem& tracksystem, Vec pos);
    nodeid getclosestswitch(Tracksystem& tracksystem, Vec pos);
}

}