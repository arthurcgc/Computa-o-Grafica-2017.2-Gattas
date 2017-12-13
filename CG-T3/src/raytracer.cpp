// renderwidget.cpp

#include <QImage>
#include <QGLWidget>
#include <QMouseEvent>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include "renderwidget.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

static bool Intesect(glm::vec3 &r1,glm::vec3 &r2,
              glm::vec3 &pl1,glm::vec3 &pl2,glm::vec3 &pl3,glm::vec3 &pl4,
              float *t,glm::vec3 *itr )
{
    glm::vec3 r=r2-r1;
    r=glm::normalize(r);
    glm::vec3 v12=pl2-pl1;
    glm::vec3 v14=pl4-pl1;
    glm::vec3 n=glm::cross(v12,v14);    // face plane is: nx*x+ny*y+nz*z+d=0
    n=glm::normalize(n);
    float prodesc=glm::dot(n,r);
    if(prodesc>-0.000001){      // evita backfaces e plano paralelo a reta
        return false;
    }
    float d=-glm::dot(n,pl1);
    *t=-(d+glm::dot(n,r1))/prodesc;
    *itr=r1+(*t)*r;
    
    return true;
}

static bool PtInsideFace(glm::vec3 &pt,glm::vec3 &pl1,glm::vec3 &pl2,glm::vec3 &pl3,glm::vec3 &pl4)
{
    float xmin=pl1.x,xmax=pl1.x,ymin=pl1.y,ymax=pl1.y,zmin=pl1.z,zmax=pl1.z;
    if (pl2.x<xmin) xmin=pl2.x; if (pl2.x>xmax) xmax=pl2.x;
    if (pl2.y<ymin) ymin=pl2.y; if (pl2.y>ymax) ymax=pl2.y;
    if (pl2.z<zmin) zmin=pl2.z; if (pl2.z>zmax) zmax=pl2.z;
    
    if (pl3.x<xmin) xmin=pl3.x; if (pl3.x>xmax) xmax=pl3.x;
    if (pl3.y<ymin) ymin=pl3.y; if (pl3.y>ymax) ymax=pl3.y;
    if (pl3.z<zmin) zmin=pl3.z; if (pl3.z>zmax) zmax=pl3.z;
    
    if (pl4.x<xmin) xmin=pl4.x; if (pl4.x>xmax) xmax=pl4.x;
    if (pl4.y<ymin) ymin=pl4.y; if (pl4.y>ymax) ymax=pl4.y;
    if (pl4.z<zmin) zmin=pl4.z; if (pl4.z>zmax) zmax=pl4.z;
    
    glm::vec3 v12=pl2-pl1;
    glm::vec3 v14=pl4-pl1;
    glm::vec3 n=glm::cross(v12,v14);    // face plane is: nx*x+ny*y+nz*z+d=0
    n=glm::normalize(n);
    
    if (fabs(n.z-1.0)<0.0001)  // plano paralelo ao XY
    {
        if(pt.x<xmin) return false;
        if(pt.x>xmax) return false;
        if(pt.y<ymin) return false;
        if(pt.y>ymax) return false;
    }
    else if(fabs(n.y-1.0)<0.0001)
    {
        if(pt.x<xmin) return false;
        if(pt.x>xmax) return false;
        if(pt.z<zmin) return false;
        if(pt.z>zmax) return false;
    }
    else{
        if(pt.y<ymin) return false;
        if(pt.y>ymax) return false;
        if(pt.z<zmin) return false;
        if(pt.z>zmax) return false;
    }
    return true;
}


static void GetCube(float x,float y,float z,float size,glm::vec3 vtx[8],int face[6][4])
{
    vtx[0].x=x-0.5*size;    vtx[0].y=y-0.5*size;    vtx[0].z=z-0.5*size;
    vtx[1].x=x+0.5*size;    vtx[1].y=y-0.5*size;    vtx[1].z=z-0.5*size;
    vtx[2].x=x+0.5*size;    vtx[2].y=y-0.5*size;    vtx[2].z=z+0.5*size;
    vtx[3].x=x-0.5*size;    vtx[3].y=y-0.5*size;    vtx[3].z=z+0.5*size;

    vtx[4].x=x-0.5*size;    vtx[4].y=y+0.5*size;    vtx[4].z=z-0.5*size;
    vtx[5].x=x+0.5*size;    vtx[5].y=y+0.5*size;    vtx[5].z=z-0.5*size;
    vtx[6].x=x+0.5*size;    vtx[6].y=y+0.5*size;    vtx[6].z=z+0.5*size;
    vtx[7].x=x-0.5*size;    vtx[7].y=y+0.5*size;    vtx[7].z=z+0.5*size;

    face[0][0]=0;face[0][1]=1;face[0][2]=2;face[0][3]=3;
    face[1][0]=0;face[1][1]=4;face[1][2]=5;face[1][3]=1;
    face[2][0]=0;face[2][1]=3;face[2][2]=7;face[2][3]=4;
    face[3][0]=3;face[3][1]=2;face[3][2]=6;face[3][3]=7;
    face[4][0]=1;face[4][1]=5;face[4][2]=6;face[4][3]=2;
    face[5][0]=4;face[5][1]=7;face[5][2]=6;face[5][3]=5;

}

static bool IntersectObjs(glm::vec3 &p1,glm::vec3 &p2,glm::vec3 &itr)
{
    float t=1000;
    for( float x = -3; x <= 3; x+=3 )
    {
        for( float z = -3; z <= 3; z+=3)
        {
            glm::vec3 vtx[8];
            int face[6][4];
            GetCube(x,0.0f,z,2.0f,vtx,face);
            for(int i=0;i<6;i++)
            {
               float tlocal;
               glm::vec3 itrlocal;
               bool ii = Intesect(p1,p2,vtx[face[i][0]],vtx[face[i][1]],vtx[face[i][2]],vtx[face[i][3]],&tlocal,&itrlocal);
               if(ii==true)
               {
                  if(PtInsideFace(itr,vtx[face[i][0]],vtx[face[i][1]],vtx[face[i][2]],vtx[face[i][3]])==true)
                  {
                          if(tlocal<t)
                          {
                            t=tlocal;
                            itr=itrlocal;
                          }
                 }
              }
            }
          }
    }
    if(t<1000) return true;
    return false;
}

void RenderWidget::DrawRays(){
    for(int i=0;i<raios.size();++i)
    {
        /*
        glBegin(GL_LINES);
        glVertex3f(rayeye.x,rayeye.y,rayeye.z);
        glVertex3f(raios[i].x,raios[i].y,raios[i].z);
        glEnd();*/
    }
}

void RenderWidget::RayTracer()
{
    GLubyte *data=(GLubyte *)malloc(4*cam.width*cam.height);
    glReadPixels(0,0,cam.width,cam.height,4,GL_UNSIGNED_BYTE,data);
    rayimage=new QImage(data,cam.width,cam.height,QImage::Format_RGB32);
    rayimage->save("Imagem.png");
    rayeye=cam.eye;
    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, cam.width, cam.height);
    raios.clear();
    //para cada pixel
    for (int y=0;y<cam.height;y++){
        for(int x=0;x<cam.width;x++){
            glm::vec3 screenPos = glm::vec3(x, y, 0.1);
            glm::vec3 point= glm::unProject(screenPos, view*model, proj, viewport);
            glm::vec3 itr;
            //dispare um raio para cada pixel
            if(IntersectObjs(cam.eye,point,itr)==true)
            {
                raios.push_back(itr);
            }

        }
    }
}


