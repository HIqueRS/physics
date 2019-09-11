//*****************************************************
// Projeto OpenGL2 + GLFW + Box2D
//
// Finalmente, removendo a GLUT da história :D
//
// Adaptado do projeto com GLUT por Rossana B. Queiroz 08/2011
// Última modificação: 30/03/2017
//*****************************************************

#include <cstdlib>
#include <iostream>
#include "Render.h" //é a Render que está incluindo a glfw!
#include <ctime>

//oooo

using namespace std;

//Algumas globais para interface e simulação (IDEAL: criar uma classe gerenciadora)
float32 timeStep;
int32 velocityIterations;
int32 positionIterations;
float xMin = -50.0, xMax = 50.0, yMin = -50.0, yMax = 50.0; //ortho2D

// O objeto World serve para armazenar os dados da simulação --> MUNDO FÍSICO DA BOX2D
b2World *world;

//Alguns corpos rígidos
b2Body *box1, *box2;
b2Body* ground;
b2Body* Circ;
b2Body* test;

//Objeto para a classe que faz o desenho das formas de colisão dos corpos rígidos
DebugDraw renderer;

int EX = 0;
bool Four = false;
float Res = 0.f;

//Callback de erro - PADRÃO DA GLFW - não precisa mexer
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


//Rotina que cria uma caixa sempre com os mesmos atributos, variando só a posição de seu centro
b2Body *CriaCaixa(float posX, float posY)
{
	b2Body *novoObjeto;

	//Primeiro, criamos a definição do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter até 8 vértices
	b2PolygonShape forma,forma2;
	forma.SetAsBox(5, 5);
	/*b2Vec2 points[4];
	points[0].x = 0.0f;
	points[0].y = 0.0f;

	points[1].x = 0.0f;
	points[1].y = 1.0f;

	points[2].x = 1.0f;
	points[2].y = 1.0f;

	points[3].x = 1.0f;
	points[3].y = 0.0f;

	forma.Set(points, 4);

	points[0].x = 1.0f;
	points[0].y = 1.0f;

	points[1].x = 1.0f;
	points[1].y = 10.0f;

	points[2].x = 10.0f;
	points[2].y = 10.0f;

	points[3].x = 10.0f;
	points[3].y = 1.0f;

	forma2.Set(points, 4);*/

	//Depois, criamos uma fixture que vai conter a forma do corpo
	b2FixtureDef fix,fix2;
	fix.shape = &forma;
	fix2.shape = &forma2;
	//Setamos outras propriedades da fixture
	fix.density = 10.0;
	fix.friction = 0.5;
	fix.restitution = 0.5;

	fix2.density = 10.0;
	fix2.friction = 0.5;
	fix2.restitution = 0.5;

	//Por fim, criamos o corpo...
	novoObjeto = world->CreateBody(&bodyDef);
	//... e criamos a fixture do corpo 	
	novoObjeto->CreateFixture(&fix);
	//novoObjeto->CreateFixture(&fix2);

	

	return novoObjeto;
}

b2Body *CreateBox(float posX, float posY,int tam,float den, float atr, float res)
{
	b2Body *novoObjeto;

	//Primeiro, criamos a definição do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter até 8 vértices
	b2PolygonShape forma;
	forma.SetAsBox(tam, tam);

	

	//Depois, criamos uma fixture que vai conter a forma do corpo
	b2FixtureDef fix;
	fix.shape = &forma;
	//Setamos outras propriedades da fixture
	fix.density = den;
	fix.friction = atr;
	fix.restitution = res;

	//Por fim, criamos o corpo...
	novoObjeto = world->CreateBody(&bodyDef);
	//... e criamos a fixture do corpo 	
	novoObjeto->CreateFixture(&fix);

	return novoObjeto;
}

