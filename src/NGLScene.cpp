#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>
#include "PhysicsWorld.h"
#include "CollisionShape.h"


NGLScene::NGLScene()
{
  setTitle("Bullet Physics and  NGL Demo");
  m_animate=true;
  m_randomPlace=false;
  m_bboxDraw=false;
  m_wireframe=false;
  m_physics.reset(new PhysicsWorld());
  m_physics->setGravity(ngl::Vec3(0.0f,-10.0f,0.0f));
  m_physics->addGroundPlane(ngl::Vec3(0.0f,0.0f,0.0f),ngl::Vec3(50.0f,0.01f,50.0f));
  ngl::Random::setSeed();
}

void NGLScene::addCube()
{
  ngl::Vec3 pos=ngl::Random::getRandomVec3();
  pos*=10.0f;
  pos.m_y=10.0f;
  if(m_randomPlace == false)
    pos.set(0.0f,10.0f,0.0f);
   m_physics->addBox("box",pos);
}

void NGLScene::addSphere()
{
  ngl::Vec3 pos=ngl::Random::getRandomVec3();
  pos*=10.0f;
  pos.m_y=10.0f;
  if(m_randomPlace == false)
    pos.set(0.0f,10.0f,0.0f);
  m_physics->addSphere("sphere",pos,ngl::Random::randomPositiveNumber(5.0f),ngl::Random::getRandomVec3()*20.0f);

}

void NGLScene::addCylinder()
{
  ngl::Vec3 pos=ngl::Random::getRandomVec3();
  pos*=10.0f;
  pos.m_y=10.0f;
  if(m_randomPlace == false)
    pos.set(0.0f,10.0f,0.0f);
  m_physics->addCylinder("cylinder",pos);

}

void NGLScene::addCone()
{
  ngl::Vec3 pos=ngl::Random::getRandomVec3();
  pos*=10.0f;
  pos.m_y=10.0f;
  if(m_randomPlace == false)
    pos.set(0.0f,10.0f,0.0f);
  m_physics->addCone("cone",pos);

}
void NGLScene::addCapsule()
{
  ngl::Vec3 pos=ngl::Random::getRandomVec3();
  pos*=10;
  pos.m_y=10;
  if(m_randomPlace == false)
    pos.set(0,10,0);
  m_physics->addCapsule("capsule",pos);
}

void NGLScene::addMesh(MeshType _m)
{
  ngl::Vec3 pos=ngl::Random::getRandomVec3();
  pos*=10;
  pos.m_y=10;
  if(m_randomPlace == false)
    pos.set(0,10,0);
  if(_m == TEAPOT)
    m_physics->addMesh("teapot",pos);
  else if(_m == APPLE)
    m_physics->addMesh("apple",pos);


}



NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL( int _w, int _h )
{
  m_project=ngl::perspective( 45.0f, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}

void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::initialize();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);

  ngl::ShaderLib::use("nglDiffuseShader");
  ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,0.0f,1.0f);
  ngl::ShaderLib::setUniform("lightPos",1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);

  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0.0f,5.0f,15.0f);
  ngl::Vec3 to(0.0f,0.0f,0.0f);
  ngl::Vec3 up(0.0f,1.0f,0.0f);
  // now load to our new camera
  m_view=ngl::lookAt(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_project=ngl::perspective(45.0f,720.0f/576.0f,0.05f,350.0f);
  ngl::VAOPrimitives::createSphere("sphere",0.5f,40.0f);
  ngl::VAOPrimitives::createLineGrid("plane",140.0f,140.0f,40.0f);
  m_teapotMesh.reset(new ngl::Obj("models/teapot.obj"));
  m_teapotMesh->createVAO();
  //create a dynamic rigidbody

  m_appleMesh.reset(new ngl::Obj("models/apple.obj"));
  m_appleMesh->createVAO();
  m_appleMesh->calcBoundingSphere();
  ngl::VAOPrimitives::createCapsule("defaultCap",0.5,1.0,20);
  ngl::VAOPrimitives::createCone("cone",0.5,1.0,20,20);
  ngl::VAOPrimitives::createCylinder("cylinder",0.5,2.0,20,20);

 startTimer(10);
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
  // we add the shapes first then refer to them by name in the
  // rest of the system this reduces the overhead to bullet as it
  // can re-use the shapes for efficiency.
  CollisionShape *shapes=CollisionShape::instance();
  shapes->addMesh("teapot","models/teapotCollisionMesh.obj");
  shapes->addMesh("apple","models/appleCollisionMesh.obj");

  shapes->addBox("box",1.0f,1.0f,1.0f);
  shapes->addSphere("sphere",1.0f);
  shapes->addCapsule("capsule",0.5f,1.0f);
  shapes->addCone("cone",0.5f,2.0f);
  shapes->addCylinder("cylinder",0.5f,1.0f);
  m_text=std::make_unique<ngl::Text>("fonts/Arial.ttf",18);
  m_text->setScreenSize(width(),height());
}


