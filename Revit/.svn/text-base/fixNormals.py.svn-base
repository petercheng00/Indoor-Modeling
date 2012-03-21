import sys
from math import *

testPointMinDist = 0.01
testPointMaxDist = 1.0

walls = []
flats = []

class Plane:
    def __init__(self):
        self.eqn = []
        self.points = []
    
def readFile(fileName):
    inFile = open(fileName, "r")
    numPlanes = int(inFile.readline())
    counter = 1
    for i in xrange(numPlanes):
        currPlane = Plane()
        numPoints = int(inFile.readline())
        currPlane.eqn = [float(x) for x in inFile.readline().split()]
        for k in xrange(numPoints):
            currPoint = [float(x) for x in inFile.readline().split()]
            currPlane.points.append(currPoint)
        if counter >=1:
            if currPlane.eqn[2] == 1.0 or currPlane.eqn[2] == -1.0:
                if currPlane.eqn[2] == -1.0:
                    flipEqn(currPlane)
                flats.append(currPlane)
            else:
                walls.append(currPlane)
        counter += 1
    inFile.close()
    
def writeFile(fileName):
    outFile = open(fileName, "w")
    counter = 0
    outFile.write(str(len(walls) + len(flats)) + '\n')
    for plane in (flats+walls):
    #outFile.write(str(len(walls)) + '\n')
    #for plane in walls + flats:
        if counter > -1:
            outFile.write(str(len(plane.points)) + '\n')
            outFile.write(str(plane.eqn[0]) + " " + str(plane.eqn[1]) + " " + str(plane.eqn[2]) + " " + str(plane.eqn[3]) + '\n')
            for vertex in plane.points:
                outFile.write(str(vertex[0]) + " " + str(vertex[1]) + " " + str(vertex[2]) + '\n')
        counter += 1
    outFile.close()
    
def mag(v):
    return sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2))
    
def norm(v):
    m = mag(v)
    return [x / m for x in v]
    
def midPoint(a, b):
    return [(a[i]+b[i])/2.0 for i in xrange(3)]

def dist(a, b):
    return mag([abs(b[0]-a[0]), abs(b[1]-a[1]), abs(b[2]-a[2])])
    
def planePointDist(plane, point):
    eqn = norm(plane.eqn)
    return sum(eqn[i]*point[i] for i in xrange(2))
    
def closestDist(plane, point):
    minPoint1 = plane.points[0]
    minPoint2 = plane.points[1]
    minPointDist1 = dist(plane.points[0], point)
    minPointDist2 = dist(plane.points[1], point)
    for p in plane.points:
        candidate = dist(p, point)
        if candidate < minPointDist2:
            if candidate < minPointDist1:
                minPoint2 = minPoint1
                minPointDist2 = minPointDist1
                minPoint1 = p
                minPointDist1 = candidate
            else:
                minPoint2 = p
                minPointDist2 = candidate
    difference = 10
    while difference > 0.1:
        middle = midPoint(minPoint1, minPoint2)
        candidate = dist(middle, point)
        if candidate < minPointDist2:
            if candidate < minPointDist1:
                difference = abs(minPointDist1 - candidate)
                minPoint2 = minPoint1
                minPointDist2 = minPointDist1
                minPoint1 = p
                minPointDist1 = candidate
            else:
                difference = abs(minPointDist2 - candidate)
                minPoint2 = p
                minPointDist2 = candidate
        else:
            difference = 0
    return minPointDist1
    
def within(a, b, thresh):
    return abs(a-b) < thresh
	
def center(points):
    totalX = 0
    totalY = 0
    totalZ = 0
    for point in points:
        totalX += point[0]
        totalY += point[1]
        totalZ += point[2]
    return [ totalX/len(points), totalY/len(points), totalZ/len(points) ]
    
def areaAABBXY(points):
    minX = maxX = points[0][0]
    minY = maxY = points[0][1]
    for point in points:
        minX = min(minX, point[0])
        maxX = max(maxX, point[0])
        minY = min(minY, point[1])
        maxY = max(maxY, point[1])
    return (maxX - minX) * (maxY - minY)
    
def getClosestFlat(point):
    if (len(flats) == 0):
        assert False
    closestFlat = flats[0]
    closestFlatVal = closestDist(flats[0], point)
    for flat in flats:
        candidate = closestDist(flat, point)
        if candidate < closestFlatVal:
            closestFlatVal = candidate
            closestFlat = flat
    return closestFlat
    
def slopeXY(a, b):
    if (b[0] - a[0]) == 0:
        return float("inf")
    return (float)(b[1] - a[1])/(b[0] - a[0])
    
def intersects(point, end1, end2):
    top = end1
    bottom = end2
    if (top[1] < bottom[1]):
        top = end2
        bottom = end1
    if (top[1] < point[1] or bottom[1] > point[1]):
        return False
    edgeSlope = slopeXY(end1, end2)
    if (edgeSlope == 0):
        return False
    yDist = (point[1] - end1[1])
    xDist = yDist / edgeSlope
    return (end1[0] + xDist) > point[0]

    
def isInsideXY(point, plane):
    numIntersects = 0
    for i in xrange(len(plane)-1):
        if intersects(point, plane[i], plane[i+1]):
            numIntersects += 1
    if intersects(point, plane[len(plane)-1], plane[0]):
        numIntersects += 1
    return (numIntersects % 2) == 1
    
def isInsideZ(point, plane):
    minZ = plane[0][2]
    maxZ = plane[0][2]
    for p in plane:
        minZ = min(minZ, p[2])
        maxZ = max(maxZ, p[2])
    return (point[2] < maxZ and point[2] > minZ)
    
def getTestPoint(start, vect, dist, sign):
    vect = norm(vect)
    if sign == '+':
        return [ start[i] + (dist * vect[i]) for i in xrange(3) ]
    else:
        return [ start[i] - (dist * vect[i]) for i in xrange(3) ]

def flipEqn(plane):
    plane.eqn = [(0.0 if x == 0.0 else -1.0 * x) for x in plane.eqn]

def fixFlats():
    flipCount = 0
    for flat in flats:
        currCenter = center(flat.points)
        testPoint1 = getTestPoint(currCenter, flat.eqn, testPointMinDist, '+')
        testPoint2 = getTestPoint(currCenter, flat.eqn, testPointMinDist, '-')
        inside1 = 0
        inside2 = 0
        for wall in walls:
            if (isInsideZ(testPoint1, wall.points)):
                inside1 += 1
            if (isInsideZ(testPoint2, wall.points)):
                inside2 += 1
        if (inside2 > inside1):
            flipEqn(flat)
            flipCount += 1
    print "flipped flats: " + str(flipCount)
    
def fixWalls():
    flipCount = 0
    for wall in walls:
        currCenter = center(wall.points)
        testPoint1 = getTestPoint(currCenter, wall.eqn, testPointMinDist, '+')
        testPoint2 = getTestPoint(currCenter, wall.eqn, testPointMinDist, '-')
        inside1 = 0
        inside2 = 0
        for flat in flats:
            if (isInsideXY(testPoint1, flat.points)):
                inside1 += 1
            if (isInsideXY(testPoint2, flat.points)):
                inside2 += 1
        if (inside2 > inside1):
            flipEqn(wall)
            flipCount += 1
    print "flipped walls: " + str(flipCount)
	
if __name__ == "__main__":
    readFile(sys.argv[1])
    print "num walls read: ", len(walls)
    print "num flats read: ", len(flats)
    fixFlats()
    fixWalls()
    writeFile(sys.argv[2])