b2Body *CreateCircule(float posX, float posY, int tam, float den, float atr, float res)
{
	b2Body *novoObjeto;

	//Primeiro, criamos a definição do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter até 8 vértices
	//b2PolygonShape forma;
	//forma.SetAsBox(tam, tam);
	b2CircleShape forma;
	forma.m_radius = tam;

	//Depois, criamos uma fixture que vai conter a forma do corpo
	b2FixtureDef fix;
	fix.shape = &forma;
	//Setamos outras propriedades da fixture
	fix.density = den;
	fix.friction = atr;
	fix.restitution = res;

	//Por fim, criamos o corpo...
	novoObjeto = world->CreateBody(&bodyDef);
	//... e criamos a fixture do corpo 	
	novoObjeto->CreateFixture(&fix);

	return novoObjeto;
}

b2Body *CreateEdge(float posX, float posY, int tam, float den, float atr, float res)
{
	b2Body *novoObjeto;

	//Primeiro, criamos a definição do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter até 8 vértices
	//b2PolygonShape forma;
	//forma.SetAsBox(tam, tam);
	b2EdgeShape forma;
	forma.Set(b2Vec2(0, 0), b2Vec2(tam, tam));
	

	//Depois, criamos uma fixture que vai conter a forma do corpo
	b2FixtureDef fix;
	fix.shape = &forma;
	//Setamos outras propriedades da fixture
	fix.density = den;
	fix.friction = atr;
	fix.restitution = res;

	//Por fim, criamos o corpo...
	novoObjeto = world->CreateBody(&bodyDef);
	//... e criamos a fixture do corpo 	
	novoObjeto->CreateFixture(&fix);

	return novoObjeto;
}

//Função de inicialização da Box2D
void InitBox2D()
{

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento - VALORES RECOMENDADOS no manual
	velocityIterations = 10;
	positionIterations = 8;
	timeStep = 1.0f / 60.0f; //60 Hz -- 1/60 seconds - pensando em tempo real para jogos (60FPS)

	//Criação de corpo rígido: caixa
	box1 = CriaCaixa(-10, 10);

	//Criação de corpo rígido: caixa
	//box2 = CriaCaixa(10, 10);

	Circ = CreateCircule(10, 10, 5, 0.5, 0.5, 0.5);

	//Cria o chão
	//criação do corpo rígido
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	//Forma do chão: edge
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape, 1.0);

	//Cria parede esq
	//criação do corpo rígido
	b2BodyDef bd1;
	ground = world->CreateBody(&bd1);
	//Forma do chão: edge
	b2EdgeShape shape1;
	shape1.Set(b2Vec2(-39.5, -39.5), b2Vec2(-39.5, 39.5));
	ground->CreateFixture(&shape1, 1.0);

	//Cria teto
	//criação do corpo rígido
	b2BodyDef bd2;
	ground = world->CreateBody(&bd2);
	//Forma do chão: edge
	b2EdgeShape shape2;
	shape2.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
	ground->CreateFixture(&shape2, 1.0);

	//Cria paredir
	//criação do corpo rígido
	b2BodyDef bd3;
	ground = world->CreateBody(&bd3);
	//Forma do chão: edge
	b2EdgeShape shape3;
	shape3.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape3, 1.0);

}

void InitBox2D4()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento - VALORES RECOMENDADOS no manual
	velocityIterations = 10;
	positionIterations = 8;
	timeStep = 1.0f / 60.0f; //60 Hz -- 1/60 seconds - pensando em tempo real para jogos (60FPS)

	//Cria o chão
	//criação do corpo rígido
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	//Forma do chão: edge
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape, 1.0);

	//Cria parede esq
	//criação do corpo rígido
	b2BodyDef bd1;
	ground = world->CreateBody(&bd1);
	//Forma do chão: edge
	b2EdgeShape shape1;
	shape1.Set(b2Vec2(-39.5, -39.5), b2Vec2(-39.5, 39.5));
	ground->CreateFixture(&shape1, 1.0);

	//Cria teto
	//criação do corpo rígido
	b2BodyDef bd2;
	ground = world->CreateBody(&bd2);
	//Forma do chão: edge
	b2EdgeShape shape2;
	shape2.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
	ground->CreateFixture(&shape2, 1.0);

	//Cria paredir
	//criação do corpo rígido
	b2BodyDef bd3;
	ground = world->CreateBody(&bd3);
	//Forma do chão: edge
	b2EdgeShape shape3;
	shape3.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape3, 1.0);

}

