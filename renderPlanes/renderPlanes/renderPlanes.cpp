#include "utilities.h"

string modelFile = "";
string mapFile = "";

int findEar( Vec2Array* vertices, vector<int>& validVertices ) {
	float minPerim = -1;
	int minIndex = -1;
	for ( int i = 0; i < validVertices.size(); i++ ) {
		int prevIndex = i - 1;
		int candidateIndex = i;
		int nextIndex = i + 1;
		if ( prevIndex == -1 ){
			prevIndex = validVertices.size() - 1;
		}
		if ( nextIndex == validVertices.size() ){
			nextIndex = 0;
		}
		prevIndex = validVertices[prevIndex];
		candidateIndex = validVertices[candidateIndex];
		nextIndex = validVertices[nextIndex];
		Vec2 prev = vertices->at( prevIndex );
		Vec2 candidate = vertices->at( candidateIndex );
		Vec2 next = vertices->at( nextIndex );

		float currPerim = (next -  candidate).length() + (candidate - prev).length() + (prev - next).length();
		if (minIndex == -1 || currPerim < minPerim ) {
			minPerim = currPerim;
			minIndex = i;
		}
		if ( verticesInTriangle( prevIndex, candidateIndex, nextIndex, vertices, validVertices) ) {
			continue;
		}
		Vec2 testPoint = (candidate + prev + next)/3;
		if ( !vertexInsidePolygon( testPoint, vertices ) ) {
			continue;
		}
		return i;
	}
	return minIndex;
}

void doEarClipping( Geometry* planeGeometry, Vec2Array* planeVertices ) {
	//list of all unclipped vertices
	vector<int> remainingVertices;
	for ( int i = 0; i < planeVertices->size(); i++ ){
		remainingVertices.push_back( i );
	}
	while(remainingVertices.size() > 0) {
		DrawElementsUInt* currTriangle = 
			new DrawElementsUInt( PrimitiveSet::TRIANGLES, 0 );
		if ( remainingVertices.size() == 3 ) {
			currTriangle->push_back( remainingVertices[0] );
			currTriangle->push_back( remainingVertices[1] );
			currTriangle->push_back( remainingVertices[2] );
			remainingVertices.clear();
		}
		else {
			int currEarIndex = findEar( planeVertices, remainingVertices );
			if ( currEarIndex == 0 ){
				currTriangle->push_back( remainingVertices.back() );
			}
			else {
				currTriangle->push_back( remainingVertices[(currEarIndex-1)] );
			}
			currTriangle->push_back( remainingVertices[currEarIndex] );
			if ( currEarIndex == remainingVertices.size() - 1 ) {
				currTriangle->push_back( remainingVertices.front() );
			}
			else {
				currTriangle->push_back( remainingVertices[(currEarIndex+1)] );
			}
			remainingVertices.erase( remainingVertices.begin() + currEarIndex );
		}
		planeGeometry-> addPrimitiveSet( currTriangle );
	}
}


Vec2Array* convert3dTo2d( Vec3Array* vertices ) {
	Vec2Array* vertices_2d = new Vec2Array();
	for (int i = 0; i < vertices->size(); i++ ) {
		vertices_2d->push_back(Vec2(vertices->at(i)[0], vertices->at(i)[1]));
	}
	return vertices_2d;
}

