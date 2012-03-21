#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile> 
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>


using namespace std;
using namespace osg;

bool sameSide( Vec2 p1, Vec2 p2, Vec2 a, Vec2 b );
bool vertexInsidePolygon( Vec2 testPoint, Vec2Array* vertices );
bool verticesInTriangle( int a, int b, int c, Vec2Array* vertices, vector<int>& valid );