void InitBox2D5()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento - VALORES RECOMENDADOS no manual
	velocityIterations = 10;
	positionIterations = 8;
	timeStep = 1.0f / 60.0f; //60 Hz -- 1/60 seconds - pensando em tempo real para jogos (60FPS)

	b2BodyDef bdr;
	ground = world->CreateBody(&bdr);
	//Forma do chão: edge
	b2EdgeShape shaper;
	shaper.Set(b2Vec2(0, -39.5), b2Vec2(39.5, 0));
	ground->CreateFixture(&shaper, 1.0);

	//Cria o chão
	//criação do corpo rígido
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	//Forma do chão: edge
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape, 1.0);

	//Cria parede esq
	//criação do corpo rígido
	b2BodyDef bd1;
	ground = world->CreateBody(&bd1);
	//Forma do chão: edge
	b2EdgeShape shape1;
	shape1.Set(b2Vec2(-39.5, -39.5), b2Vec2(-39.5, 39.5));
	ground->CreateFixture(&shape1, 1.0);

	//Cria teto
	//criação do corpo rígido
	b2BodyDef bd2;
	ground = world->CreateBody(&bd2);
	//Forma do chão: edge
	b2EdgeShape shape2;
	shape2.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
	ground->CreateFixture(&shape2, 1.0);

	//Cria paredir
	//criação do corpo rígido
	b2BodyDef bd3;
	ground = world->CreateBody(&bd3);
	//Forma do chão: edge
	b2EdgeShape shape3;
	shape3.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape3, 1.0);

}

void InitBox2D6()
{

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento - VALORES RECOMENDADOS no manual
	velocityIterations = 10;
	positionIterations = 8;
	timeStep = 1.0f / 60.0f; //60 Hz -- 1/60 seconds - pensando em tempo real para jogos (60FPS)

	//Criação de corpo rígido: caixa
	box1 = CriaCaixa(-10, -35);
	box1 = CriaCaixa(-10, -25.1);
	box1 = CriaCaixa(-10, -15.2);
	box1 = CriaCaixa(-10, -5.3);
	box1 = CriaCaixa(-10, 5.4);
	box1 = CriaCaixa(-10, 15.5);

	//Criação de corpo rígido: caixa
	//box2 = CriaCaixa(10, 10);

	Circ = CreateCircule(10, -35, 5, 0.5, 0.5, 0.5);
	Circ = CreateCircule(10, -25.1, 5, 0.5, 0.5, 0.5);
	Circ = CreateCircule(10, -15.2, 5, 0.5, 0.5, 0.5);
	Circ = CreateCircule(10, -5.3, 5, 0.5, 0.5, 0.5);
	Circ = CreateCircule(10, 5.4, 5, 0.5, 0.5, 0.5);
	Circ = CreateCircule(10, 15.5, 5, 0.5, 0.5, 0.5);

	//Cria o chão
	//criação do corpo rígido
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	//Forma do chão: edge
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape, 1.0);

	//Cria parede esq
	//criação do corpo rígido
	b2BodyDef bd1;
	ground = world->CreateBody(&bd1);
	//Forma do chão: edge
	b2EdgeShape shape1;
	shape1.Set(b2Vec2(-39.5, -39.5), b2Vec2(-39.5, 39.5));
	ground->CreateFixture(&shape1, 1.0);

	//Cria teto
	//criação do corpo rígido
	b2BodyDef bd2;
	ground = world->CreateBody(&bd2);
	//Forma do chão: edge
	b2EdgeShape shape2;
	shape2.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
	ground->CreateFixture(&shape2, 1.0);

	//Cria paredir
	//criação do corpo rígido
	b2BodyDef bd3;
	ground = world->CreateBody(&bd3);
	//Forma do chão: edge
	b2EdgeShape shape3;
	shape3.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape3, 1.0);

}

