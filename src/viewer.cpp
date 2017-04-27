#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(1,0,0.25)),
    _mode(false),
    _showShadowMap(false),
    _depthResol(512),
    _spot(0),
    _terrainResol(SIZE_TERRAIN) {

  setlocale(LC_ALL,"C");

  //_mesh = new Mesh(filename);
  //_cam  = new Camera(_mesh->radius,glm::vec3(_mesh->center[0],_mesh->center[1],_mesh->center[2]));
  _cam  = new Camera(1,glm::vec3(0,0,0));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));

  _grid = new Grid(_terrainResol,-1.0f,1.0f);
}

Viewer::~Viewer() {
  delete _timer;
  //delete _mesh;
  delete _cam;
  delete _grid;

  // delete all GPU objects  
  deleteShaders();
  deleteTextures();
  deleteVAO(); 
  deleteFBO();
}

void Viewer::deleteTextures() {
  // delete loaded textures 
  glDeleteTextures(1,&_texTerrain);
  glDeleteTextures(1,&_texNormTerrain);
  glDeleteTextures(1,&_rockTexture);
  glDeleteTextures(1,&_grassTexture);
  glDeleteTextures(2,_texColor);
  glDeleteTextures(2,_texNormal);
}


void Viewer::deleteFBO() {

    glDeleteFramebuffers(1, &_fboTerrain);
    glDeleteFramebuffers(1, &_fboNormal);

}

void Viewer::createFBO() {

    glGenFramebuffers(1, &_fboTerrain);
    glGenFramebuffers(1, &_fboNormal);
    glGenTextures(1, &_texTerrain);
    glGenTextures(1, &_texNormTerrain);

}

void Viewer::noiseFBO() {

    //create texture
    glBindTexture(GL_TEXTURE_2D,_texTerrain);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _terrainResol, _terrainResol, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //bind to FBO
    glBindFramebuffer(GL_FRAMEBUFFER,_fboTerrain);
    glBindTexture(GL_TEXTURE_2D,_texTerrain);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texTerrain,0);

    //unbind
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::normalFBO() {

    //create texture
    glBindTexture(GL_TEXTURE_2D,_texNormTerrain);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _terrainResol, _terrainResol, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //bind to FBO
    glBindFramebuffer(GL_FRAMEBUFFER,_fboNormal);
    glBindTexture(GL_TEXTURE_2D,_texNormTerrain);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texNormTerrain,0);

    //unbind
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

/*void Viewer::loadTexture(GLuint id,const char *filename) {

}*/

void Viewer::createTextures() {
    QImage imgRock, imgGrass;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&_rockTexture);
    glGenTextures(1,&_grassTexture);
    
    glBindTexture(GL_TEXTURE_2D,_rockTexture);
    imgRock = QGLWidget::convertToGLFormat(QImage("textures/rock.jpg"));
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,imgRock.width(),imgRock.height(),0,
             GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)imgRock.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D,_grassTexture);
    imgGrass = QGLWidget::convertToGLFormat(QImage("textures/grass.jpg"));
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,imgGrass.width(),imgGrass.height(),0,
             GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)imgGrass.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

}

void Viewer::createVAO() {
  //the variable _grid should be an instance of Grid
  //the .h file should contain the following VAO/buffer ids
  //GLuint _vaoTerrain;
  //GLuint _vaoQuad;
  //GLuint _terrain[2];
  //GLuint _quad;

  const GLfloat quadData[] = {
    -1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };

  glGenBuffers(2,_terrain);
  glGenVertexArrays(1,&_vaoTerrain);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);

  // create the VBO associated with the screen quad
  glGenBuffers(1,&_quad);
  glGenVertexArrays(1,&_vaoQuad);
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
}

void Viewer::deleteVAO() {
  glDeleteBuffers(2,_terrain);
  glDeleteBuffers(1,&_quad);
  glDeleteVertexArrays(1,&_vaoTerrain);
  glDeleteVertexArrays(1,&_vaoQuad);
}

void Viewer::createShaders() {

  //Noise shader
  _renderingShader = new Shader();
  _renderingShader->load("shaders/noise.vert","shaders/noise.frag");

  //Render texture
  _defaultShader = new Shader();
  _defaultShader->load("shaders/default.vert","shaders/default.frag");
  
  _grilleShader = new Shader();
  _grilleShader->load("shaders/grille.vert","shaders/grille.frag");
  
  //Render montagnes
  _normalShader = new Shader();
  _normalShader->load("shaders/normal.vert","shaders/normal.frag");
  
  _dispShader = new Shader();
  _dispShader->load("shaders/displacement.vert","shaders/displacement.frag");
  
  _lightShader = new Shader();
  _lightShader->load("shaders/light.vert","shaders/light.frag");
}


