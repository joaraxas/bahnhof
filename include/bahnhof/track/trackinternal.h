#pragma once
#include<map>
#include "state.h"
#include "tracksdisplaymode.h"
#include "bahnhof/common/math.h"
#include "bahnhof/common/shape.h"
#include "bahnhof/common/orientation.h"
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
    Node(Tracksystem& newtracksystem, Vec posstart, Tangent dirstart, nodeid myid);
    void connecttrack(Track* track, bool fromupordown);
    void disconnecttrack(Track* track, bool fromupordown);
    void render(Rendering* r);
    Vec getpos();
    Tangent getdir();
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
    Tangent dir;
    inline static Sprite sprite;
};

Vec getswitchpos(Vec nodepos, Tangent nodedir, bool pointsupwards);

class Switch
{
friend class Node;
public:
    Switch(Node* node, Track* track, bool isupordown);
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
    bool pointsupwards;
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
    void connecttonodes();
    void disconnectfromnodes();
    void render(Rendering* r, TracksDisplayMode mode);
    void renderballast(Rendering* r, TracksDisplayMode mode);
    Vec getpos(float nodedist, float transverseoffset=0);
    State getcloseststate(Vec pos);
    float getarclength(float nodedist);
    Angle getorientation(float nodedist);
    float getradius(State state);
    Track* nexttrack();
    Track* previoustrack();
    void split(Track& track1, Track& track2, State where);
    State getnewstateaftersplit(Track& track1, Track& track2, State where, State oldstate);
    void connectsignal(State signalstate, signalid signal);
    void disconnectsignal(signalid signal);
    signalid nextsignal(State state, bool startfromtrackend=false, bool mustalign=true);
    std::unique_ptr<Shape> getcollisionmask();
    Node* previousnode;
    Node* nextnode;
    trackid id;
private:
    bool isabovepreviousnode();
    bool isbelownextnode();
    bool aboveprev;
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
    Vec getpos() {return pos;};
    void addtotrack();
    void disconnectfromtrack();
    void render(Rendering* r);
    void update();
    void set(int redgreenorflip);
    int getcolorforordergeneration();
    bool isred(Train* train);
    State state;
    Train* reservedfor = nullptr;
    signalid id;
private:
    Tracksystem* tracksystem;
    Vec pos;
    bool isgreen = true;
    Trackblock blocks;
    Icon sprite;
};

Vec getsignalposfromstate(Tracksystem&, State state);

State tryincrementingtrack(Tracksystem&, State state);
float distancebetween(Vec, Vec);

namespace Input
{
    State whatdidiclick(Tracksystem& tracksystem, Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch);
    State getcloseststate(Tracksystem& tracksystem, Vec pos, float mindist=INFINITY);
    nodeid getclosestnode(Tracksystem& tracksystem, Vec pos);
    signalid getclosestsignal(Tracksystem& tracksystem, Vec pos);
    nodeid getclosestswitch(Tracksystem& tracksystem, Vec pos);
}

namespace Construction
{
    Tracksection extendtracktopos(Tracksystem& tracksystem, Vec frompos, Vec pos);
    Tracksection extendtracktopos(Tracksystem& tracksystem, Node* fromnode, Vec pos);
    Tracksection connecttwonodes(Tracksystem& tracksystem, Node* node1, Node* node2);
    void splittrack(Tracksystem&, Node* node, State state);
    Tangent gettangentofcurvestartingfromnode(Node& startnode, Vec curvetopos);
};

}