void InitBox2D7()
{

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento - VALORES RECOMENDADOS no manual
	velocityIterations = 10;
	positionIterations = 8;
	timeStep = 1.0f / 60.0f; //60 Hz -- 1/60 seconds - pensando em tempo real para jogos (60FPS)

	//Criação de corpo rígido: caixa
	box1 = CriaCaixa(-49, -49);
	box1 = CriaCaixa(-49, -39);
	box1 = CriaCaixa(-49, -29);
	box1 = CriaCaixa(-49, -19);
	box1 = CriaCaixa(-49, -9);
	box1 = CriaCaixa(-49, 9);
	box1 = CriaCaixa(-49, 19);
	box1 = CriaCaixa(-49, 29);
	box1 = CriaCaixa(-49, 39);
	box1 = CriaCaixa(-49, 49);

	box1 = CriaCaixa(-39, -49);
	box1 = CriaCaixa(-39, -39);
	box1 = CriaCaixa(-39, -29);
	box1 = CriaCaixa(-39, -19);
	box1 = CriaCaixa(-39, -9);
	box1 = CriaCaixa(-39, 9);
	box1 = CriaCaixa(-39, 19);
	box1 = CriaCaixa(-39, 29);
	box1 = CriaCaixa(-39, 39);
	box1 = CriaCaixa(-39, 49);

	box1 = CriaCaixa(-29, -49);
	box1 = CriaCaixa(-29, -39);
	box1 = CriaCaixa(-29, -29);
	box1 = CriaCaixa(-29, -19);
	box1 = CriaCaixa(-29, -9);
	box1 = CriaCaixa(-29, 9);
	box1 = CriaCaixa(-29, 19);
	box1 = CriaCaixa(-29, 29);
	box1 = CriaCaixa(-29, 39);
	box1 = CriaCaixa(-29, 49);

	box1 = CriaCaixa(-19, -49);
	box1 = CriaCaixa(-19, -39);
	box1 = CriaCaixa(-19, -29);
	box1 = CriaCaixa(-19, -19);
	box1 = CriaCaixa(-19, -9);
	box1 = CriaCaixa(-19, 9);
	box1 = CriaCaixa(-19, 19);
	box1 = CriaCaixa(-19, 29);
	box1 = CriaCaixa(-19, 39);
	box1 = CriaCaixa(-19, 49);

	box1 = CriaCaixa(-9, -49);
	box1 = CriaCaixa(-9, -39);
	box1 = CriaCaixa(-9, -29);
	box1 = CriaCaixa(-9, -19);
	box1 = CriaCaixa(-9, -9);
	box1 = CriaCaixa(-9, 9);
	box1 = CriaCaixa(-9, 19);
	box1 = CriaCaixa(-9, 29);
	box1 = CriaCaixa(-9, 39);
	box1 = CriaCaixa(-9, 49);

	box1 = CriaCaixa(9, -49);
	box1 = CriaCaixa(9, -39);
	box1 = CriaCaixa(9, -29);
	box1 = CriaCaixa(9, -19);
	box1 = CriaCaixa(9, -9);
	box1 = CriaCaixa(9, 9);
	box1 = CriaCaixa(9, 19);
	box1 = CriaCaixa(9, 29);
	box1 = CriaCaixa(9, 39);
	box1 = CriaCaixa(9, 49);

	box1 = CriaCaixa(19, -49);
	box1 = CriaCaixa(19, -39);
	box1 = CriaCaixa(19, -29);
	box1 = CriaCaixa(19, -19);
	box1 = CriaCaixa(19, -9);
	box1 = CriaCaixa(19, 9);
	box1 = CriaCaixa(19, 19);
	box1 = CriaCaixa(19, 29);
	box1 = CriaCaixa(19, 39);
	box1 = CriaCaixa(19, 49);

	box1 = CriaCaixa(29, -49);
	box1 = CriaCaixa(29, -39);
	box1 = CriaCaixa(29, -29);
	box1 = CriaCaixa(29, -19);
	box1 = CriaCaixa(29, -9);
	box1 = CriaCaixa(29, 9);
	box1 = CriaCaixa(29, 19);
	box1 = CriaCaixa(29, 29);
	box1 = CriaCaixa(29, 39);
	box1 = CriaCaixa(29, 49);

	box1 = CriaCaixa(39, -49);
	box1 = CriaCaixa(39, -39);
	box1 = CriaCaixa(39, -29);
	box1 = CriaCaixa(39, -19);
	box1 = CriaCaixa(39, -9);
	box1 = CriaCaixa(39, 9);
	box1 = CriaCaixa(39, 19);
	box1 = CriaCaixa(39, 29);
	box1 = CriaCaixa(39, 39);
	box1 = CriaCaixa(39, 49);

	box1 = CriaCaixa(49, -49);
	box1 = CriaCaixa(49, -39);
	box1 = CriaCaixa(49, -29);
	box1 = CriaCaixa(49, -19);
	box1 = CriaCaixa(49, -9);
	box1 = CriaCaixa(49, 9);
	box1 = CriaCaixa(49, 19);
	box1 = CriaCaixa(49, 29);
	box1 = CriaCaixa(49, 39);
	box1 = CriaCaixa(49, 49);
	

	//Criação de corpo rígido: caixa
	//box2 = CriaCaixa(10, 10);

	

	//Cria o chão
	//criação do corpo rígido
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	//Forma do chão: edge
	b2EdgeShape shape;
	shape.Set(b2Vec2(-49.5, -49.5), b2Vec2(49.5, -49.5));
	ground->CreateFixture(&shape, 1.0);

	//Cria parede esq
	//criação do corpo rígido
	b2BodyDef bd1;
	ground = world->CreateBody(&bd1);
	//Forma do chão: edge
	b2EdgeShape shape1;
	shape1.Set(b2Vec2(-49.5, -49.5), b2Vec2(-49.5, 49.5));
	ground->CreateFixture(&shape1, 1.0);

	//Cria teto
	//criação do corpo rígido
	b2BodyDef bd2;
	ground = world->CreateBody(&bd2);
	//Forma do chão: edge
	b2EdgeShape shape2;
	shape2.Set(b2Vec2(-49.5, 49.5), b2Vec2(49.5, 49.5));
	ground->CreateFixture(&shape2, 1.0);

	//Cria paredir
	//criação do corpo rígido
	b2BodyDef bd3;
	ground = world->CreateBody(&bd3);
	//Forma do chão: edge
	b2EdgeShape shape3;
	shape3.Set(b2Vec2(49.5, 49.5), b2Vec2(49.5, -49.5));
	ground->CreateFixture(&shape3, 1.0);

}