void NGLScene::loadMatricesToShader()
{
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  MV = m_view * m_globalTransformMatrix * m_bodyTransform;
  MVP = m_project * MV;

  normalMatrix=MV;

  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP",MVP);
  ngl::ShaderLib::setUniform("normalMatrix",normalMatrix);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // grab an instance of the shader manager
  ngl::ShaderLib::use("nglDiffuseShader");

  // Rotation based on the mouse position for our global transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_win.spinXFace);
  rotY.rotateY(m_win.spinYFace);
  // multiply the rotations
  m_globalTransformMatrix=rotX*rotY;
  // add the translations
  m_globalTransformMatrix.m_m[3][0] = m_modelPos.m_x;
  m_globalTransformMatrix.m_m[3][1] = m_modelPos.m_y;
  m_globalTransformMatrix.m_m[3][2] = m_modelPos.m_z;
  // set this in the TX stack

  size_t bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {


    m_bodyTransform=m_physics->getTransformMatrix(i);

    loadMatricesToShader();
    ngl::ShaderLib::setUniform("Colour",0.0f,0.0f,1.0f,1.0f);
    switch(m_physics->getCollisionShape(i))
    {
      case BOX_SHAPE_PROXYTYPE :
        ngl::ShaderLib::setUniform("Colour",1.0f,0.0f,0.0f,1.0f);
        ngl::VAOPrimitives::draw("cube");
      break;
      case SPHERE_SHAPE_PROXYTYPE :
        ngl::ShaderLib::setUniform("Colour",0.0f,1.0f,0.0f,1.0f);
        ngl::VAOPrimitives::draw("sphere");

      break;
      case CAPSULE_SHAPE_PROXYTYPE :
        ngl::ShaderLib::setUniform("Colour",0.0f,0.0f,1.0f,1.0f);
        ngl::VAOPrimitives::draw("defaultCap");
      break;

      case CONE_SHAPE_PROXYTYPE :
        ngl::ShaderLib::setUniform("Colour",0.0f,1.0f,1.0f,1.0f);
        ngl::VAOPrimitives::draw("cone");
      break;
      case CYLINDER_SHAPE_PROXYTYPE :
        ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,0.0f,1.0f);
        ngl::VAOPrimitives::draw("cylinder");
      break;
      case 4 :
         std::string name=m_physics->getBodyNameAtIndex(i);
        if(name =="teapot")
        {
          ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,0.0f,1.0f);
          m_teapotMesh->draw();
        }
        else if(name =="apple")
        {
          ngl::ShaderLib::setUniform("Colour",0.0f,1.0f,0.0f,1.0f);
          m_appleMesh->draw();
        }
      break;
    }
  }


  ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

  m_bodyTransform.identity();
  loadMatricesToShader();

  ngl::VAOPrimitives::draw("plane");
  m_text->setColour(1,1,1);
  m_text->renderText(10,700,fmt::format("Number of Bodies={}",bodies-1) );


}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent( QMouseEvent* _event )
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if ( m_win.rotate && _event->buttons() == Qt::LeftButton )
  {
    int diffx = _event->x() - m_win.origX;
    int diffy = _event->y() - m_win.origY;
    m_win.spinXFace += static_cast<int>( 0.5f * diffy );
    m_win.spinYFace += static_cast<int>( 0.5f * diffx );
    m_win.origX = _event->x();
    m_win.origY = _event->y();
    update();
  }
  // right mouse translate code
  else if ( m_win.translate && _event->buttons() == Qt::RightButton )
  {
    int diffX      = static_cast<int>( _event->x() - m_win.origXPos );
    int diffY      = static_cast<int>( _event->y() - m_win.origYPos );
    m_win.origXPos = _event->x();
    m_win.origYPos = _event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.origX  = _event->x();
    m_win.origY  = _event->y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if ( _event->button() == Qt::RightButton )
  {
    m_win.origXPos  = _event->x();
    m_win.origYPos  = _event->y();
    m_win.translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.rotate = false;
  }
  // right mouse translate mode
  if ( _event->button() == Qt::RightButton )
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent( QWheelEvent* _event )
{

  // check the diff of the wheel position (0 means no change)
  if ( _event->delta() > 0 )
  {
    m_modelPos.m_z += ZOOM;
  }
  else if ( _event->delta() < 0 )
  {
    m_modelPos.m_z -= ZOOM;
  }
  update();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the NGLScene
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Space : toggleAnimation(); break;
  case Qt::Key_X : stepAnimation(); break;
  case Qt::Key_1 : addCube(); break;
  case Qt::Key_2 : addSphere(); break;
  case Qt::Key_3 : addCapsule(); break;
  case Qt::Key_4 : addCylinder(); break;
  case Qt::Key_5 : addCone(); break;
  case Qt::Key_6 : addMesh(TEAPOT); break;
  case Qt::Key_7 : addMesh(APPLE); break;
  case Qt::Key_Left : m_physics->addImpulse(ngl::Vec3(-5,0.0f,0.0f)); break;
  case Qt::Key_Right : m_physics->addImpulse(ngl::Vec3(5.0f,0.0f,0.0f)); break;
  case Qt::Key_Up : m_physics->addImpulse(ngl::Vec3(0.0f,5.0f,0.0f)); break;
  case Qt::Key_Down : m_physics->addImpulse(ngl::Vec3(0.0f,-5.0f,0.0f)); break;



  case Qt::Key_B : toggleBBox(); break;
  case Qt::Key_R : toggleRandomPlace(); break;
  case Qt::Key_0 : resetSim(); break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}

void NGLScene::resetSim()
{
   m_physics->reset();
}

void NGLScene::timerEvent(QTimerEvent *)
{
  if(m_animate == true)
  {
    m_physics->step(1.0f/60.0f,10);
  }
  update();

}
void NGLScene::stepAnimation()
{
  m_physics->step(1.0f/60.0f,10);

}
