#include "AppClass.h"
void Application::InitVariables(void)
{
	m_pCameraMngr->SetPositionTargetAndUpward(vector3(0.0f, 0.0f, 30.0f), ZERO_V3, AXIS_Y);

	sides = 3.0f;
	radius = 0;
	backwards = 0.0f;

	MyMesh* pMesh = new MyMesh();
	m_pMeshList.push_back(pMesh);
	m_pMeshList[0]->GenerateDiamond(sides, 2, 1, C_GREEN_LIME);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	static float fAngle = 0.0f;

	//camera projection
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Model = ToMatrix4(m_qArcBall);

	// Create the timer
	static float fTimer = 0;
	static uint uClock = m_pSystem->GenClock();
	fTimer += m_pSystem->GetDeltaTime(uClock);

	// Generate a new diamond every second
	if (fTimer >= 1.0f)
	{
		sides++;
		radius++;
		backwards -= 1.5f;

		// Generate a new diamond shape with more sides than the last
		MyMesh* pMesh = new MyMesh();
		pMesh->GenerateDiamond(sides, 2, 1, C_GREEN_LIME);
		m_pMeshList.push_back(pMesh);

		// Reset the timer
		fTimer = 0.0f;
	}

	// A default vector which will have all coordinates changed
	// as it is used for the translation matrix to determine
	// placement of diamond on the orbit
	vector3 v3Position(0.0f, 0.0f, 0.0f);
	
	// Used to evenly space out diamonds around the orbit
	float tempAngle = fAngle;

	int diamonds = sides - 2;
	
	// Render each diamond
	for (int i = 0; i < diamonds; i++)
	{
		// Find placement around the orbit
		v3Position.x = glm::cos(glm::radians(tempAngle)) * radius;
		v3Position.y = glm::sin(glm::radians(tempAngle)) * radius;

		// All diamonds will be pushed back 1.5 units every time a diamond is created
		v3Position.z = backwards;

		// Create matrixes for translation and rotation to replicate loading screen
		matrix4 m4RotationWorld = glm::rotate(IDENTITY_M4, glm::radians(fAngle), vector3(0.0f, 0.0f, 1.0f));
		matrix4 m4RotationSelf = glm::rotate(IDENTITY_M4, glm::radians(fAngle), vector3(0.0f, 1.0f, 0.0f));
		matrix4 m4Translation = glm::translate(IDENTITY_M4, v3Position);

		// Finally render the diamond
		m4Model = m4RotationWorld * m4Translation * m4RotationSelf;
		m_pMeshList[i]->Render(m4Projection, m4View, m4Model);

		// Increment space between diamonds
		tempAngle += 360.0f / (sides - 2.0f);
	}
	
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();

	//Angle increment
	fAngle += 1.0f;
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);
	//release GUI
	ShutdownGUI();
}