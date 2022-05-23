#include "minirt/minirt.h"
#pragma once
using namespace minirt;

struct Point
{
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
};

class RetraceJob
{
private:
    Scene &scene;
    Image &image;
    ViewPlane &viewPlane;
    Point cur_point;
    size_t numOfSamples;

public:
    RetraceJob(Scene &scene, Image &image, ViewPlane &viewPlane, Point &curPoint, size_t numOfSamples);

    void Execute() override;
};

void PixelTask(const Scene &scene, Image &image, ViewPlane &viewPlane, Point point, size_t numOfSamples)
{

    const auto color = viewPlane.computePixel(scene, point.x, point.y, numOfSamples);
    image.set(point.x, point.y, color);
}

void RetraceJob::Execute()
{
    PixelTask(scene, image, viewPlane, cur_point, numOfSamples);
}

RetraceJob::RetraceJob(Scene &scene, Image &image, ViewPlane &viewPlane, Point &curPoint, size_t numOfSamples)
    : scene(scene), viewPlane(viewPlane), image(image), cur_point(curPoint), numOfSamples(numOfSamples) {}
