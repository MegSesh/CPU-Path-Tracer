#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{
    look = Vector3f(0,0,-1);
    up = Vector3f(0,1,0);
    right = Vector3f(1,0,0);
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, Vector3f(0,0,10), Vector3f(0,0,0), Vector3f(0,1,0), 0.0, 1.0)
{}

Camera::Camera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp, const Float lensRadius, const Float focalDistance):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp),

    lensRadius(lensRadius),
    focalDistance(focalDistance),
    sampler(new Sampler(10, 0))       //doesn't matter what values you give sampler


{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H),


    lensRadius(c.lensRadius),
    focalDistance(c.focalDistance),
    sampler(new Sampler(10, 0))


{}

void Camera::CopyAttributes(const Camera &c)
{
    fovy = c.fovy;
    near_clip = c.near_clip;
    far_clip = c.far_clip;
    eye = c.eye;
    ref = c.ref;
    look = c.look;
    up = c.up;
    right = c.right;
    width = c.width;
    height = c.height;
    aspect = c.aspect;
    V = c.V;
    H = c.H;



    lensRadius = c.lensRadius;
    focalDistance = c.focalDistance;


}

void Camera::RecomputeAttributes()
{
    look = glm::normalize(ref - eye);
    right = glm::normalize(glm::cross(look, world_up));
    up = glm::cross(right, look);

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width/(float)height;
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::GetViewProj() const
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    deg = glm::radians(deg);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    deg = glm::radians(deg);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}

Ray Camera::Raycast(const Point2f &pt) const
{
    return Raycast(pt.x, pt.y);
}

Ray Camera::Raycast(float x, float y) const
{
    float ndc_x = (2.f * x / width - 1);
    float ndc_y = (1 - 2.f * y / height);
    return RaycastNDC(ndc_x, ndc_y);
}

Ray Camera::RaycastNDC(float ndc_x, float ndc_y) const
{
    glm::vec3 P = ref + ndc_x*H + ndc_y*V;
    Ray result(eye, glm::normalize(P - eye));
//    return result;


    //THIN LENS BASED CAMERA
//    glm::vec3 P = glm::vec3(ndc_x, ndc_y, 1.0f);//ndc_x * H + ndc_y * V;    //H and V bring it back to world space
//    Ray result(Point3f(0.0f), P);


//    //transform ray to camera space
//    //screen --> camera : multiply inverse Proj
//    glm::mat4 projMat = glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip);
//    glm::mat4 viewMat = glm::lookAt(eye, ref, up);

//    result.origin = glm::vec3(glm::inverse(projMat) * glm::vec4(result.origin, 1.0f));
//    result.direction = glm::vec3(glm::inverse(projMat) * glm::vec4(result.direction, 0.0f));


    //ERROR: the problem with doing this here is that you won't get a different noise value from pcg, you'll get the same one
//    //get random sample point
//    pcg32 rng;
//    Point2f randPoint = Point2f(rng.nextFloat(), rng.nextFloat());


    if(lensRadius > 0.0f)
    {
//        Point3f pLens = lensRadius * WarpFunctions::squareToDiskConcentric(sampler->Get2D());
//        Float ft = glm::abs(focalDistance / result.direction.z);
//        Point3f pFocus = ft * result.direction + result.origin;

//        result.origin = pLens;
//        result.direction = glm::normalize(pFocus - result.origin);

        Point3f pLens = lensRadius * WarpFunctions::squareToDiskConcentric(sampler->Get2D());
        Point3f pFocus = focalDistance * result.direction + result.origin;  //focalDistance is already in world space
        Point3f aperaturePoint = result.origin + (up * pLens.y) + (right * pLens.x);

        result.origin = aperaturePoint;
        result.direction = glm::normalize(pFocus - aperaturePoint);
    }

    //multiply by inverse of view matrix to bring back to world
//    glm::mat4 inverseViewMat = glm::inverse(glm::lookAt(eye, ref, up));
//    result.origin = glm::vec3(inverseViewMat * glm::vec4(result.origin, 1.0f));
//    result.direction = glm::normalize(glm::vec3(inverseViewMat * glm::vec4(result.direction, 0.0f)));

    return result;
}

void Camera::create()
{
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> col;
    std::vector<GLuint> idx;

    //0: Eye position
    pos.push_back(eye);
    //1 - 4: Near clip
        //Lower-left
        Ray r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * near_clip);
    //5 - 8: Far clip
        //Lower-left
        r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * far_clip);

    for(int i = 0; i < 9; i++){
        col.push_back(glm::vec3(1,1,1));
    }

    //Frustum lines
    idx.push_back(1);idx.push_back(5);
    idx.push_back(2);idx.push_back(6);
    idx.push_back(3);idx.push_back(7);
    idx.push_back(4);idx.push_back(8);
    //Near clip
    idx.push_back(1);idx.push_back(2);
    idx.push_back(2);idx.push_back(3);
    idx.push_back(3);idx.push_back(4);
    idx.push_back(4);idx.push_back(1);
    //Far clip
    idx.push_back(5);idx.push_back(6);
    idx.push_back(6);idx.push_back(7);
    idx.push_back(7);idx.push_back(8);
    idx.push_back(8);idx.push_back(5);

    //Camera axis
    pos.push_back(eye); col.push_back(glm::vec3(0,0,1)); idx.push_back(9);
    pos.push_back(eye + look); col.push_back(glm::vec3(0,0,1));idx.push_back(10);
    pos.push_back(eye); col.push_back(glm::vec3(1,0,0));idx.push_back(11);
    pos.push_back(eye + right); col.push_back(glm::vec3(1,0,0));idx.push_back(12);
    pos.push_back(eye); col.push_back(glm::vec3(0,1,0));idx.push_back(13);
    pos.push_back(eye + up); col.push_back(glm::vec3(0,1,0));idx.push_back(14);

    count = idx.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx.data(), count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(pos.data(), pos.size() * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(col.data(), col.size() * sizeof(glm::vec3));
}

GLenum Camera::drawMode() const {return GL_LINES;}






//TRY 1
//        result.origin = Point3f(GetViewProj() * glm::vec4(pLens, 0.0f));
//        result.direction = Point3f(GetViewProj() * glm::vec4(glm::normalize(pFocus - result.origin), 0.0f));

//TRY 2
//don't multiply with viewproj, just add eye to origin. direction will be the same, but don't multiply viewproj
//        result.origin = eye + pLens;
//        result.direction = glm::normalize(pFocus - result.origin);


//    else
//    {
//        result.origin = Point3f(GetViewProj() * glm::vec4(result.origin, 0.0f));
//        result.direction = Point3f(GetViewProj() * glm::vec4(result.direction, 0.0f));
//    }