void addLighting(Group* root) {
	ref_ptr<LightSource> lightSource = new LightSource;
	Light* light = lightSource->getLight();
	light->setLightNum(0);
	light->setPosition(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	light->setAmbient(osg::Vec4(0.7f, 0.7f,0.7f,1.0f));
	light->setDiffuse(osg::Vec4(0.0f, 0.0f,0.0f,1.0f));
	light->setSpecular(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	root->addChild(lightSource);
	StateSet* statelightON_OVRD = root->getOrCreateStateSet();
	statelightON_OVRD->setAttribute(light, StateAttribute::ON);
}

void applyTextures(Group* root, vector<string>& fileVect, vector<Vec2Array*>& coordVect) {
		/*osg::Vec4Array* colors = new osg::Vec4Array;
		if (i == 29){
			colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else{
			colors->push_back(osg::Vec4(i/(float)numPlanes, i/(float)numPlanes, i/(float)numPlanes, 1.0f) ); 
		}
		planeGeometry->setColorArray(colors);
		planeGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);*/
	for (int i = 0; i < fileVect.size(); i ++ ){
		if (fileVect[i] == ""){
			continue;
		}
		Geode* currGeode = (Geode*)root->getChild(i);
		Geometry* currGeometry = (Geometry*)currGeode->getDrawable(0);
		Vec2Array* currVectArray = coordVect[i];
		osg::Vec2Array* texcoords = new osg::Vec2Array(currVectArray->size());
		for (int j = 0; j < currVectArray->size(); j ++ ){
			(*texcoords)[j].set(currVectArray->at(j)[0], currVectArray->at(j)[1]);
		}
		currGeometry->setTexCoordArray(0,texcoords);

		osg::Texture2D* currTexture = new osg::Texture2D;
		currTexture->setDataVariance(osg::Object::DYNAMIC);
		osg::Image* currFace = osgDB::readImageFile(fileVect[i]);
		currTexture->setImage(currFace);
		osg::StateSet* state = new osg::StateSet;
		state->setTextureAttributeAndModes(0, currTexture,osg::StateAttribute::ON);
		currGeode->setStateSet(state);
	}
}


void parseMapFile(string mapFile, vector<string>& fileVect, vector<Vec2Array*>& coordVect) {
	ifstream inFile(mapFile);
	if(!inFile.is_open()){cerr << "error opening .map file" << endl;;}
	int numPlanes;
	inFile >> numPlanes;
	int i = 0;
	string currToken;
	double currX, currY;
	string imageFile;
	while (i < numPlanes) {
		inFile >> currToken;
		if (currToken == "SKIP_TO") {
			int target = numPlanes;
			inFile >> currToken;
			if (currToken != "END"){
				target = atoi(currToken.c_str());
			}
			while(i < target){
				fileVect.push_back("");
				coordVect.push_back(NULL);
				++i;
			}
		}
		else {
			int numCoords = atoi(currToken.c_str());
			inFile >> imageFile;
			fileVect.push_back(imageFile);
			Vec2Array* currCoords = new Vec2Array();
			for (int j = 0; j < numCoords; j++ ){
				inFile >> currX;
				inFile >> currY;
				currCoords->push_back(Vec2(currX, currY));
			}
			coordVect.push_back(currCoords);
			++i;
		}
	}


}
void parseModelFile(Group* root) {
	ifstream inFile(modelFile); 
	if(!inFile.is_open()){cerr << "error opening file specifying planes" << endl;;}
	int numPlanes;
	inFile >> numPlanes; 
	//iterate over each plane
	for(int i = 0; i!=numPlanes; ++i) {

		Geode* planeGeode = new Geode();
		Geometry* planeGeometry = new Geometry();
		planeGeode->addDrawable( planeGeometry );
		root->addChild( planeGeode );
		Vec3Array* planeVertices = new Vec3Array;
		//read in the number of delimiting points for a plane
		int numDelimitingPoints; 
		inFile >> numDelimitingPoints; 

		//read in the plane equation, which is comprised of the normal vector (nx,ny,nz) and d such that nx*x+ny*y+nz*z + d = 0
		double nx,ny,nz,d; 
		inFile >> nx >> ny >> nz >> d; 
			
		for(int j = 0; j!= numDelimitingPoints; ++j) {
			double vx,vy,vz; 
			inFile >> vx >> vy >> vz; 
			planeVertices->push_back( Vec3( 1000*vx, 1000*vy, 1000*vz) ); 
		}

		planeGeometry->setVertexArray( planeVertices );



		//dotransformation to make horizontal
		Vec3 normal = ((planeVertices->at(1) - planeVertices->at(0)) ^ (planeVertices->at(2) - planeVertices->at(1)));
		Matrix rotateMat = Matrix();
		rotateMat.makeRotate(normal, Vec3(0, 0, 1));
		for ( int j = 0; j < planeVertices->size() ; j++ ){
			planeVertices->at(j) = rotateMat * planeVertices->at(j);
		}
		float zVal = planeVertices->at(0)[2];
		Vec2Array* planeVertices2d = convert3dTo2d( planeVertices );

		doEarClipping( planeGeometry, planeVertices2d );

		Matrix unRotateMat = Matrix();
		unRotateMat.makeRotate(Vec3(0, 0, 1), normal);
		for ( int j = 0; j < planeVertices2d->size() ; j++ ){
			planeVertices->at(j)[0] = planeVertices2d->at(j)[0];
			planeVertices->at(j)[1] = planeVertices2d->at(j)[1];
			planeVertices->at(j)[2] = zVal;
			planeVertices->at(j) = unRotateMat * planeVertices->at(j);
		}
	
	}
	inFile.close(); 
}

int main(int argc, char** argv)
{
	if (argc > 1){
		ifstream inputFile(argv[1]);
		string fileName;
		getline(inputFile, fileName);
		while(inputFile){
			size_t l = fileName.find_last_of("."); 
			string extension = fileName.substr(l+1);
			if(extension == "model") {
				modelFile = fileName;
				cout << "Setting model file to be:" << endl;
				cout << fileName << endl;
			}
			else if (extension == "map") {
				mapFile = fileName;
				cout << "Setting map file to be:" << endl;
				cout << fileName << endl;
			}
			getline(inputFile, fileName);
		}
	}
    osgViewer::Viewer viewer;
    Group* root = new Group();
    vector<string> planeToImageFile;
    vector<Vec2Array*> planeToImageCoords;
    parseMapFile(mapFile, planeToImageFile, planeToImageCoords);
    parseModelFile(root);
    applyTextures(root, planeToImageFile, planeToImageCoords);
	addLighting(root);
 
    viewer.setSceneData( root );
    //viewer.run();
         
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();
 
    while( !viewer.done() )
    {
       viewer.frame();
    } 
	 
    return 0;
}