void Viewer::deleteShaders() {
  delete _renderingShader; _renderingShader = NULL;
  delete _defaultShader; _defaultShader = NULL;
  delete _normalShader; _normalShader = NULL;
  delete _grilleShader; _grilleShader = NULL;
  delete _dispShader; _dispShader = NULL;
  delete _lightShader; _lightShader = NULL;
}

    void Viewer::drawSceneFromCamera(GLuint id) {
	/*const float size = _terrainResol;
	glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
	glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
	glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 m   = glm::mat4(1.0);
	glm::mat4 mv  = v*m;*/
    
	glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
	glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
	
	//const glm::vec3 pos = glm::vec3(0,0,0);
    //const glm::mat4 mdv = glm::translate(_cam->mdvMatrix(),pos);
    glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));

    //lumière
    glUniform3f(glGetUniformLocation(id,"light"), _light[0], _light[1], _light[2]);
    
    //normalMode
    
    glUniform1i(glGetUniformLocation(id,"spot"), _spot);
    
    // envoie la texture du terrain au shader  
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,_texTerrain);
    glUniform1i(glGetUniformLocation(id,"hmap"),0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,_texNormTerrain);
    glUniform1i(glGetUniformLocation(id,"nmap"),1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,_rockTexture);
    glUniform1i(glGetUniformLocation(id,"rocktex"),2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,_grassTexture);
    glUniform1i(glGetUniformLocation(id,"grasstex"),3);
    
	glBindVertexArray(_vaoTerrain);
	glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
	glBindVertexArray(0);
}

void Viewer::paintGL() {

  /* ********************* passe 1 ********************* */

  // Attache le buffer pour le terrain
  glBindFramebuffer(GL_FRAMEBUFFER, _fboTerrain);
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texTerrain, 0);
  //glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glViewport(0,0,_terrainResol,_terrainResol);

  // Clear les buffers avant dessin
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Active le noise shader
  glUseProgram(_renderingShader->id());  

  // dessine sur vaoQuad
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  
  // glDrawBuffer(GL_COLOR_ATTACHMENT0);
  
  //unbind
  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /* ********************* passe 2 ********************* */

  // Attache le buffer pour le terrain
  glBindFramebuffer(GL_FRAMEBUFFER, _fboNormal);
  glViewport(0,0,_terrainResol,_terrainResol);

  // Clear les buffers avant dessin
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Active le normal shader et envoie le bruit généré dans la passe 1
  glUseProgram(_normalShader->id());
  glBindTexture(GL_TEXTURE_2D,_texTerrain);
  glUniform1i(glGetUniformLocation(_defaultShader->id(),"heightmap"),0);

  // dessine sur vaoQuad
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);

  //unbind
  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  /* ********************* passe 3 ********************* */

  // Back to default viewport
  glViewport(0,0,width(),height());
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* 
  // TEST : Decommenter pour afficher la texture voulue
  glUseProgram(_defaultShader->id());
  glBindTexture(GL_TEXTURE_2D,_texNormTerrain);
  glUniform1i(glGetUniformLocation(_defaultShader->id(),"texTerrain"),0);
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  */

  // Déforme la grille
  
  //glUseProgram(_dispShader->id());
  //drawSceneFromCamera(_dispShader->id());
  
  //Lumieres 
  
  glUseProgram(_lightShader->id());
  drawSceneFromCamera(_lightShader->id());
  
  // disable shader 
  glUseProgram(0);
  
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
  noiseFBO();
  normalFBO();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  
  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }
  
  // key l: change l'eclairage
  if(ke->key()==Qt::Key_L) {
    _spot=!_spot;
    cout << "Mode changed" << endl;
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    _renderingShader->reload("shaders/noise.vert","shaders/noise.frag");
    _defaultShader->reload("shaders/default.vert","shaders/default.frag");
    _grilleShader->reload("shaders/grille.vert","shaders/grille.frag");
    _normalShader->reload("shaders/normal.vert","shaders/normal.frag");
    _dispShader->reload("shaders/displacement.vert","shaders/displacement.frag");
    _lightShader->reload("shaders/light.vert","shaders/light.frag");
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  glewExperimental = GL_TRUE;

  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init VAO
  createVAO();
  
  // init textures 
  createTextures();
  
  // create/init FBO
  createFBO();
  noiseFBO();
  normalFBO();

  // starts the timer 
  _timer->start();
}
