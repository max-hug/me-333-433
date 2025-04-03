#include<stdio.h>
#include<stdbool.h>
#include<math.h>
#define NUM_POINTS 2

struct Point{
	double x;
	double y;
};


double findDist(struct Point*, int, int);
bool getInput(struct Point*, int);

int main (){
	struct Point points[2];

	int validPoints = 0;

	while(validPoints < NUM_POINTS){
		if(getInput(points, validPoints)){
            validPoints++;
        }
	}

    double dist = findDist(points, 0, 1);

    printf("The distance between the points is %f", dist);

	return 0;
}

double findDist(struct Point* points, int i, int j){
    double dist = sqrt(pow(points[i].x - points[j].x,2) + pow(points[i].y - points[j].y,2));
    return dist;
}

bool getInput(struct Point* points, int i){
    printf("Input point coordinates in the form x y: ");
    scanf("%lf %lf", &(points[i].x), &(points[i].y));
    return true;
}