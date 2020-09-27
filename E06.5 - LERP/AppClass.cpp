#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Nick Malone - nmm3037@rit.edu";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(vector3(5.0f,3.0f,15.0f), ZERO_V3, AXIS_Y);

	m_pModel = new Simplex::Model();
	m_pModel->Load("Sorted\\WallEye.bto");
	
	m_stopsList.push_back(vector3(-4.0f, -2.0f, 5.0f));
	m_stopsList.push_back(vector3(1.0f, -2.0f, 5.0f));

	m_stopsList.push_back(vector3(-3.0f, -1.0f, 3.0f));
	m_stopsList.push_back(vector3(2.0f, -1.0f, 3.0f));

	m_stopsList.push_back(vector3(-2.0f, 0.0f, 0.0f));
	m_stopsList.push_back(vector3(3.0f, 0.0f, 0.0f));

	m_stopsList.push_back(vector3(-1.0f, 1.0f, -3.0f));
	m_stopsList.push_back(vector3(4.0f, 1.0f, -3.0f));

	m_stopsList.push_back(vector3(0.0f, 2.0f, -5.0f));
	m_stopsList.push_back(vector3(5.0f, 2.0f, -5.0f));

	m_stopsList.push_back(vector3(1.0f, 3.0f, -5.0f));
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

	// Draw the model
	m_pModel->PlaySequence();

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	//calculate the current position
	vector3 v3CurrentPos;
	
	//your code goes here

	// I mostly just used the code from Ex04 - LERP [code]...

	// Start and end points of each route
	vector3 v3Start;
	vector3 v3End;
	static uint route = 0;

	switch (route)
	{
	case 0:
		v3Start = m_stopsList[0];
		v3End = m_stopsList[1];
		break;
	case 1:
		v3Start = m_stopsList[1];
		v3End = m_stopsList[2];
		break;
	case 2:
		v3Start = m_stopsList[2];
		v3End = m_stopsList[3];
		break;
	case 3:
		v3Start = m_stopsList[3];
		v3End = m_stopsList[4];
		break;
	case 4:
		v3Start = m_stopsList[4];
		v3End = m_stopsList[5];
		break;
	case 5:
		v3Start = m_stopsList[5];
		v3End = m_stopsList[6];
		break;
	case 6:
		v3Start = m_stopsList[6];
		v3End = m_stopsList[7];
		break;
	case 7:
		v3Start = m_stopsList[7];
		v3End = m_stopsList[8];
		break;
	case 8:
		v3Start = m_stopsList[8];
		v3End = m_stopsList[9];
		break;
	case 9:
		v3Start = m_stopsList[9];
		v3End = m_stopsList[10];
		break;
	case 10:
		v3Start = m_stopsList[10];
		v3End = m_stopsList[0];
		break;
	}

	float fPercentage = static_cast<float>(MapValue(fTimer, 0.0f, 1.0f, 0.0f, 1.0f));

	v3CurrentPos = glm::lerp(v3Start, v3End, fPercentage);
	
	// Change route
	if (fPercentage >= 1.0f)
	{
		route++;
		fTimer = m_pSystem->GetDeltaTime(uClock);

		// Won't go out of range
		route %= m_stopsList.size();
	}
	//-------------------
	


	
	matrix4 m4Model = glm::translate(v3CurrentPos);
	m_pModel->SetModelMatrix(m4Model);

	m_pMeshMngr->Print("\nTimer: ");//Add a line on top
	m_pMeshMngr->PrintLine(std::to_string(fTimer), C_YELLOW);

	// Draw stops
	for (uint i = 0; i < m_stopsList.size(); ++i)
	{
		m_pMeshMngr->AddSphereToRenderList(glm::translate(m_stopsList[i]) * glm::scale(vector3(0.05f)), C_GREEN, RENDER_WIRE);
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pModel);
	//release GUI
	ShutdownGUI();
}