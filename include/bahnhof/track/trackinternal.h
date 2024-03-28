#pragma once
#include<map>
#include "state.h"
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/sprite.h"

class Rendering;
class Train;

class Tracksystem;
class Node;
class Track;
class Switch;
class Signal;

class Node
{
friend class Switch;
public:
    Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid);
    void connecttrack(Track* track, bool fromupordown);
    void render(Rendering* r);
    Vec getpos();
    float getdir();
    bool hasswitch();
    trackid trackup = 0;
    trackid trackdown = 0;
    Train* reservedfor = nullptr;
    nodeid id;
private:
    Tracksystem* tracksystem;
    std::unique_ptr<Switch> switchup = nullptr;
    std::unique_ptr<Switch> switchdown = nullptr;
    Vec pos;
    float dir;
};

class Switch
{
friend class Node;
public:
    Switch(Node* node, Track* track, bool updown);
    void render(Rendering* r);
    void setswitch(int newstate);
    Vec pos();
    int getstateforordergeneration();
    switchid id;
private:
    void addtrack(Track* track);
    Tracksystem* tracksystem;
    Node* node;
    bool updown;
    int switchstate = 0;
    std::vector<trackid> tracks;
    Sprite sprite;
};

float getradiusoriginatingfromnode(Track* track, nodeid node);

class Track
{
public:
    Track(Tracksystem& newtracksystem, Node& previous, Node& next, trackid myid);
    void render(Rendering* r, int mode);
    Vec getpos(float nodedist);
    Vec getpos(float nodedist, float transverseoffset);
    State getcloseststate(Vec pos);
    float getarclength(float nodedist);
    float getorientation(float nodedist);
    float getradius(State state);
    trackid nexttrack();
    trackid previoustrack();
    void addsignal(State signalstate, signalid signal);
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
    Sprite sprite;
};