void InitBox2D8()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento - VALORES RECOMENDADOS no manual
	velocityIterations = 10;
	positionIterations = 8;
	timeStep = 1.0f / 60.0f; //60 Hz -- 1/60 seconds - pensando em tempo real para jogos (60FPS)


	b2Vec2 points_1[3],points_2[3];

	//triangulo de fora
	points_1[0].x = 3*5;
	points_1[0].y = 2*5;
					 
	points_1[1].x = 5*5;
	points_1[1].y = 6*5;
					 
	points_1[2].x = 7*5;
	points_1[2].y = 2*5;
	//triangulo de de*5ntro
	points_2[0].x = 5*5;
	points_2[0].y = 2*5;
		   			 
	points_2[1].x = 4*5;
	points_2[1].y = 4*5;
		   			 
	points_2[2].x = 6*5;
	points_2[2].y = 4*5;

	b2Body *tri;

	b2BodyDef bodyDef;
	bodyDef.position.Set(0, 0);
	bodyDef.type = b2_dynamicBody;

	b2PolygonShape forma,forma2;
	forma.Set(points_1, 3);
	forma2.Set(points_2, 3);

	b2FixtureDef fix, fix2;
	fix.shape = &forma;
	fix2.shape = &forma2;
	//Setamos outras propriedades da fixture
	fix.density = 10.0;
	fix.friction = 0.5;
	fix.restitution = 0.5;

	fix2.density = 10.0;
	fix2.friction = 0.5;
	fix2.restitution = 0.5;

	//Por fim, criamos o corpo...
	tri = world->CreateBody(&bodyDef);
	//... e criamos a fixture do corpo 	
	tri->CreateFixture(&fix);
	tri->CreateFixture(&fix2);

	//Cria o chão
	//criação do corpo rígido
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	//Forma do chão: edge
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape, 1.0);

	//Cria parede esq
	//criação do corpo rígido
	b2BodyDef bd1;
	ground = world->CreateBody(&bd1);
	//Forma do chão: edge
	b2EdgeShape shape1;
	shape1.Set(b2Vec2(-39.5, -39.5), b2Vec2(-39.5, 39.5));
	ground->CreateFixture(&shape1, 1.0);

	//Cria teto
	//criação do corpo rígido
	b2BodyDef bd2;
	ground = world->CreateBody(&bd2);
	//Forma do chão: edge
	b2EdgeShape shape2;
	shape2.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
	ground->CreateFixture(&shape2, 1.0);

	//Cria paredir
	//criação do corpo rígido
	b2BodyDef bd3;
	ground = world->CreateBody(&bd3);
	//Forma do chão: edge
	b2EdgeShape shape3;
	shape3.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape3, 1.0);

}

