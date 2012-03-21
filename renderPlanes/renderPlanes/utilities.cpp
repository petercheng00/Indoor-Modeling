#include "utilities.h"

bool sameSide( Vec2 p1, Vec2 p2, Vec2 a, Vec2 b ) {
	Vec3 p1_3d = Vec3(p1[0], p1[1], 0);
	Vec3 p2_3d = Vec3(p2[0], p2[1], 0);
	Vec3 a_3d = Vec3(a[0], a[1], 0);
	Vec3 b_3d = Vec3(b[0], b[1], 0);

	Vec3 cp1 = ( b_3d - a_3d ) ^ ( p1_3d - a_3d );
	Vec3 cp2 = ( b_3d - a_3d ) ^ ( p2_3d - a_3d );
	return ( cp1 * cp2 >= 0 );
}

bool vertexInsidePolygon( Vec2 testPoint, Vec2Array* vertices ){
	int counter = 0;
	Vec2 p1 = vertices->front();
	for ( int i = 1; i <= vertices->size(); i++ ) {
		Vec2 p2 = vertices->at( i % vertices->size() );
		if ( testPoint[1] > min(p1[1], p2[1]) &&
			 testPoint[1] <= max(p1[1], p2[1]) &&
			 testPoint[0] <= max(p1[0], p2[0]) &&
			 p1[1] != p2[1]) {
				float xintercept = (testPoint[1]-p1[1])*(p2[0]-p1[0])/(p2[1]-p1[1])+p1[0];
				if (p1[0] == p2[0] || testPoint[0] <= xintercept) {
					counter++;
				}
		}
		p1 = p2;
	}
	return (counter % 2 == 1);
}

bool verticesInTriangle ( int a, int b, int c, Vec2Array* vertices, vector<int>& valid ) {
	for ( int i = 0; i < valid.size(); i++ ) {
		if ( valid[i] == a || valid[i] == b || valid[i] == c ) continue;
		if ( sameSide(vertices->at( valid[i] ), vertices->at(a), vertices->at(b), vertices->at(c)) &&
		     sameSide(vertices->at( valid[i] ), vertices->at(b), vertices->at(a), vertices->at(c)) &&
			 sameSide(vertices->at( valid[i] ), vertices->at(c), vertices->at(a), vertices->at(c)) ) {
				 return true;
		}
	}
	return false;
}
