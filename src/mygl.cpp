#include "mygl.h"
#include <la.h>
#include <raytracing/intersection.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <tbb/tbb.h>
#include <QTime>

using namespace tbb;

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent)
{
    setFocusPolicy(Qt::ClickFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    vao.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    vao.create();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat-color shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();

    //Test scene data initialization
    scene.CreateTestScene();
    integrator.scene = &scene;
    integrator.intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;
    ResizeToSceneCamera();

    // test scene build BVH tree
    intersection_engine.root = BVHNode::buildBVHTree(scene.objects);
}

void MyGL::resizeGL(int w, int h)
{
    gl_camera = Camera(w, h);

    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the projection matrix
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function updateGL is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the viewproj matrix
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    GLDrawScene();
}

void MyGL::drawBVHTree(BVHNode *root, int level)
{
    if(root == NULL)
        return;

    if(root->left != NULL)
        drawBVHTree(root->left,level);

    // render first several level
    if(root->depth <= level)
    {
        prog_flat.draw(*this, root->bBox);
    }
//    else // leaf
//    {
//        if(root->left == NULL && root->right == NULL)
//        {
//            prog_flat.draw(*this, root->bBox);
//        }
//    }

    if(root->right != NULL)
        drawBVHTree(root->right,level);
}

void MyGL::GLDrawScene()
{
    for(Geometry *g : scene.objects)
    {
        if(g->drawMode() == GL_TRIANGLES)
        {
            prog_lambert.setModelMatrix(g->transform.T());
            prog_lambert.draw(*this, *g);
        }
        else if(g->drawMode() == GL_LINES)
        {
            prog_flat.setModelMatrix(g->transform.T());
            prog_flat.draw(*this, *g);
        }
        //prog_flat.draw(*this, *(g->bBox));

        ///********************************************************
        /// for rendering the triangles' bbox inside mesh
        QString geometryName(g->getGeometryName());
        if(QString::compare(geometryName, QString("Mesh")) == 0)
        {
            prog_flat.setModelMatrix(g->transform.T());
            drawBVHTree(((Mesh*)g)->bvhTree, 15);
        }
        ///********************************************************

    }

    for(Geometry *l : scene.lights)
    {
        prog_flat.setModelMatrix(l->transform.T());
        prog_flat.draw(*this, *l);
    }
    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(*this, scene.camera);

    drawBVHTree(intersection_engine.root, 30);

}

void MyGL::ResizeToSceneCamera()
{
    this->setFixedWidth(scene.camera.width);
    this->setFixedHeight(scene.camera.height);
//    if(scene.camera.aspect <= 618/432)
//    {
//        this->setFixedWidth(432 / scene.camera.aspect);
//        this->setFixedHeight(432);
//    }
//    else
//    {
//        this->setFixedWidth(618);
//        this->setFixedHeight(618 * scene.camera.aspect);
//    }
    gl_camera = Camera(scene.camera);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_F) {
        gl_camera.CopyAttributes(scene.camera);
    } else if (e->key() == Qt::Key_R) {
        scene.camera = Camera(gl_camera);
        scene.camera.recreate();
    }
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::SceneLoadDialog()
{
    QString filepath = QFileDialog::getOpenFileName(0, QString("Load Scene"), QString("../scene_files"), tr("*.xml"));
    if(filepath.length() == 0)
    {
        return;
    }

    QFile file(filepath);
    int i = filepath.length() - 1;
    while(QString::compare(filepath.at(i), QChar('/')) != 0)
    {
        i--;
    }
    QStringRef local_path = filepath.leftRef(i+1);

    //Reset all of our objects
    BVHNode::releaseTree(intersection_engine.root);
    scene.Clear();
    integrator = Integrator();
    intersection_engine = IntersectionEngine();
    //Load new objects based on the XML file chosen.
    xml_reader.LoadSceneFromFile(file, local_path, scene, integrator);
    integrator.scene = &scene;
    integrator.intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;

    //build bvh tree
    intersection_engine.root = BVHNode::buildBVHTree(scene.objects);
}

void MyGL::RaytraceScene()
{
    QString filepath = QFileDialog::getSaveFileName(0, QString("Save Image"), QString("../rendered_images"), tr("*.bmp"));
    if(filepath.length() == 0)
    {
        return;
    }

    //(debug) printing intersection results to file for future reference
    //FILE *file;
    //file = fopen("log.txt","wt");

    // allocate memory for film.pixels
    scene.film.pixels.resize(scene.film.width);
    for (unsigned int i=0;i<scene.film.width;i++)
        scene.film.pixels[i].resize(scene.film.height);


    /// FOLLOWING IS FOR BEST CANDIDATE SAMPLER!!!!!!!
    // best candidate sampler
    if(QString::compare(scene.pixel_sampler->GetSamplerType(), QString("bestcandidate") )== 0)
    {
        QTime time;
        time.start();

        int sumTemp[scene.film.width][scene.film.height];
        memset(sumTemp,0,sizeof(int)*scene.film.width*scene.film.height);

        // get samples and traceray
        QList<glm::vec2> samples = scene.pixel_sampler->GetSamples(scene.film.width,scene.film.height);
        for(glm::vec2 s:samples)
        {
            Ray r = scene.camera.Raycast(s.x, s.y);
            int i = int(s.x);
            int j = int(s.y);
            scene.film.pixels[i][j] = scene.film.pixels[i][j] + integrator.TraceRay(r, 0);
            sumTemp[i][j] += 1;
        }

        // average pixels
        for(int i=0;i<scene.film.width;i++)
            for(int j=0;j<scene.film.height;j++)
                if(sumTemp[i][j] != 0)
                {
                    scene.film.pixels[i][j] = scene.film.pixels[i][j] / float(sumTemp[i][j]);
                }


        int diff = time.elapsed();

        std::cout << "rendering time = "
                  << float(diff)
                  << " milliseconds.\n\n";

        scene.film.WriteImage(filepath);
        return;
    }


    /// FOLLOWING IS FOR OTHER SAMPLERS EXCEPT FOR BEST_CANDIDATE
    // other sampler canbe handled normally
    QTime time;
    time.start();
    #define TBB //Uncomment this line out to render your scene with multiple threads.
    //This is useful when debugging your raytracer with breakpoints.
    #ifdef TBB
        parallel_for(0, (int)scene.camera.width, 1, [=](unsigned int i)
        {
            for(unsigned int j = 0; j < scene.camera.height; j++)
            {
                glm::vec3 resultColor(0.0f);

                QList<glm::vec2> samples_pixel = scene.pixel_sampler->GetSamples(i, j);
                for(glm::vec2 sample : samples_pixel)
                {
                    Ray r = scene.camera.Raycast(sample.x, sample.y);
                    resultColor = resultColor + integrator.TraceRay(r, 0);
                }

                scene.film.pixels[i][j] = resultColor / float(samples_pixel.size());
            }
        });
    #else
        for(unsigned int i = 0; i < scene.camera.width; i++)
        {
            for(unsigned int j = 0; j < scene.camera.height; j++)
            {
                Ray r = scene.camera.Raycast(i,j);
                scene.film.pixels[i][j] = integrator.TraceRay(r, 0);


//                Intersection inter = intersection_engine.GetIntersection(r);
//                if(inter.t > 0)
//                    scene.film.pixels[i][j] = inter.color;
            }   
        }
    #endif
    int diff = time.elapsed();

    std::cout << "rendering time = "
              << float(diff)
              << " milliseconds.\n\n";


    //fclose(file);
    scene.film.WriteImage(filepath);
}
