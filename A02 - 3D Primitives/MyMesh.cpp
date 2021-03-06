#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// -------------------------------
	// Create array of coords for the vertices
	std::vector<vector3> coneVertex;

	// Change in angle in radians
	GLfloat delta = 2 * PI / a_nSubdivisions;

	// Angle (start at 0 and go around)
	GLfloat theta = 0;

	// Create 2 vectors based on parameters inputted that will be used as starting points for all faces
	vector3 baseCenter(0, 0, -a_fHeight / 2);
	vector3 tip(0, 0, a_fHeight / 2);

	// Create the ring of vertices at the base
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, -a_fHeight / 2);

		theta += delta;

		coneVertex.push_back(temp);
	}

	// Create the triangle faces
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// The base face
		AddTri(baseCenter, coneVertex[(i + 1) % a_nSubdivisions], coneVertex[i]);
		// Faces from tip to the bottom ring of vertices
		AddTri(tip, coneVertex[i], coneVertex[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// ----------------------------------
	// Create vectors of vertices for both 'rings' or 'loops' 
	// of the cylinder to connect faces with later
	std::vector<vector3> topCylinderVertex;
	std::vector<vector3> botCylinderVertex;

	// Change in angle in radians
	GLfloat delta = 2 * PI / a_nSubdivisions;

	// Angle (start at 0 and go around)
	GLfloat theta = 0;

	vector3 topCenter(0, 0, a_fHeight / 2);
	vector3 botCenter(0, 0, -a_fHeight / 2);

	// Create vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 top = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, a_fHeight / 2);
		vector3 bot = vector3(cos(theta) * a_fRadius, sin(theta) * a_fRadius, -a_fHeight / 2);

		theta += delta;

		topCylinderVertex.push_back(top);
		botCylinderVertex.push_back(bot);
	}

	// Create the faces
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Top face
		AddTri(topCenter, topCylinderVertex[i], topCylinderVertex[(i + 1) % a_nSubdivisions]);
		// Bottom face
		AddTri(botCenter, botCylinderVertex[(i + 1) % a_nSubdivisions], botCylinderVertex[i]);
		// Vertical walls/faces
		AddQuad(botCylinderVertex[i], botCylinderVertex[(i + 1) % a_nSubdivisions], topCylinderVertex[i], topCylinderVertex[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// -------------------------------
	// 4 different vectors of vertices, each representing a 'ring' or 'loop' of vertices
	std::vector<vector3> topInnerVertex;
	std::vector<vector3> topOuterVertex;
	std::vector<vector3> botInnerVertex;
	std::vector<vector3> botOuterVertex;

	// Change in angle in radians
	GLfloat delta = 2 * PI / a_nSubdivisions;

	// Angle (start at 0 and go around)
	GLfloat theta = 0;

	vector3 topCenter(0, 0, a_fHeight / 2);
	vector3 botCenter(0, 0, -a_fHeight / 2);

	// Create vertices for 4 different vectors
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 topIn = vector3(cos(theta) * a_fInnerRadius, sin(theta) * a_fInnerRadius, a_fHeight / 2);
		vector3 topOut = vector3(cos(theta) * a_fOuterRadius, sin(theta) * a_fOuterRadius, a_fHeight / 2);
		vector3 botIn = vector3(cos(theta) * a_fInnerRadius, sin(theta) * a_fInnerRadius, -a_fHeight / 2);
		vector3 botOut = vector3(cos(theta) * a_fOuterRadius, sin(theta) * a_fOuterRadius, -a_fHeight / 2);

		theta += delta;

		topInnerVertex.push_back(topIn);
		topOuterVertex.push_back(topOut);
		botInnerVertex.push_back(botIn);
		botOuterVertex.push_back(botOut);
	}

	// Create the quad faces
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Inner vertical walls
		AddQuad(botInnerVertex[i], botInnerVertex[(i + 1) % a_nSubdivisions], botOuterVertex[i], botOuterVertex[(i + 1) % a_nSubdivisions]);
		// Top ring face
		AddQuad(topInnerVertex[(i + 1) % a_nSubdivisions], topInnerVertex[i], topOuterVertex[(i + 1) % a_nSubdivisions], topOuterVertex[i]);
		// Bottom ring face
		AddQuad(botInnerVertex[(i + 1) % a_nSubdivisions], botInnerVertex[i], topInnerVertex[(i + 1) % a_nSubdivisions], topInnerVertex[i]);
		// Outer vertical walls
		AddQuad(botOuterVertex[i], botOuterVertex[(i + 1) % a_nSubdivisions], topOuterVertex[i], topOuterVertex[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// --------------------------------
	// Create a 2D array to track the vertices going around the 'rings' made by subdivisons
	std::vector<std::vector<vector3>> torusVertices(a_nSubdivisionsA, std::vector<vector3>(a_nSubdivisionsB));

	// Change in angle in radians
	GLfloat deltaU = 2 * PI / a_nSubdivisionsA;
	GLfloat deltaV = 2 * PI / a_nSubdivisionsB;

	// Angle (start at 0 and go around)
	GLfloat thetaU = 0;
	GLfloat thetaV = 0;

	GLfloat middleOfTwoRadius = (a_fOuterRadius + a_fInnerRadius) / 2;
	GLfloat torusInsideRadius = a_fOuterRadius - a_fInnerRadius;

	// i represents the torus going vertically
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		// j represents the torus going horizontally
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			// Use the parametric equation to find all vertices of our torus
			vector3 temp1 = vector3((middleOfTwoRadius + (torusInsideRadius * cos(thetaV))) * cos(thetaU),
				(middleOfTwoRadius + (torusInsideRadius * cos(thetaV))) * sin(thetaU),
				torusInsideRadius * sin(thetaV));

			// Change angle V to go around its the vertical subdivision
			thetaV += deltaV;

			// Add the vertex to our 2D array
			torusVertices[i][j] = temp1;
		}
		// Change angle U to go to the next subdivision (horizontal)
		thetaU += deltaU;
	}

	// Create the quad faces with our 2D array
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			AddQuad(torusVertices[i][j], 
				torusVertices[(i + 1) % a_nSubdivisionsA][j], 
				torusVertices[i][(j + 1) % a_nSubdivisionsB],
				torusVertices[(i + 1) % a_nSubdivisionsA][(j + 1) % a_nSubdivisionsB]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// -------------------------------
	// 2D vector of vectors, 
	std::vector<std::vector<vector3>> sphereVertices(a_nSubdivisions, std::vector<vector3>(a_nSubdivisions));

	// Change in angle in radians
	GLfloat deltaT = (2 * PI) / a_nSubdivisions;
	GLfloat deltaP = PI / a_nSubdivisions;

	// Angle (start at 0 and go around)
	GLfloat theta = 0;
	GLfloat polar = 0;

	vector3 topCenter(0, 0, a_fRadius);
	vector3 botCenter(0, 0, -a_fRadius);

	// Create vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		for (int j = 0; j < a_nSubdivisions; j++)
		{
			vector3 temp = vector3(a_fRadius * cos(theta) * sin(polar),
				a_fRadius * sin(theta) * sin(polar),
				a_fRadius * cos(polar));

			theta += deltaT;

			sphereVertices[i][j] = temp;
		}
		polar += deltaP;
	}

	// Create the  faces
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Add the triangles to the poles
		AddTri(topCenter, sphereVertices[0][i], sphereVertices[0][(i + 1) % a_nSubdivisions]);
		AddTri(botCenter, sphereVertices[a_nSubdivisions - 1][(i + 1) % a_nSubdivisions], sphereVertices[a_nSubdivisions - 1][i]);
		// Add the quads to subdivisions
		for (int j = 0; j < a_nSubdivisions; j++)
		{
			AddQuad(sphereVertices[i][j], sphereVertices[(i + 1) % a_nSubdivisions][j], sphereVertices[i][(j + 1) % a_nSubdivisions], sphereVertices[(i + 1) % a_nSubdivisions][(j + 1) % a_nSubdivisions]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}