//Callback de teclado - PADRÃO DA GLFW - alterar conforme desejar (teclas de controle do programa)
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		if (EX == 4)
		{
			test = CreateBox(rand() % 80 - 40, rand() % 80 - 40, rand() % 10 + 1, (rand() % 10) / 10, (rand() % 10) / 10, Res);
			Res += 1;
			if (Res > 1)
			{
				Res = 0;
			}
		}
		else if (EX == 5)
		{
			test = CreateBox(35,7 , 5, 10, Res, 0.5);
			Res += 0.1;
			if (Res > 1)
			{
				Res = 0;
			}
		}
		else
		{
			test = CreateBox(rand() % 80 - 40, rand() % 80 - 40, rand() % 10 + 1, (rand() % 10) / 10, (rand() % 10) / 10, (rand() % 10) / 10);
		}
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		//test = CriaCi(10, 10);
		test = CreateCircule(rand() % 80 - 40, rand() % 80 - 40, rand() % 10 + 1, (rand() % 10) / 10, (rand() % 10) / 10, (rand() % 10) / 10);
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		//test = CriaCi(10, 10);
		test = CreateEdge(rand() % 80 - 40, rand() % 80 - 40, rand() % 10 + 1, (rand() % 10) / 10, (rand() % 10) / 10, (rand() % 10) / 10);
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		InitBox2D();
		EX = 0;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		InitBox2D4();
		EX = 4;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		InitBox2D5();
		EX = 5;
	}
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		InitBox2D6();
		
	}
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		InitBox2D7();

	}
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		InitBox2D8();

	}
}





// Função de Execução da Simulação
void RunBox2D()
{
	world->Step(timeStep, velocityIterations, positionIterations);
	world->ClearForces();
}

// Função que imprime todos os objetos  
void PrintBodies()
{
	b2Body *b;
	float ang;
	b2Vec2 pos;
	//PERCORRE A LISTA DE CORPOS RÍGIDOS DO MUNDO
	for (b = world->GetBodyList(); b; b = b->GetNext())
	{
		pos = b->GetPosition();
		ang = b->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", pos.x, pos.y, ang);
	}
}

