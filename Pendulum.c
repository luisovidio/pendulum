#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

GLdouble L1;
GLdouble L2;
GLdouble X1;
GLdouble X2;
GLdouble Y1;
GLdouble Y2;

GLdouble Teta1;
GLdouble Teta2;
GLdouble M1;
GLdouble M2;
GLdouble G;
GLdouble h;
GLdouble Omega1;
GLdouble Omega2;

void Disp();
void Inicializa();
void Timer(int Val);
void Quadrado(GLfloat X, GLfloat Y, GLdouble M);
void Runge();
void Deriva(double In[], double dydx[]);

int main(int argc, char **argv)
{
    glutInit(&argc,argv);


    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);


    glutInitWindowSize(600, 600);
    glutInitWindowPosition(300,100);
    glutCreateWindow("Pendulo Caotico");

    Inicializa();

    glutDisplayFunc(Disp);      //Especifica a funcao que altera o display
    glutTimerFunc(50,Timer, 1); //Chama a funcao Timer a cada 20ms

    glutMainLoop();             //Inicializa o loop da janela

    return 0;
}

void Disp()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(1.0);

    /*Eixo X*/
    glBegin(GL_LINES);
        glColor3f(0.0f,0.0f,0.0f);
        glVertex2d(-10.0,0);
        glVertex2d(10.0,0);
    glEnd();

    /*Eixo Y*/
    glBegin(GL_LINES);
        glColor3f(0.0f,0.0f,0.0f);
        glVertex2d(0,-10.0);
        glVertex2d(0,10.0);
    glEnd();

    glLineWidth(3.5);               //Espessura da linha
    /*Haste 1*/
    glBegin(GL_LINES);              //Cria uma linha
        glColor3f(0.0f,0.0f,1.0f);  //Cor da linha (Azul)
        glVertex2d(0,0);            //Ponto inicial
        X1 = L1 * sin(Teta1);       //X1 em funcao do angulo Teta1
        Y1 = -L1 * cos(Teta1);      //Y1 em funcao do angulo Teta1
        glVertex2d(X1,Y1);          //Ponto final
    glEnd();

    /*Haste 2*/
    glBegin(GL_LINES);
        glColor3f(1.0f,0.0f,0.0f);
        glVertex2d(X1,Y1);
        X2 = L2 * sin(Teta2) + X1;
        Y2 = -L2 * cos(Teta2) + Y1;
        glVertex2d(X2,Y2);
    glEnd();

    /*Peso 1*/
    glColor3f(1.0f,0.5f,0.0f);
    Quadrado(X1,Y1,M1);

    /*Peso 2*/
    glColor3f(0.0f,0.8f,0.0f);
    Quadrado(X2,Y2,M2);

    glFlush();
}

void Inicializa()
{
    glClearColor(1.0f,1.0f,1.0f,0.0f);

    //Tamanho das hastes (m)
    L1 = 4;
    L2 = 2;

    //Angulo inicial em relacao ao eixo Y (Rad)
    Teta1 = M_PI + 0.0000001;
    Teta2 = M_PI ;

    //Aceleracao inicial (Rad/s)
    Omega1 = 0;
    Omega2 = 0;

    //Massa de cada peso (Kg)
    M1 = 1;
    M2 = 1;

    //Gravidade (m/s²)
    G = 9.78;

    //Variacao do Runge-Kutta
    h = 0.08;


    //Especifica quais parametros serao mudados nos proximos comandos
    glMatrixMode(GL_PROJECTION);

    //Valores serao multiplicados pela matriz identidade (sem alteracao)
    glLoadIdentity();

    //Especifica os limites das coordenadas (Esq,Dir,Baixo,Cima)
    gluOrtho2D(-10.0,10.0,-10.0,10.0);
}


void Timer(int Val)
{
    Runge();


    glutPostRedisplay();        //Aplica as alteracoes no display
    glutTimerFunc(50,Timer, 1); //Programa o timer para 50ms de novo
}


//Quadrado de centro X,Y;
void Quadrado(GLfloat X, GLfloat Y, GLdouble M)
{
    glBegin(GL_QUADS);
        glVertex2d(X - (0.35*M), Y + (0.35*M));
        glVertex2d(X + (0.35*M), Y + (0.35*M));
        glVertex2d(X + (0.35*M), Y - (0.35*M));
        glVertex2d(X - (0.35*M), Y - (0.35*M));
    glEnd();
}

void Runge()
{
    double In[4] = {Teta1, Teta2, Omega1, Omega2};
    double Temp[4], dydx[4];
    double K1[4];
    double K2[4];
    double K3[4];
    double K4[4];
    int i;

    Deriva(In,dydx);
    for(i = 0; i < 4; i++)
    {
        K1[i] = h * dydx[i];
        Temp[i] = In[i] + K1[i]/2.0;
    }

    Deriva(Temp,dydx);
    for(i = 0; i < 4; i++)
    {
        K2[i] = h * dydx[i];
        Temp[i] = In[i] + K2[i]/2.0;
    }

    Deriva(Temp,dydx);
    for(i = 0; i < 4; i++)
    {
        K3[i] = h * dydx[i];
        Temp[i] = In[i] + K3[i];
    }

    Deriva(Temp,dydx);
    for(i = 0; i < 4; i++)
    {
        K4[i] = h * dydx[i];
        Temp[i] = In[i] + K1[i] / 6.0 + K2[i] / 3.0 + K3[i] / 3.0 + K4[i] / 6.0;
    }

    Teta1 = Temp[0];
    Teta2 = Temp[1];
    Omega1 = Temp[2];
    Omega2 = Temp[3];
}


void Deriva(double In[], double dydx[])
{
    double D = In[1] - In[0];   //Teta2 - Teta1
    double DM = M2 + M1;

    double Denominador = DM*L1 - M2*L1*cos(D)*cos(D);

    dydx[0] = In[2];


    dydx[2] = (M2*L1*In[2]*In[2]*sin(D)*cos(D)
              + M2*G*sin(In[1])*cos(D)
              + M2*L2*In[3]*In[3]*sin(D)
              - DM*G*sin(In[0])) / Denominador;

    dydx[1] = In[3];

    Denominador *= L2/L1;

    dydx[3] =  (-M2*L2*In[3]*In[3]*sin(D)*cos(D)
               + DM*G*sin(In[0])*cos(D)
               - DM*L1*In[2]*In[2]*sin(D)
               - DM*G*sin(In[1])) / Denominador;
}
