#include "dmlayer.h"

#include "dmcomponent.h"
#include "dmnode.h"
#include "dmface.h"
#include "dmedge.h"
#include "dmattribute.h"
#include "dmsystemiterators.h"
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <QImage>
#include <QPainter>
#include <cassert>

typedef void (APIENTRY *GLU_TESS_CALLBACK)();


namespace DM {

template<int SD_GL_PRIMITIVE>
struct SimpleDrawer {

    void operator()(DM::System *s, DM::View v, void *f_e, DM::Node *n, iterator_pos pos) {
        if (pos == before) {
            glBegin(SD_GL_PRIMITIVE);
            return;
        }
        if (pos == after) {
            glEnd();
            return;
        }
        glColor3f(0, 0, 0);
        glVertex3dv(n->get());
    }
};

void error_callback(GLenum e) {
    const char *error_string = (const char *) gluErrorString(e);
    int x = 100;
}

void color_callback(GLdouble *d, void *data) {
    glColor3f(0.0, 0.0, 0.0);
    glVertex3dv(d);
}

void texture_callback(GLdouble *d, void *data) {
    glColor3f(1, 1, 1);
    glTexCoord2dv(&d[3]);
    glVertex3dv(d);
}

void combine_callback(GLdouble coords[3], 
                      GLdouble *vertex_data[4],
                      GLfloat weight[4], GLdouble **dataOut,
                      void *user_data) {
    GLuint *texture = (GLuint*) user_data;
    GLdouble *vertex = new GLdouble[5]; //LEAAAAAK!!!
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    
    GLdouble *v1 = vertex_data[0];
    
    if (glIsTexture(*texture)) {
        vertex[3] = v1[3];
        vertex[4] = v1[4];
    }
    
    *dataOut = vertex;
}

struct TesselatedFaceDrawer {
    std::string attr;
    double height_scale, attr_span;
    GLuint texture, name_start;
    const ViewMetaData &vmd;
    int attribute_vector_name;
    
    std::vector<GLdouble *> vertices;
    
    TesselatedFaceDrawer(const ViewMetaData &vmd, GLuint name_start, 
                         std::string attr = "", double height_scale = -1, 
                         GLuint texture = -1, int attribute_vector_name = 0)
        : attr(attr), texture(texture), 
          vmd(vmd), name_start(name_start), 
          attribute_vector_name(attribute_vector_name) {
        
        this->height_scale = 1.0/vmd.attr_max*vmd.radius()*height_scale;
        this->attr_span = vmd.attr_max - vmd.attr_min;
    }
    
    void operator()(DM::System *s, DM::View v, DM::Face *f, DM::Node *n, iterator_pos pos) {
        if (pos == after) {
            render();
            name_start++;
            return;
        }
        if (pos != in_between) return;
        
        GLdouble *d = new GLdouble[5];
        
        d[0] = n->getX();
        d[1] = n->getY();
        d[2] = 0.0;
        if (height_scale > 0) {
            Attribute *a = f->getAttribute(attr);
            if (a->getType() == Attribute::DOUBLE) {
                d[2] = f->getAttribute(attr)->getDouble() * height_scale;
            } else {
                double attr_value = a->getDoubleVector()[attribute_vector_name];
                d[2] = attr_value * height_scale;
            }
        }
        
        if (glIsTexture(texture)) {
            Attribute *a = f->getAttribute(attr);
            if (a->getType() == Attribute::DOUBLE) {
                d[3] = (a->getDouble() - vmd.attr_min) / attr_span;
            } else {
                d[3] = (a->getDoubleVector()[attribute_vector_name] - vmd.attr_min) / attr_span;
            }
            d[4] = 0.5;
        } else {
            d[3] = d[4] = 0.0;
        }
        vertices.push_back(d);
    }
    
    void render() {
        if (glIsTexture(texture)) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        assert(glGetError() == GL_NO_ERROR);
        GLUtesselator *tess = gluNewTess();
        gluTessCallback(tess, GLU_TESS_ERROR, (void (__stdcall*)(void)) error_callback);
        gluTessCallback(tess, GLU_TESS_BEGIN, (void (__stdcall*)(void)) glBegin);
        gluTessCallback(tess, GLU_TESS_COMBINE_DATA, (void (__stdcall*)(void)) combine_callback);
        if (glIsTexture(texture)) {
            gluTessCallback(tess, GLU_TESS_VERTEX, (void (__stdcall*)(void)) texture_callback);
        } else {
            gluTessCallback(tess, GLU_TESS_VERTEX, (void (__stdcall*)(void)) color_callback);
        }
        gluTessCallback(tess, GLU_TESS_END, glEnd);
        
        glPushName(name_start);
        
        gluTessBeginPolygon(tess, &this->texture);
        gluTessBeginContour(tess);
        
        foreach(GLdouble *v, vertices) {
            gluTessVertex(tess, v, v);
        }
        
        gluTessEndContour(tess);
        gluTessEndPolygon(tess);
        gluDeleteTess(tess);
        
        glPopName();
        
        assert(glGetError() == GL_NO_ERROR);
        
        foreach (GLdouble *v, vertices) {
            delete v;
        }
        
        vertices.clear();
        
        if (glIsTexture(texture)) glDisable(GL_TEXTURE_2D);
    }
};


Layer::Layer(System *s, View v, const std::string &a) 
    : system(s), view(v), 
      attribute(a), vmd(a),
      attribute_vector_name(0) {
}

void Layer::setColorInterpretation(GLuint texture) {
    this->texture = texture;
}

void Layer::setHeightInterpretation(float percent) {
    scale_height = percent;
}

void Layer::draw() {
    if (lists.size() <= attribute_vector_name) {
        lists.resize(attribute_vector_name+1, -1);
    }
    if (!glIsList(lists[attribute_vector_name])) {
        lists[attribute_vector_name] = glGenLists(1);
        glNewList(lists[attribute_vector_name], GL_COMPILE);
        
        if (view.getType() == DM::FACE) {
            TesselatedFaceDrawer drawer(vmd, name_start, attribute, scale_height, texture, attribute_vector_name);
            iterate_faces(system, view, drawer);
        }
        if (view.getType() == DM::EDGE) {
            SimpleDrawer<GL_LINES> drawer;
            iterate_edges(system, view, drawer);
        }
        if (view.getType() == DM::NODE) {
            SimpleDrawer<GL_POINTS> drawer;
            iterate_nodes(system, view, drawer);
        }
        
        glEndList();
    }
    
    glPushMatrix();
    glTranslated(x_off, y_off, z_off);
    assert(glIsList(lists[attribute_vector_name]));
    glCallList(lists[attribute_vector_name]);
    glPopMatrix();
}

void Layer::drawWithNames() {
    draw();
}

void Layer::systemChanged() {
    vmd = ViewMetaData(attribute);

    if (view.getType() == DM::FACE) {
        iterate_faces(system, view, vmd);
    }
    if (view.getType() == DM::EDGE) {
        iterate_edges(system, view, vmd);
    }
    if (view.getType() == DM::NODE) {
        iterate_nodes(system, view, vmd);
    }
    
    foreach (GLuint list, lists) {
        if (glIsList(list)) {
            glDeleteLists(list, 1);
        }
    }
}

} // namespace DM