//Função que chama as rotinas de desenho para cada fixture de um corpo rígido, de acordo com o tipo da forma de colisão dela e 
void DrawFixture(b2Fixture* fixture, b2Color color)
{
	const b2Transform& xf = fixture->GetBody()->GetTransform();

	switch (fixture->GetType())
	{
	case b2Shape::e_circle:
	{
		b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

		b2Vec2 center = b2Mul(xf, circle->m_p);
		float32 radius = circle->m_radius;

		renderer.DrawCircle(center, radius, color);
	}
	break;

	case b2Shape::e_polygon:
	{
		b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
		int32 vertexCount = poly->m_count;
		b2Assert(vertexCount <= b2_maxPolygonVertices);
		b2Vec2 vertices[b2_maxPolygonVertices];

		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = b2Mul(xf, poly->m_vertices[i]);
		}

		renderer.DrawPolygon(vertices, vertexCount, color);
	}

	break;
	case b2Shape::e_edge:
	{
		b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
		int32 vertexCount;

		b2Vec2 vertices[b2_maxPolygonVertices];
		int i = 0;

		if (edge->m_hasVertex0)
		{
			vertices[i] = b2Mul(xf, edge->m_vertex0);
			i++;
		}
		vertices[i] = b2Mul(xf, edge->m_vertex1); i++;
		vertices[i] = b2Mul(xf, edge->m_vertex2); i++;
		if (edge->m_hasVertex3)
		{
			vertices[i] = b2Mul(xf, edge->m_vertex3);
			i++;
		}

		vertexCount = i;
		renderer.DrawPolygon(vertices, vertexCount, color);
	}

	break;

	}

}

//Para chamar a rotina de desenho das fixtures de um corpo
void DrawBody(b2Body *b, b2Color color)
{
	//Desenha todas as fixtures do corpo rígido
	b2Fixture *f;
	for (f = b->GetFixtureList(); f; f = f->GetNext())
	{
		DrawFixture(f, color);
	}
}




//Rotina de render (não é mais callback)
void Render()
{
	b2Body *b;
	glColor3f(1, 0, 0);
	glPointSize(5);
	glLineWidth(3);

	b2Color color; 
	color.r = 1.0;
	color.g = 0.0;
	color.b = 0.0;

	//PERCORRE A LISTA DE CORPOS RÍGIDOS DO MUNDO E CHAMA A ROTINA DE DESENHO PARA A LISTA DE FIXTURES DE CADA UM
	for (b = world->GetBodyList(); b; b = b->GetNext())
	{
		DrawBody(b, color);
	}
}

// Programa Principal 
int main(void)
{
	srand(time(0));

	//Inicialização da janela da aplicação
	GLFWwindow* window;

	//Setando a callback de erro
	glfwSetErrorCallback(error_callback);

	//Inicializando a lib
	if (!glfwInit())
		exit(EXIT_FAILURE);

	//Criando a janela
	window = glfwCreateWindow(640, 480, "This is mine now", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//Colocando a janela criada como sendo o contexto atual
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//Setando a callback de teclado
	glfwSetKeyCallback(window, key_callback);


	InitBox2D();


	while (!glfwWindowShouldClose(window)) //loop da aplicação :)
	{
		float ratio;
		int width, height;

		//aqui recupera o tamanho atual da janela, para correção do aspect ratio mais tarde
		glfwGetFramebufferSize(window, &width, &height);
		//setando a viewport da OpenGL, para ocupar toda a janela da aplicação
		glViewport(0, 0, width, height);

		// Limpa a janela de visualização com a cor branca
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		//Setando a matriz de projeção, para definir o Ortho2D (câmera ortográfica 2D), respeitando o aspect ratio
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		if (width >= height) 
		{
			ratio = width / (float)height;
			gluOrtho2D(xMin*ratio, xMax*ratio, yMin, yMax);
		}
		else
		{
			ratio = height / (float)width;
			gluOrtho2D(xMin, xMax, yMin*ratio, yMax*ratio);
		}
		
		//Setando a matriz de modelo, para mandar desenhar as primitivas
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Mandando simular e desenhar 	
		RunBox2D();
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}
